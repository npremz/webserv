/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:04:04 by npremont          #+#    #+#             */
/*   Updated: 2025/08/12 10:56:46 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Methods/GetHandler.hpp"

GetHandler::GetHandler(BlocServer* ctx, const BlocLocation* location_ctx,
    HttpLexer::parsedRequest req, ErrorHandler* err, Client* parent)
    :   _ctx(ctx),
        _location_ctx(location_ctx),
        _req(req),
        _err(err),
        _rep_parent(parent)
{

}

GetHandler::~GetHandler()
{}

std::string GetHandler::_testIndex(std::string URI)
{
    if (_location_ctx)
    {
        for (std::vector<std::string>::const_iterator it = _location_ctx->getIndex().begin();
            it < _location_ctx->getIndex().end(); it++)
        {
            if (access((URI + (*it)).c_str(), R_OK) == 0)
                return ((*it));
        }
    }
    else
    {
        for (std::vector<std::string>::const_iterator it = _ctx->getIndex().begin();
            it < _ctx->getIndex().end(); it++)
        {
            if (access((URI + (*it)).c_str(), R_OK) == 0)
                return ((*it));
        }
    }
    return ("none");
}

void    GetHandler::_initContentType(std::string file)
{
    Logger::log(Logger::DEBUG, file);
    size_t dot_pos = file.find_last_of('.');
    std::string ext;
    if (dot_pos != std::string::npos)
        ext = file.substr(dot_pos);
    else 
        ext = "";

    Logger::log(Logger::DEBUG, "response ext: " + ext);

    if (".html" == ext || ".htm" == ext)
        _content_type = "text/html";
    else if (".css" == ext)
        _content_type = "text/css";
    else if (".js" == ext)
        _content_type = "application/javascript";
    else if (".txt" == ext)
        _content_type = "text/plain";
    else if (".png" == ext)
        _content_type = "image/png";
    else if (".gif" == ext)
        _content_type = "image/gif";
    else if (".ico" == ext)
        _content_type = "image/x-icon";
    else if (".jpeg" == ext || ".jpg" == ext)
        _content_type = "image/jpeg";
    else if (".webp" == ext)
        _content_type = "image/webp";
    else if (".svg" == ext)
        _content_type = "image/svg+xml";
    else if (".pdf" == ext)
        _content_type = "application/pdf";
    else if (".php" == ext)
        _content_type = "cgi/php";
    else if (".py" == ext)
        _content_type = "cgi/py";
    else
        _content_type = "application/octet-stream";
    
    Logger::log(Logger::DEBUG, _content_type);
}

std::string GetHandler::_generateAutoIndex(std::string fullpath)
{
    std::ostringstream html;
    html << "<html><head><title>Index of " << _req.path << "</title></head><body>";
    html << "<h1>Index of " << _req.path << "</h1><ul>";

    DIR* dir = opendir(fullpath.c_str());
    if (!dir) 
        return (_err->createError(403, "Forbidden", "The client does not have access rights to the content"));

    std::vector<std::string> entries;
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL) {
        entries.push_back(entry->d_name);
    }
    closedir(dir);

    std::sort(entries.begin(), entries.end());

    for (std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it) {
        std::string url = _req.path;
        if (*(url.end() - 1) != '/')
            url += "/";
        url += *it;

        Logger::log(Logger::DEBUG, "url: " + url);

        html << "<li><a href=\"" << url << "\">"
             << *it << "</a></li>";
    }

    html << "</ul></body></html>";
    return ResponseHandler::createResponse(200, "OK", html.str(), _content_type);
}

bool GetHandler::_handleGetCGI()
{
    if (!_location_ctx)
        return (false);
    
    Logger::log(Logger::DEBUG, "_location_ctx for CGi checked.");

    if (!((_content_type == "cgi/py" && _location_ctx->getCGIExtension() == ".py") 
        || (_content_type == "cgi/php" && _location_ctx->getCGIExtension() == ".php")))
        return (false);

    Logger::log(Logger::DEBUG, "CGI detected with: " + _location_ctx->getCGIExtension());

    CGI cgi_handler(std::string("GET"), _req, _content_type, _ctx, _location_ctx, _rep_parent);
    cgi_handler.exec();

    return (true);
}

std::string GetHandler::handleRequest()
{
    std::string fullPath;
    if (_location_ctx)
        fullPath = _location_ctx->getRootPath() + _req.path;
    else
        fullPath = _ctx->getRootPath() + _req.path;
    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) == 0) {
        if (S_ISDIR(pathStat.st_mode))
        {
            Logger::log(Logger::DEBUG, "Path is a directory.");
            std::string indexPath = _testIndex(fullPath);
            Logger::log(Logger::DEBUG, "Path of index: " + indexPath);
            if (indexPath != "none")
                _req.path += indexPath;
            std::ifstream indexFile((fullPath + indexPath).c_str());
            if (indexFile.is_open())
            {
                _initContentType(_req.path);
                if (_handleGetCGI())
                    return ("CGI");
                std::ostringstream oss;
                oss << indexFile.rdbuf();
                return ResponseHandler::createResponse(200, "OK", oss.str(), _content_type);
            } 
            else if (_location_ctx)
            {
                if (_location_ctx->getAutoindex())
                    return (_generateAutoIndex(fullPath));
                else
                    return (_err->createError(403, "Forbidden", "The client does not have access rights to the content"));
            }
            else if (_ctx->getAutoindex())
            {
                return (_generateAutoIndex(fullPath));
            } 
            else
            {
                return (_err->createError(403, "Forbidden", "The client does not have access rights to the content"));
            }
        } 
        else 
        {
            Logger::log(Logger::DEBUG, "Fullpath of response: " + fullPath);
            _initContentType(fullPath);
            if (_handleGetCGI())
                return ("CGI");

            std::ifstream file(fullPath.c_str(), std::ios::binary);
            if (!file.is_open())
                return _err->createError(403, "Forbidden", "The client does not have access rights to the content");
            
            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            Logger::log(Logger::DEBUG, "File size initialized.");

            std::string buffer;
            if (size > 0)
            {
                buffer.resize(size);
                file.read(&buffer[0], size);
            }

            Logger::log(Logger::DEBUG, "File buffer filled.");

            return ResponseHandler::createResponse(200, "OK", buffer, _content_type);
        }
    }
    return (_err->createError(404, "Not Found", "The server cannot find the requested resource"));
}
