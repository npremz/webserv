/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:19:20 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:37:17 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <istream>
# include <iostream>
# include <string>
# include <algorithm> // x
# include <dirent.h> // x
# include <cstdio>

# include "../Utils/utils.hpp"
# include "../Config/BlocServer.hpp"
# include "../Requests/HttpLexer.hpp"
# include "../Requests/CGI.hpp"
# include "../Logger/Logger.hpp"
# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHandler.hpp"
# include "../Http/RedirectHandler.hpp"
# include "../Methods/GetHandler.hpp"
# include "../Methods/PostHandler.hpp"
# include "../Methods/DeleteHandler.hpp"

class Response
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        Client*                     _parent;
        std::string                 _response_cgi;
        ErrorHandler*               _err;
        
        std::string                 _handleMethod();
        int                         _isMethodSupportedByRoute();
        bool                        _setLocation();
        bool                        _isPathLegal();

    public:
        Response(BlocServer* ctx, HttpLexer::parsedRequest req, Client* parent);
        ~Response();

        std::string createResponseSTR();
        std::string createCGIResponseSTR(int cgi_fd);
        std::string sendError(std::string error);
};

#endif
