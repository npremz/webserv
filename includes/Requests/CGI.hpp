/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 17:12:58 by npremont          #+#    #+#             */
/*   Updated: 2025/06/20 18:39:59 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <vector>
# include <iostream>
# include <stdio.h>

# include "HttpLexer.hpp"
# include "../Config/BlocServer.hpp"
# include "../Config/BlocLocation.hpp"
# include "../Logger/Logger.hpp"
# include "../Utils/utils.hpp"
# include "../Server/Client.hpp"

class Client;

class CGI
{
    private:
        std::vector<char *>         _env_tab;
        std::vector<char *>         _argv;
        std::string                 _method;
        std::string                 _file_name;
        HttpLexer::parsedRequest    _req;
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        Client*                     _client;
        int                         _cgi_pipe[2];
        std::string                 _content_type;

        void                    _initEnvTab();
        void                    _initArgv();

    public:
        CGI(std::string method, HttpLexer::parsedRequest req,
                const std::string& content_type, BlocServer* ctx,
                const BlocLocation* location_ctx, Client* parent);
        ~CGI();

        void    exec();
};

#endif
