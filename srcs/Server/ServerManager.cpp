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
    const std::vector<BlocServer>& servers = _config.getServersCtx();

    for (std::vector<BlocServer>::const_iterator bloc_it = servers.begin();
        bloc_it != servers.end(); ++bloc_it)
    {
        const std::vector<s_ip_port>& ip_tab = bloc_it->getIpTab();

        for (std::vector<s_ip_port>::const_iterator ip_it = ip_tab.begin();
            ip_it != ip_tab.end(); ++ip_it)
        {
            if (_router.find(*ip_it) != _router.end())
            {
                bool isInMap = false;
                for (std::vector<BlocServer>::const_iterator it = _router[*ip_it].begin();
                    it != _router[*ip_it].end(); ++it)
                {
                    if (*it == *bloc_it)
                        isInMap = true;
                }
                if (!isInMap)
                    _router[*ip_it].push_back(*bloc_it);
            }
            else
            {
                std::vector<BlocServer> tab;
                tab.push_back(*bloc_it);
                _router[*ip_it] = tab;
            }
        }
    }

    for (std::map<s_ip_port, std::vector<BlocServer>, s_ip_portCompare>::iterator it = _router.begin();
        it != _router.end(); ++it)
    {
        std::cout << (it->first).ip << " " << (it->first).port << std::endl;
        for (std::vector<BlocServer>::iterator s_it = (it->second).begin(); s_it != (it->second).end(); ++s_it)
        {
            s_it->print(2);
        }
    }
}

void    ServerManager::run()
{
    if (!this->_config.getIsLoaded())
        Logger::log(Logger::FATAL, "Can't run server -> config not loaded");

    _initRouter();
}
