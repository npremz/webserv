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

# include "../Config/ParserConfig.hpp"
# include "../Utils/utils.hpp"


class ServerManager
{
    private:
        ParserConfig                                        _config;
        std::map<s_ip_port, std::vector<BlocServer>, s_ip_portCompare>   _router;

        void    _initRouter();

    public:
        ServerManager();
        ~ServerManager();

        void    initConfig(std::string config_src);
        void    run();
};

#endif
