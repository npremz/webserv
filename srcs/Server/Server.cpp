/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 23:05:16 by npremont          #+#    #+#             */
/*   Updated: 2025/05/06 23:05:16 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Server.hpp"

Server::Server()
{
    int epoll_fd = epoll_create1(0);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(socket_fd, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(2025);
    sa.sin_addr.s_addr = INADDR_ANY;

    bind(socket_fd, (struct sockaddr*)&sa, sizeof(sa));
    listen(socket_fd, 10);

    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

    while (true)
    {
        struct epoll_event events[16];
        int n = epoll_wait(epoll_fd, events, 16, -1);

        for (int i = 0; i < n; ++i)
        {
            if (events[i].data.fd == socket_fd) {
                int c_socket_fd = accept(socket_fd, 0, 0);
                fcntl(c_socket_fd, F_SETFL, O_NONBLOCK);
                ev.events = EPOLLIN;
                ev.data.fd = c_socket_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, c_socket_fd, &ev);
            } else {
                char buf[2048];
                int r = read(events[i].data.fd, buf, 255);
                printf("%s", buf);
                if (r > 0) {
                    write(events[i].data.fd, "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello", 44);
                }
                close(events[i].data.fd);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            }
        }
    }
}

Server::~Server()
{}