/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:19:20 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 16:05:02 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <istream>
# include <iostream>
# include <string>
# include <algorithm>
# include <dirent.h>
# include <cstdio>

# include "../Utils/utils.hpp"
# include "../Config/BlocServer.hpp"
# include "../Requests/HttpLexer.hpp"
# include "../Requests/CGI.hpp"
# include "../Logger/Logger.hpp"
# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHanlder.hpp"
# include "../Http/RedirectHandler.hpp"

class Response
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        std::string                 _content_type;
        Client*                     _parent;
        std::string                 _response_cgi;
        ErrorHandler*               _err;
        
        std::string                 _handleMethod();
        std::string                 _handleDelete();
        std::string                 _handlePost();
        std::string                 _handleUpload(std::string uploadDir);
        std::string                 _handleGet();
        bool                        _handleGetCGI();
        std::string                 _generateAutoIndex(std::string fullpath);
        std::string                 _testIndex(std::string URI);
        void                        _initContentType(std::string file);
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
