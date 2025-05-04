/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 14:45:08 by npremont          #+#    #+#             */
/*   Updated: 2025/05/04 14:45:08 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/main.hpp"

int server()
{
    // Creation instance epoll
    int epoll_fd = epoll_create1(0);
    
    // Creation du socket SOCK_STREAM => on specifie domain IPV4
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Met le socket en mode non bloquant, si le socket est pas dispo 
    // pour des operations les operations renverrons un retour indiquant 
    // c'est pas dispo mais ne bloqueront pas le programme
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);

    // Structure qui permet de definir les adresse qui pourront communiquer
    // avec notre socket
    struct sockaddr_in sa;
    // IPV4
    sa.sin_family = AF_INET;
    // On specifie le port
    sa.sin_port = htons(PORT);
    // On accepte toutes les entrees
    sa.sin_addr.s_addr = INADDR_ANY;

    // On link la stucture d'adresse avec notre socket
    bind(socket_fd, (struct sockaddr*)&sa, sizeof(sa));

    // On met le socket en mode ecoute, il peut de cette facon ecouter
    // les requetes et les mettre en file d'attente jusqu'a 10
    // file d'attente qui sera traitee par accept(), si il y a des nouvelles
    // connections au dela, les connections seront refusee
    listen(socket_fd, 10);

    // Structure pour configurer epoll (event et retours)
    epoll_event ev;
    // On veut que epoll se reveille quand le fd est lisible
    ev.events = EPOLLIN;
    // fd a renvoyer en cas d'event (si il y a quelque chose a lire)
    ev.data.fd = socket_fd;

    // Ajoute (EPOLL_CTL_ADD) a epoll_fd le socket fd avec le contexte ev
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    while (true)
    {
        // On attend jusqu'a 16 connections simultanees.
        struct epoll_event events[16];

        // "endors" l'epoll jusqu'a ce qu'il detecte un event sur le socket et rempli
        // le tableau events avec les details
        int n = epoll_wait(epoll_fd, events, 16, -1);

        // traitement des events recus
        for (int i = 0; i < n; ++i)
        {
            // si le fd de l'event correspond a notre socket d'ecoute
            if (events[i].data.fd == socket_fd) {
                // on creer un socket a partir de la liste d'ecoute de socket_fd
                int c_socket_fd = accept(socket_fd, 0, 0);
                // non bloquant
                fcntl(c_socket_fd, F_SETFL, O_NONBLOCK);
                // on adapte ev au nouveau socket
                ev.events = EPOLLIN;
                ev.data.fd = c_socket_fd;
                // on ajoute le nouveau socket a la liste de suveillance
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c_socket_fd, &ev);
            } else { // si le fd vient d'un socket client -> on repond
                char buf[256];
                // on lit sur le fd client
                int r = read(events[i].data.fd, buf, 255);
                // si il y avait bien une requete -> on ecrit la reponse sur le fd client
                if (r > 0) {
                    write(events[i].data.fd, "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello", 44);
                }
                // on ferme le fd
                close(events[i].data.fd);
                // on dit a epoll de ne plus ecouter ce fd -> fin de la connection
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }
}

int main (int ac, char* av[])
{
    if (2 < ac)
    {
        std::cerr << "This program takes one or no arguments." << std::endl;
        return (1);
    }

    (void)av;

    server();

    return (EXIT_SUCCESS);
}