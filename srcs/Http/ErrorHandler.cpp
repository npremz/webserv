/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 10:38:16 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 14:23:59 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Http/ErrorHandler.hpp"

ErrorHandler::ErrorHandler(BlocServer* ctx, const BlocLocation* location_ctx) 
    :   _ctx(ctx),
        _location_ctx(location_ctx)
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