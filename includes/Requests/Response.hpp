/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:19:20 by npremont          #+#    #+#             */
/*   Updated: 2025/08/03 14:11:49 by npremont         ###   ########.fr       */
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

class Response
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        std::string                 _content_type;
        Client*                     _parent;
        std::string                 _response_cgi;

        std::string                 _createError(unsigned int code, std::string error,
                                        std::string bodyStr);
        std::string                 _createCustomError(unsigned int code, std::string error_page,
                                        std::string error_msg);
        std::string                 _createResponse(unsigned int code, std::string msg,
                                        const std::string& bodyStr);
        std::string                 _createRedirect(unsigned int code, const std::string& url);
        std::string                 _isCustomError(unsigned int code);

        std::string                 _handleLexerErrors();
        std::string                 _handleMethod();
        std::string                 _handleDelete();
        std::string                 _handlePost();
        std::string                 _handlePostContentType(std::string fullPath);
        std::string                 _handleFilePost(std::string fullPath);
        std::string                 _handleFormSub(std::string fullPath);
        std::string                 _handleUpload(std::string uploadDir);
        std::string                 _handleNewPost(std::string fullPath);
        std::string                 _handleGet();
        bool                        _handleGetCGI();
        std::string                 _generateAutoIndex(std::string fullpath);
        std::string                 _testIndex(std::string URI);
        void                        _initContentType(std::string file);
        bool                        _isUploadAllowed();
        bool                        _isMethodSupportedByRoute();
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
