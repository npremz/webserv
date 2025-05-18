/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:42:04 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:42:04 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/ServerManager.hpp"

ServerManager::ServerManager()
{}

ServerManager::~ServerManager()
{}

void    ServerManager::initConfig(std::string config_src)
{
    Logger::log(Logger::INFO, "Loading config...");
    _config.parse(config_src);
    Logger::log(Logger::INFO, "Config loaded successfully.");
}

void    ServerManager::_initRouter()
{
    _router = _config.initRouter();
    // for (RouterMap::iterator it = _router.begin();
    // it != _router.end(); ++it)
    // {
    //     std::cout << (it->first).ip << " " << (it->first).port << std::endl;
    //     for (std::vector<BlocServer>::iterator s_it = (it->second).begin(); s_it != (it->second).end(); ++s_it)
    //     {
    //         s_it->print(2);
    //     }
    // }
}

bool    isDouble(const unsigned int val, std::vector<unsigned int> tab)
{
    for (std::vector<unsigned int>::iterator v_it = tab.begin(); v_it != tab.end(); v_it++)
    {
        if (val == *v_it)
            return true;
    }
    return false;
}

void    ServerManager::_initListenSockets()
{
    std::vector<unsigned int>    tmp;

    for (RouterMap::iterator it = _router.begin();
        it != _router.end(); ++it)
    {
        if (isDouble(it->first.port, tmp))
            continue;
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1)
            Logger::log(Logger::FATAL, "Initialisation error => socket error");
        if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => socket fcntl error");
        
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(it->first.port);
        sa.sin_addr.s_addr = INADDR_ANY;

        if (bind(socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => bind error");
        if (listen(socket_fd, 10) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => listen error");
        
        _listen_sockets.push_back(socket_fd);
        tmp.push_back(it->first.port);
    }
}

void    ServerManager::_initEpoll()
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
        Logger::log(Logger::FATAL, "Initialisation error => epoll creation error");
}

void    ServerManager::_linkEpollToListensFD()
{
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = *it;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, *it, &ev) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl error");
    }
}

int     ServerManager::_isListenSocket(int event_fd)
{
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        if (event_fd == *it)
            return (*it);
    }
    return (-1);
}

void    ServerManager::_run()
{
    while (true)
    {
        struct epoll_event events[16];
        int n = epoll_wait(_epoll_fd, events, 16, -1);
        if (n == -1)
            Logger::log(Logger::FATAL, "Initialisation error => epoll_wait error");
        
        for (int i = 0; i < n; ++i)
        {
            int f_socket_fd;
            if ((f_socket_fd = _isListenSocket(events[i].data.fd)) != -1) {
                int c_socket_fd = accept(f_socket_fd, 0, 0);
                if (fcntl(c_socket_fd, F_SETFL, O_NONBLOCK) == -1)
                    Logger::log(Logger::FATAL, "Initialisation error => fcntl c_socket error");
                epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = c_socket_fd;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, c_socket_fd, &ev) == -1)
                    Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl add c_socket error");
            } else {
                char buf[2048];
                int r = read(events[i].data.fd, buf, 255);
                printf("%s", buf);
                if (r > 0) {
                    write(events[i].data.fd, "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello\n", 44);
                }
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
                    Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl del c_socket error");
                close(events[i].data.fd);
            }
        }
    }
}

void    ServerManager::run()
{
    if (!this->_config.getIsLoaded())
        Logger::log(Logger::FATAL, "Can't run server => config not loaded");

    _initRouter();
    _initListenSockets();
    _initEpoll();
    _linkEpollToListensFD();
    _run();
}
