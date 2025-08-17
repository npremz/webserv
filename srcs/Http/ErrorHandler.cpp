/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 10:38:16 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 16:04:30 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Http/ErrorHandler.hpp"

ErrorHandler::ErrorHandler(BlocServer* ctx, const BlocLocation* location_ctx,
    HttpLexer::parsedRequest req) 
    :   _ctx(ctx),
        _location_ctx(location_ctx),
        _req(req)
{}

ErrorHandler::~ErrorHandler()
{}

std::string ErrorHandler::getCustomErrorPath(unsigned int code)
{
    if (!_ctx)
        return ("");
    for (std::map<int, std::string>::const_iterator it = _ctx->getErrorPages().begin();
        it != _ctx->getErrorPages().end(); ++it)
    {
        if (it->first == (int)code)
            return (it->second);
    }
    return ("");
}

std::string ErrorHandler::createCustomError(unsigned int code, std::string error_page,
    std::string error_msg)
{
    std::string root = _location_ctx ? _location_ctx->getRootPath() : _ctx->getRootPath();

    std::ifstream file((root + error_page).c_str(), std::ios::binary);
    if (!file.is_open())
        return createError(500, "Internal Server Error",
            "The HTTP 500 Internal Server Error server error response status code indicates that the server encountered an unexpected condition that prevented it from fulfilling the request.");
    
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer;
    if (size > 0)
    {
        buffer.resize(size);
        file.read(&buffer[0], size);
    }

    std::ostringstream oss_header;
    oss_header << "HTTP/1.1 " << code << " " << error_msg << "\r\n";
    oss_header << "Content-Type: text/html\r\n";
    oss_header << "Content-Length: " << buffer.size() << "\r\n";
    oss_header << "\r\n"; 

    std::string response = oss_header.str();
    response.append(buffer.data(), buffer.size());

    return (response);
}

std::string ErrorHandler::createError(unsigned int code, std::string error,
    std::string bodyStr)
{
    std::string error_page = getCustomErrorPath(code);
    if (error_page.size() > 0)
        return (createCustomError(code, error_page, error));

    std::ostringstream oss;
    std::ostringstream oss_header;
    std::ostringstream oss_body;

    oss_header << "HTTP/1.1 " << code << " " << error << "\r\n";
    oss_body << "<html><head><title>" << code << " " << error << "</title></head><body><h1>"
             << code << " " << error
             << "</h1><p>"
             << bodyStr
             << "</p></body></html>"
             << "\r\n\r\n";

    oss << oss_header.str()
        << "Content-Type: text/html\r\n"
        << "Content-length: " << oss_body.str().size() << "\r\n";
    if (code == 500)
        oss << "Connection: close\r\n";
    oss << "\r\n"
        << oss_body.str();

    Logger::log(Logger::DEBUG, oss.str());

    return (oss.str());
}

std::string ErrorHandler::sendError(std::string error)
{

    std::string msg;
    switch (_req.endstatus)
    {
        case 400: msg = "Bad Request"; break;
        case 401: msg = "Unauthorized"; break;
        case 403: msg = "Forbidden"; break;
        case 404: msg = "Not Found"; break;
        case 405: msg = "Method Not Allowed"; break;
        case 408: msg = "Request Timeout"; break;
        case 413: msg = "Payload Too Large"; break;
        case 500: msg = "Internal Server Error"; break;
        case 501: msg = "Not Implemented"; break;
        case 502: msg = "Bad Gateway"; break;
        case 503: msg = "Service Unavailable"; break;
        default:
        {
            std::ostringstream oss;
            oss << "Unknown HTTP Error (" << _req.endstatus << ")";
            msg = oss.str();
        }
    }
    return (createError(_req.endstatus, msg, error));
}

std::string ErrorHandler::handleLexerErrors()
{
    Logger::log(Logger::DEBUG, "req endstatus after lexer >= 400");
    switch (_req.endstatus)
    {
        case 400:
            return (createError(400, "Bad Request",
                "The server cannot or will not process the request due to something that is perceived to be a client error"));
            break;
        case 413:
            return (createError(413, "Content Too Large",
                "The request body is larger than limits defined by server"));
    }
    return (createError(400, "Bad Request",
        "The server cannot or will not process the request due to something that is perceived to be a client error"));
}
