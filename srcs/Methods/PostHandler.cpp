/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:04:02 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:25:07 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Methods/PostHandler.hpp"

PostHandler::PostHandler(BlocServer* ctx, const BlocLocation* location_ctx,
    HttpLexer::parsedRequest req, ErrorHandler* err, Client* parent)
    :   _ctx(ctx),
        _location_ctx(location_ctx),
        _req(req),
        _err(err),
        _rep_parent(parent)
{

}

PostHandler::~PostHandler()
{}

std::string PostHandler::_handleUpload(std::string uploadDir)
{
    Logger::log(Logger::DEBUG, "Target path: " + uploadDir);

    if ((_location_ctx && !_location_ctx->getCGIExtension().empty())
        && uploadDir.substr(uploadDir.find_last_of(".")) == _location_ctx->getCGIExtension())
    {
        Logger::log(Logger::DEBUG, "Forwarding raw body to CGI");
        CGI cgi_handler(std::string("POST"), _req, _req.contentType, _ctx, _location_ctx, _rep_parent);
        cgi_handler.exec();
        return ("CGI");
    }
    else
    {
        Logger::log(Logger::DEBUG, "No CGI configured. sending 405");
        return (_err->createError(405, "Method Not Allowed", "This server does not support direct form processing. Please configure a CGI handler"));
    }
}

std::string PostHandler::handleRequest()
{
    std::string fullPath;
    if (_location_ctx)
        fullPath = _location_ctx->getRootPath() + _req.path;
    else
        fullPath = _ctx->getRootPath() + _req.path;
    Logger::log(Logger::DEBUG, "POST target path: " + fullPath);

    if (isDirectory(fullPath) && _location_ctx 
        && _location_ctx->getCGIPass().find(_location_ctx->getCGIExtension()) == std::string::npos)
        return (_err->createError(403, "Forbidden", "POST not correctly configured for this route."));

    if (access(fullPath.c_str(), F_OK) == 0)
    {
        if (access(fullPath.c_str(), R_OK | X_OK) == 0)
            return (_handleUpload(fullPath));
        else
            return (_err->createError(403, "Forbidden", "Request failed due to insufficient permissions"));
    }
    return (_err->createError(404, "Not Found", "The server cannot find the requested resource"));
}


