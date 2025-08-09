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
# include <list>
# include <sys/types.h>
# include <map>
# include <signal.h>
# include <sys/wait.h>

# include "../Config/ParserConfig.hpp"
# include "../Utils/utils.hpp"
# include "../Server/Client.hpp"

class Client;

class ServerManager
{
    private:
        ParserConfig                    _config;
        RouterMap                       _router;
        std::vector<int>                _listen_sockets;
        int                             _epoll_fd;
        int                             _exit_pipe[2];
        std::list<int>                  _client_fds;
        std::map<int, Client*>          _clients;
        std::multimap<Client*, int>     _cgi_map;

        void                _initRouter();
        void                _initListenSockets();
        void                _initExitPipe();
        void                _initEpoll();
        void                _linkEpollToListensFD();
        void                _linkEpollToExitPipe();
        void                _run();
        void                _addClient(int fd, u_int32_t ip);
        void                _removeClient(int fd);
        Client*             _isCGIClient(int fd);
        void                _closeAllClients();
        void                _logRunningInfos();
        void                _cleanup();
        void                _sweepTimeout();
        
        int         _isListenSocket(int event_fd); // -1 if not found

    public:
        ServerManager();
        ~ServerManager();

        static ServerManager    *instance;
        static void             signalHandler(int signum);

        bool                    isRunning;

        void                    initConfig(std::string config_src);
        void                    run();
        void                    stop();

        void                    addCGIlink(Client* client, int cgi_fd);
        void                    removeCGILink(int cgi_fd);

        int               getEpollFd() const;

};

#endif
