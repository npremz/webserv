/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 10:24:50 by npremont          #+#    #+#             */
/*   Updated: 2025/08/01 18:59:15 by npremont         ###   ########.fr       */
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
}

Response::~Response()
{}

std::string Response::_isCustomError(unsigned int code)
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

std::string Response::_createCustomError(unsigned int code, std::string error_page,
    std::string error_msg)
{
    std::ifstream file((_ctx->getRootPath() + error_page).c_str(), std::ios::binary);
    if (!file.is_open())
        return _createError(500, "Internal Server Error",
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

std::string Response::_createError(unsigned int code, std::string error, std::string bodyStr)
{
    std::string error_page = _isCustomError(code);
    if (error_page.size() > 0)
        return (_createCustomError(code, error_page, error));

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

std::string Response::_createResponse(unsigned int code, std::string msg, const std::string& bodyStr)
{
    std::ostringstream oss_header;
    oss_header << "HTTP/1.1 " << code << " " << msg << "\r\n";
    oss_header << "Content-Type: " << _content_type << "\r\n";
    oss_header << "Content-Length: " << bodyStr.size() << "\r\n";
    oss_header << "\r\n"; 

    std::string response = oss_header.str();
    response.append(bodyStr.data(), bodyStr.size());

    return (response);
}

std::string Response::sendError(std::string error)
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
    return (_createError(_req.endstatus, msg, error));
}

std::string Response::_createRedirect(unsigned int code, const std::string& url)
{
    std::string status_msg;

    if (code == 300)
        status_msg = "Multiple Choices";
    else if (code == 301)
        status_msg = "Moved Permanently";
    else if (code == 302)
        status_msg = "Found";
    else if (code == 303)
        status_msg = "See Other";
    else if (code == 305)
        status_msg = "Use Proxy";
    else if (code == 306)
        status_msg = "(Unused)";
    else if (code == 307)
        status_msg = "Temporary Redirect";

    std::ostringstream oss_body;
    oss_body << "<html><head><title>" << code << status_msg << "</title></head><body><h1>"
             << code << " " << status_msg
             << "</h1><p>The document has moved <a href=" 
             << url
             << ">here</a>."
             << "</p></body></html>"
             << "\r\n\r\n";

    std::ostringstream oss_header;
    oss_header << "HTTP/1.1 " << code << " " << status_msg << "\r\n"
               << "Location: " << url << "\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << oss_body.str().size() << "\r\n"
               << "\r\n";
    
    std::string response = oss_header.str() + oss_body.str();
    return (response);
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
            it->print(2);
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

std::string Response::_testIndex(std::string URI)
{
    if (_location_ctx)
    {
        for (std::vector<std::string>::const_iterator it = _location_ctx->getIndex().begin();
            it < _location_ctx->getIndex().end(); it ++)
        {
            if (access((URI + (*it)).c_str(), R_OK) == 0)
                return ((*it));
        }
    }
    else
    {
        for (std::vector<std::string>::const_iterator it = _ctx->getIndex().begin();
            it < _ctx->getIndex().end(); it ++)
        {
            if (access((URI + (*it)).c_str(), R_OK) == 0)
                return ((*it));
        }
    }
    return ("none");
}

void    Response::_initContentType(std::string file)
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
        _content_type = "image/webp";
    else if (".pdf" == ext)
        _content_type = "image/svg+xml";
    else if (".php" == ext)
        _content_type = "cgi/php";
    else if (".py" == ext)
        _content_type = "cgi/py";
    else
        _content_type = "application/octet-stream";
    
    Logger::log(Logger::DEBUG, _content_type);
}

std::string Response::_generateAutoIndex(std::string fullpath)
{
    std::ostringstream html;
    html << "<html><head><title>Index of " << _req.path << "</title></head><body>";
    html << "<h1>Index of " << _req.path << "</h1><ul>";

    DIR* dir = opendir(fullpath.c_str());
    if (!dir) 
        return (_createError(403, "Forbidden", "The client does not have access rights to the content"));

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
    return _createResponse(200, "OK", html.str());
}

bool Response::_handleGetCGI()
{
    if (!_location_ctx)
        return (false);
    
    Logger::log(Logger::DEBUG, "_location_ctx for CGi checked.");

    if (!((_content_type == "cgi/py" && _location_ctx->getCGIExtension() == ".py") 
        || (_content_type == "cgi/php" && _location_ctx->getCGIExtension() == ".php")))
        return (false);

    Logger::log(Logger::DEBUG, "CGI detected with: " + _location_ctx->getCGIExtension());

    CGI cgi_handler(std::string("GET"), _req, _content_type, _ctx, _location_ctx, _parent);
    cgi_handler.exec();

    return (true);
}

std::string Response::_handleGet()
{
    std::string fullPath;
    if (_location_ctx && _location_ctx->getRootPath().size() > 0)
        fullPath = _location_ctx->getRootPath() + _req.path;
    else
        if (_ctx->getRootPath().size() > 0)
            fullPath = _ctx->getRootPath() + _req.path;
    Logger::log(Logger::DEBUG, "Path of location: " + fullPath);
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
                return _createResponse(200, "OK", oss.str());
            } 
            else if (_location_ctx)
            {
                if (_location_ctx->getAutoindex())
                    return (_generateAutoIndex(fullPath));
                else
                    return (_createError(403, "Forbidden", "The client does not have access rights to the content"));
            }
            else if (_ctx->getAutoindex())
            {
                return (_generateAutoIndex(fullPath));
            } 
            else
            {
                return (_createError(403, "Forbidden", "The client does not have access rights to the content"));
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
                return _createError(403, "Forbidden", "The client does not have access rights to the content");
            
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

            return _createResponse(200, "OK", buffer);
        }
    }
    return (_createError(404, "Not Found", "The server cannot find the requested resource"));
}

std::string Response::_handleUpload(std::string uploadDir)
{
    Logger::log(Logger::DEBUG, "Target path: " + uploadDir);
    if (_location_ctx && !_location_ctx->getCGIExtension().empty())
    {
        Logger::log(Logger::DEBUG, "Forwarding raw body to CGI");
        CGI cgi_handler(std::string("POST"), _req, _req.contentType, _ctx, _location_ctx, _parent);
        cgi_handler.exec();
        return ("CGI");
    }
    else
    {
        Logger::log(Logger::DEBUG, "No CGI configured. sending 405");
        return (_createError(405, "Method Not Allowed", "This server does not support direct form processing. Please configure a CGI handler"));
    }
}

std::string Response::_handlePostContentType(std::string fullPath)
{
    (void)fullPath;
    Logger::log(Logger::DEBUG, "POST: Directory post.");
    std::string contentType;
    contentType = _req.contentType;
    if (contentType.find("multipart/form-data") != std::string::npos
        || contentType.find("application/x-www-form-urlencoded") != std::string::npos)
        return (_handleUpload(fullPath));
    return (_createError(415, "Unsupported Media Type", "Content type not supported for directory POST"));
}

std::string Response::_handlePost()
{
    std::string fullPath;
    if (_location_ctx && _location_ctx->getRootPath().size() > 0)
        fullPath = _location_ctx->getRootPath() + _req.path;
    else
        if (_ctx->getRootPath().size() > 0)
            fullPath = _ctx->getRootPath() + _req.path;
    Logger::log(Logger::DEBUG, "POST target path: " + fullPath);

    if (access(fullPath.c_str(), F_OK) == 0)
    {
        if (access(fullPath.c_str(), R_OK | X_OK) == 0)
            return (_handlePostContentType(fullPath));
        else
            return (_createError(403, "Forbidden", "Request failed due to insufficient permissions"));
    }
    return (_createError(404, "Not Found", "The server cannot find the requested resource"));
}

std::string Response::_handleDelete()
{
    std::string fullpath;
    if (_location_ctx && _location_ctx->getRootPath().size() > 0)
        fullpath = _location_ctx->getRootPath() + _req.path;
    else
        if (_ctx->getRootPath().size() > 0)
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
                _createError(403, "Forbidden", "Request failed due to insufficient permissions");
            
            Logger::log(Logger::DEBUG, "Sending delete request to cgi.");
            CGI cgi_handler(std::string("DELETE"), _req, _req.contentType, _ctx, _location_ctx, _parent);
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
                _createError(403, "Forbidden", "Request failed due to insufficient permissions");

            int delete_res = std::remove(fullpath.c_str()); 
            if (delete_res == 0)
            {
                Logger::log(Logger::DEBUG, "Deleted " + fullpath);
                return (_createResponse(200, "OK", ""));
            }
            else
                return (_createError(500, "Internal Server Error", ""));
        }
    }
    return (_createError(404, "Not Found", "The server cannot find the requested resource"));
    
}

std::string Response::_handleMethod()
{
    switch (_req.method)
    {
        case HttpLexer::HTTP_GET:
            return (_handleGet());
            break;
        case HttpLexer::HTTP_POST:
            return (_handlePost());
            break;
        case HttpLexer::HTTP_DELETE:
            return (_handleDelete());
        default:
            return ("waf");
    }
    return ("waf");
}

std::string Response::createResponseSTR()
{
    if (_req.endstatus >= 400)
        return (_handleLexerErrors());
    _setLocation();
    if (_location_ctx && _location_ctx->isRedirectSet())
        return (_createRedirect(_location_ctx->getRedirectCode(),
            _location_ctx->getRedirectUrl()));
    if (_location_ctx && _location_ctx->getClientMaxBodySize() < _req.expectedoctets)
        return (_createError(413, "Content Too Large",
            "The request entity was larger than limits defined by server"));
    else if (_ctx->getClientMaxBodySize() < _req.expectedoctets)
        return (_createError(413, "Content Too Large",
            "The request entity was larger than limits defined by server"));
    if (!_isMethodSupportedByRoute())
        return (_createError(405, "Method Not Allowed",
            "The request method is known by the server but is not supported by the target resource. "));
        
    return (_handleMethod());
    return (_createResponse(200, "OK", "Hello World"));
    
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
    std::cout << _response_cgi << std::endl;

    if (_location_ctx->getCGIExtension() == ".php")
    {
        _response_cgi = buildHttpResponseFromCGI(_response_cgi);
        Logger::log(Logger::DEBUG, "php CGI formated content:");
        std::cout << _response_cgi << std::endl;
    }

    return (_response_cgi);
}
