/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:04:10 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:43:44 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Methods/DeleteHandler.hpp"

DeleteHandler::DeleteHandler(BlocServer* ctx, const BlocLocation* location_ctx,
    HttpLexer::parsedRequest req, ErrorHandler* err, Client* parent)
    :   _ctx(ctx),
        _location_ctx(location_ctx),
        _req(req),
        _err(err),
        _rep_parent(parent)
{

}

DeleteHandler::~DeleteHandler()
{}

std::string DeleteHandler::handleRequest()
{
    std::string fullpath;
    if (_location_ctx)
        fullpath = _location_ctx->getRootPath() + _req.path;
    else
        fullpath = _ctx->getRootPath() + _req.path;

    Logger::log(Logger::DEBUG, "DELETE target path: " + fullpath);

    if (access(fullpath.c_str(), F_OK) == 0)
    {
        if (_location_ctx && !_location_ctx->getCGIExtension().empty()
            && ((fullpath.size() >= 3 && (fullpath.substr(fullpath.size() - 3) == ".py" 
                    && _location_ctx->getCGIExtension() == ".py"))
                || ((fullpath.size() >= 4 && fullpath.substr(fullpath.size() - 4) == ".php")
                    && _location_ctx->getCGIExtension() == ".php")))
        {
            if (access(fullpath.c_str(), R_OK | X_OK) != 0)
                _err->createError(403, "Forbidden", "Request failed due to insufficient permissions");
            
            Logger::log(Logger::DEBUG, "Sending delete request to cgi.");
            CGI cgi_handler(std::string("DELETE"), _req, _req.contentType, _ctx, _location_ctx, _rep_parent);
            cgi_handler.exec();
            return ("CGI");
        }
        else
        {
            Logger::log(Logger::DEBUG, "native DELETE");

            std::string directory_path;
            std::string::size_type pos = fullpath.find_last_of("/");

            directory_path = fullpath.substr(0, pos);
            Logger::log(Logger::DEBUG, "Delete directory target: " + directory_path);

            if (access(directory_path.c_str(), W_OK | X_OK) != 0)
                _err->createError(403, "Forbidden", "Request failed due to insufficient permissions");

            int delete_res = std::remove(fullpath.c_str()); 
            if (delete_res == 0)
            {
                Logger::log(Logger::DEBUG, "Deleted " + fullpath);
                return (ResponseHandler::createResponse(200, "OK", ""));
            }
            else
                return (_err->createError(500, "Internal Server Error", ""));
        }
    }
    return (_err->createError(404, "Not Found", "The server cannot find the requested resource"));
    
}
