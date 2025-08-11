/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 10:24:50 by npremont          #+#    #+#             */
/*   Updated: 2025/08/11 09:55:22 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/Response.hpp"

Response::Response(BlocServer* ctx, HttpLexer::parsedRequest req, Client* parent) : 
    _ctx(ctx),
    _location_ctx(NULL),
    _req(req),
    _parent(parent)
{
    (void)_ctx;
    _setLocation();
    _err = new ErrorHandler(_ctx, _location_ctx, req);
}

Response::~Response()
{
    delete _err;
}

std::string Response::sendError(std::string error)
{
    return (_err->sendError(error));
}

bool    Response::_setLocation()
{
    if (_ctx->getLocationBlocs().size() == 0)
        return (false);
    for (std::vector<BlocLocation>::const_iterator it = _ctx->getLocationBlocs().begin(); 
    it < _ctx->getLocationBlocs().end(); it++)
    {
        std::string location = it->getLocationPath();
        if (*(location.end() - 1) != '/')
            location += "/";
        
        std::string path = _req.path;
        std::string fullpath = _ctx->getRootPath() + path;

        if (isDirectory(fullpath))
        {
            if (*(path.end() - 1) != '/')
                path += "/";
        } 
        else
        {
            std::string::size_type pos = path.find_last_of("/");
            path = path.substr(0, pos + 1);
        }

        if (location == path)
        {
            _location_ctx = &(*it);
            Logger::log(Logger::DEBUG, "Location found.");
            if (DEBUG_MODE)
                it->print(2);
            return (true);
        }
    }
    return (false);
}

int    Response::_isMethodSupportedByRoute()
{
    switch (_req.method)
    {
        case HttpLexer::HTTP_GET:
            if (_location_ctx)
            {
                if (_location_ctx->getGetMethod())
                    return (1);
            }
            else
                if (_ctx->getGetMethod())
                    return (1);
            break;
        case HttpLexer::HTTP_POST:
            if (_location_ctx)
            {
                if (_location_ctx->getPostMethod())
                    return (1);
            }
            else
                if (_ctx->getPostMethod())
                    return (1);
            break;
        case HttpLexer::HTTP_DELETE:
            if (_location_ctx)
            {
                if (_location_ctx->getDeleteMethod())
                    return (1);
            }
            else
                if (_ctx->getDeleteMethod())
                    return (1);
            break;
        case HttpLexer::HTTP_HEAD:  return (-1);    break;
        case HttpLexer::HTTP_PUT:   return (-1);    break;
        case HttpLexer::HTTP_CONNECT:   return (-1);    break;
        case HttpLexer::HTTP_OPTIONS:   return (-1);    break;
        case HttpLexer::HTTP_PATCH:     return (-1);    break;
        case HttpLexer::HTTP_TRACE:     return (-1);    break;
        default:
            return (0);
    }
    return false;
}

bool    Response::_isPathLegal()
{
    std::string request_root =
        _location_ctx ? _location_ctx->getRootPath() : _ctx->getRootPath();

    Logger::log(Logger::DEBUG, "request root: " + request_root);
    Logger::log(Logger::DEBUG, "request path: " + _req.path);

    if (normalize_path(request_root, _req.path).empty())
        return (false);
    return (true);
}


std::string Response::_handleMethod()
{
    switch (_req.method)
    {
        case HttpLexer::HTTP_GET:
        {
            GetHandler get(_ctx, _location_ctx, _req, _err, _parent);
            return (get.handleRequest());
            break;
        }
        case HttpLexer::HTTP_POST:
        {
            PostHandler post(_ctx, _location_ctx, _req, _err, _parent);
            return (post.handleRequest());
            break;
        }
        case HttpLexer::HTTP_DELETE:
        {
            DeleteHandler del(_ctx, _location_ctx, _req, _err, _parent);
            return (del.handleRequest());
            break;
        }
        default:
            return ("waf");
    }
    return ("waf");
}

std::string Response::createResponseSTR()
{
    if (_req.endstatus >= 400)
        return (_err->handleLexerError());  
    if (!_isPathLegal())
        return (_err->createError(403, "Forbidden", "Illegal request path."));
    if (_location_ctx && _location_ctx->isRedirectSet())
        return (RedirectHandler::createRedirect(_location_ctx->getRedirectCode(),
            _location_ctx->getRedirectUrl()));
    if (_location_ctx && _location_ctx->getClientMaxBodySize() < _req.expectedoctets)
        return (_err->createError(413, "Content Too Large",
            "The request entity was larger than limits defined by server."));
    else if (_ctx->getClientMaxBodySize() < _req.expectedoctets)
        return (_err->createError(413, "Content Too Large",
            "The request entity was larger than limits defined by server."));
    int is_method_supported = _isMethodSupportedByRoute();
    if (is_method_supported == 0)
        return (_err->createError(405, "Method Not Allowed",
            "The request method is not allowed on this route or doesn't exists."));
    if (is_method_supported == -1)
        return (_err->createError(501, "Not implemented",
            "The request method is known by the server but is not supported by the target resource. ")); 
    return (_handleMethod());
    return (ResponseHandler::createResponse(200, "OK", "Hello World"));
    
}

std::string Response::createCGIResponseSTR(int cgi_fd)
{
    char            buf[MAX_CHUNK_SIZE];
    ssize_t          read_bytes;

    if ((read_bytes = read(cgi_fd, buf, MAX_CHUNK_SIZE)) > 0) {
        _response_cgi.append(buf, read_bytes);
        return ("Not complete.");
    }

    if (read_bytes < 0) {
        Logger::log(Logger::WARNING, "CGI pipe read returned -1.");
    }
    
    Logger::log(Logger::DEBUG, "CGI pipe content:");
    if (DEBUG_MODE)
        std::cout << _response_cgi << std::endl;

    if (_location_ctx->getCGIExtension() == ".php")
    {
        _response_cgi = buildHttpResponseFromCGI(_response_cgi);
        Logger::log(Logger::DEBUG, "php CGI formated content:");
        if (DEBUG_MODE)
            std::cout << _response_cgi << std::endl;
    }

    return (_response_cgi);
}
