/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:19:20 by npremont          #+#    #+#             */
/*   Updated: 2025/06/19 19:04:09 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <istream>
# include <iostream>
# include <string>
# include <dirent.h>

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

        std::string                 _createError(unsigned int code, std::string error,
                                        std::string bodyStr);
        std::string                 _createResponse(unsigned int code, std::string msg,
                                        const std::string& bodyStr);

        std::string                 _handleLexerErrors();
        std::string                 _handleMethod();
        std::string                 _handlePost();
        std::string                 _handleDirPost(std::string fullPath);
        std::string                 _handleFilePost(std::string fullPath);
        std::string                 _handleFormSub(std::string fullPath);
        std::string                 _handleMultiUpload(std::string uploadDir);
        std::string                 _handleNewPost(std::string fullPath);
        std::string                 _handleGet();
        bool                        _handleGetCGI();
        std::string                 _generateAutoIndex(std::string fullpath);
        std::string                 _testIndex(std::string URI);
        void                        _initContentType(std::string file);
        bool                        _isUploadAllowed();
        bool                        _isMethodSupportedByRoute();
        bool                        _setLocation();

    public:
        Response(BlocServer* ctx, HttpLexer::parsedRequest req, Client* parent);
        ~Response();

        std::string createResponseSTR();
        std::string createCGIResponseSTR(int cgi_fd);
};

#endif
