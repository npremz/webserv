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
    for (RouterMap::iterator it = _router.begin();
    it != _router.end(); ++it)
    {
        std::cout << (it->first).ip << " " << (it->first).port << std::endl;
        for (std::vector<BlocServer>::iterator s_it = (it->second).begin(); s_it != (it->second).end(); ++s_it)
        {
            s_it->print(2);
        }
    }
}

void    ServerManager::_initListenSockets()
{
    for (RouterMap::iterator it = _router.begin();
        it != _router.end(); ++it)
    {
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1)
            Logger::log(Logger::FATAL, "Initialisation error => socket error");
        
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(it->first.port);
        sa.sin_addr.s_addr = htonl(it->first.ip);

        if (bind(socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => bind error");
        if (listen(socket_fd, 10) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => listen error");
        
        _listen_sockets.push_back(socket_fd);
    }
}

void    ServerManager::run()
{
    if (!this->_config.getIsLoaded())
        Logger::log(Logger::FATAL, "Can't run server => config not loaded");

    _initRouter();
    _initListenSockets();
}
