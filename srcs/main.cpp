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

int main (int ac, char* av[])
{
    if (2 < ac)
    {
        std::cerr << "This program takes one or no arguments." << std::endl;
        return (1);
    }
    
    std::string config_src = "config_files/default.conf";

    if (av[1])
        config_src = av[1];

    try
    {
        ServerManager webserv;
        ServerManager::instance = &webserv;
        webserv.initConfig(config_src);
        signal(SIGINT, ServerManager::signalHandler);
        signal(SIGTERM, ServerManager::signalHandler);
        signal(SIGCHLD, ServerManager::signalHandler);
        webserv.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return (EXIT_SUCCESS);
}
