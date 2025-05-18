/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:42:09 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:42:09 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include <iostream>
# include <cstdlib>
# include <cstring>
# include <cstdio>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/epoll.h>
# include <string.h>
# include <errno.h>

# include "../Config/ParserConfig.hpp"
# include "../Utils/utils.hpp"


class ServerManager
{
    private:
        ParserConfig        _config;
        RouterMap           _router;
        std::vector<int>    _listen_sockets;
        int                 _epoll_fd;

        void    _initRouter();
        void    _initListenSockets();
        void    _initEpoll();
        void    _linkEpollToListensFD();
        void    _run();

        int     _isListenSocket(int event_fd); // -1 if not found

    public:
        ServerManager();
        ~ServerManager();

        void    initConfig(std::string config_src);
        void    run();
};

#endif
