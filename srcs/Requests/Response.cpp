/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 10:24:50 by npremont          #+#    #+#             */
/*   Updated: 2025/06/17 14:19:28 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/Response.hpp"

Response::Response(BlocServer* ctx, HttpLexer::parsedRequest req) : 
    _ctx(ctx),
    _location_ctx(NULL),
    _req(req)
{
    (void)_ctx;
}

Response::~Response()
{}

std::string Response::_createError(unsigned int code, std::string error, std::string bodyStr)
{
    std::ostringstream oss;
    std::ostringstream oss_header;
    std::ostringstream oss_body;

    oss_header << "HTTP/1.1 " << code << " " << error << "\r\n";
    oss_body << "<html><body><h1>"
             << code << " " << error
             << "</h1><p>"
             << bodyStr
             << "</p></body></html>"
             << "\r\n\r\n";

    oss << oss_header.str()
        << "Content-Type: text/html\r\n"
        << "Content-length: " << oss_body.str().size() << "\r\n\r\n"
        << oss_body.str();

    Logger::log(Logger::DEBUG, oss.str());

    return (oss.str());
}

std::string Response::_createResponse(unsigned int code, std::string msg, std::string bodyStr)
{
    std::ostringstream oss;
    std::ostringstream oss_header;
    std::ostringstream oss_body;

    oss_header << "HTTP/1.1 " << code << " " << msg << "\r\n";
    oss_body << bodyStr;

    oss << oss_header.str()
        << "Content-Type: text/html\r\n"
        << "Content-length: " << oss_body.str().size() << "\r\n\r\n"
        << oss_body.str();

    Logger::log(Logger::DEBUG, oss.str());

    return (oss.str());
}

std::string Response::_handleLexerErrors()
{
    Logger::log(Logger::DEBUG, "req endstatus after lexer >= 400");
    std::cout << _req.endstatus;
    switch (_req.endstatus)
    {
        case 400:
            return (_createError(400, "Bad Request",
                "The server cannot or will not process the request due to something that is perceived to be a client error"));
            break;
        case 413:
            return (_createError(413, "Content Too Large",
                "The request body is larger than limits defined by server"));
    }
    return (_createError(400, "Bad Request",
        "The server cannot or will not process the request due to something that is perceived to be a client error"));
}

bool    Response::_isLocation()
{
    if (_ctx->getLocationBlocs().size() == 0)
        return (false);
    for (std::vector<BlocLocation>::const_iterator it = _ctx->getLocationBlocs().begin(); 
        it < _ctx->getLocationBlocs().end(); it++)
    {
        if (it->getLocationPath() == _req.path)
        {
            _location_ctx = &(*it);
            return (true);
        }
    }
    return (false);
}

bool    Response::_isMethodSupportedByRoute()
{
    switch (_req.method)
    {
        case HttpLexer::HTTP_GET:
            if (_location_ctx)
            {
                if (_location_ctx->getGetMethod())
                    return (true);
            }
            else
                if (_ctx->getGetMethod())
                    return (true);
            break;
        case HttpLexer::HTTP_POST:
            if (_location_ctx)
            {
                if (_location_ctx->getPostMethod())
                    return (true);
            }
            else
                if (_ctx->getPostMethod())
                    return (true);
            break;
        case HttpLexer::HTTP_DELETE:
            if (_location_ctx)
            {
                if (_location_ctx->getDeleteMethod())
                    return (true);
            }
            else
                if (_ctx->getDeleteMethod())
                    return (true);
            break;
        case HttpLexer::HTTP_HEAD:
            return (false);
            break;
        case HttpLexer::HTTP_PUT:
            return (false);
            break;
        case HttpLexer::HTTP_CONNECT:
            return (false);
            break;
        case HttpLexer::HTTP_OPTIONS:
            return (false);
            break;
        case HttpLexer::HTTP_PATCH:
            return (false);
            break;
        case HttpLexer::HTTP_TRACE:
            return (false);
            break;
        default:
            return (false);
    }
    return false;
}

std::string Response::createResponseSTR()
{
    
    if (_req.endstatus >= 400)
        return (_handleLexerErrors());
    if (_isLocation())
        _location_ctx->print(2);
    if (!_isMethodSupportedByRoute())
        return (_createError(405, "Method Not Allowed",
            "The request method is known by the server but is not supported by the target resource. "));
    return (_createResponse(200, "OK", "Hello World"));
    
}
