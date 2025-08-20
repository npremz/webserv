/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:04:02 by npremont          #+#    #+#             */
/*   Updated: 2025/08/20 17:32:33 by npremont         ###   ########.fr       */
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

std::string PostHandler::_handlePlainText()
{
    Logger::log(Logger::DEBUG, "Handling plain/text");

    try {
        std::time_t now = std::time(0);
        std::tm* timeinfo = std::localtime(&now);
    
        std::ostringstream filename;
        filename << "file_" 
                << (timeinfo->tm_year + 1900)
                << (timeinfo->tm_mon + 1)
                << timeinfo->tm_mday
                << "_"
                << timeinfo->tm_hour
                << timeinfo->tm_min
                << timeinfo->tm_sec
                << ".txt";
    
        std::ofstream file((_fullpath + "/" + filename.str()).c_str());
        file << _req.body;
        file.close();

        Logger::log(Logger::INFO, "File " + _fullpath + "/" + filename.str() + " saved.");
    }
    catch (const std::exception& e)
    {
        return (_err->createError(500, "Internal Server Error", e.what()));
    }

    return (ResponseHandler::createResponse(201, "Created",
        "Ressource created successfully."));
}

std::string PostHandler::_handleMultiPart()
{
    Logger::log(Logger::DEBUG, "Handling multipart/form-data");
    PostMultiPartHandler mp_hdl(_req, _fullpath);

    if (mp_hdl.parseMultipartData() == false)
        return _err->createError(400, "Bad Request",
            "Invalid request format for multipart");

    
    if (mp_hdl.saveUploadedFiles() == false)
        return _err->createError(500, "Internal Server Error",
            "The server occured an intern error while attempting to upload the files.");

    return (ResponseHandler::createResponse(201, "Created",
        "Ressource created successfully."));
}

std::string PostHandler::_handleNativePost()
{
    if (!isDirectory(_fullpath))
        return (_err->createError(405, "Method Not Allowed",
            "This server does not allow native POST on files."));

    if (_location_ctx && _location_ctx->getUploadEnable() == "off")
        return (_err->createError(403, "Forbidden",
            "Upload not allowed on this route."));

    if (_location_ctx && !(_location_ctx->getUploadPath().empty()))
        _fullpath = _fullpath + _location_ctx->getUploadPath();
    
    Logger::log(Logger::DEBUG, "Upload path: " + _fullpath);
    
    if (_req.contentType == "text/plain")
        return (_handlePlainText());
    if (_req.contentType.find("multipart/form-data") != std::string::npos)
        return (_handleMultiPart());

    return (_err->createError(415, "Unsupported Media Type",
        "The server refused to accept the request because the message content format is not supported"));
}

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
        Logger::log(Logger::DEBUG, "Native Post.");
        return (_handleNativePost());
        Logger::log(Logger::DEBUG, "No CGI configured. sending 405");
        return (_err->createError(405, "Method Not Allowed", "This server does not support direct form processing. Please configure a CGI handler"));
    }
}

std::string PostHandler::handleRequest()
{
    if (_location_ctx)
        _fullpath = _location_ctx->getRootPath() + _req.path;
    else
        _fullpath = _ctx->getRootPath() + _req.path;
    Logger::log(Logger::DEBUG, "POST target path: " + _fullpath);

    if (isDirectory(_fullpath) && _location_ctx 
        && _location_ctx->getCGIPass().find(_location_ctx->getCGIExtension()) == std::string::npos)
        return (_err->createError(403, "Forbidden", "POST not correctly configured for this route."));

    if (access(_fullpath.c_str(), F_OK) == 0)
    {
        if (access(_fullpath.c_str(), R_OK | X_OK) == 0)
            return (_handleUpload(_fullpath));
        else
            return (_err->createError(403, "Forbidden", "Request failed due to insufficient permissions"));
    }
    return (_err->createError(404, "Not Found", "The server cannot find the requested resource"));
}


