/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMultiPartHandler.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 12:17:51 by npremont          #+#    #+#             */
/*   Updated: 2025/08/25 13:28:37 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Methods/PostMultiPartHandler.hpp"

PostMultiPartHandler::PostMultiPartHandler(HttpLexer::parsedRequest req,
    const std::string& uploadDir) 
    : _uploadDirectory(uploadDir), _req(req)
{}

PostMultiPartHandler::~PostMultiPartHandler()
{}

bool PostMultiPartHandler::parseMultipartData()
{
    _boundary = _extractBoundary(_req.contentType);
    if (_boundary.empty())
        return false;
    
    std::vector<std::string> parts = _splitByBoundary(_req.body);
    
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (!parts[i].empty())
        {
            MultipartPart part = _parsePart(parts[i]);
            if (!part.name.empty() && part.isFile)
                _parts.push_back(part);
        }
    }

    if (DEBUG_MODE)
    {
        for (std::vector<MultipartPart>::iterator it = _parts.begin();
            it != _parts.end(); it++)
        {
            Logger::log(Logger::DEBUG, "File " + (*it).name + " parsed in multipart");
        }
    }
    
    return !_parts.empty();
}

std::string PostMultiPartHandler::_extractBoundary(const std::string& contentType)
{
    std::string lowerContentType = toLowercase(contentType);
    
    size_t boundaryPos = lowerContentType.find("boundary=");
    if (boundaryPos == std::string::npos)
        return "";
    
    size_t valueStart = boundaryPos + 9;
    if (valueStart >= contentType.length())
        return "";
    
    size_t valueEnd = contentType.find(';', valueStart);
    if (valueEnd == std::string::npos)
        valueEnd = contentType.length();
    
    std::string boundary = contentType.substr(valueStart, valueEnd - valueStart);
    
    trim(boundary);
    boundary = removeQuotes(boundary);
    
    if (boundary.empty() || boundary.length() > 70)
        return "";
    
    for (size_t i = 0; i < boundary.length(); ++i)
    {
        char c = boundary[i];
        if (!((c >= '0' && c <= '9') || 
                (c >= 'A' && c <= 'Z') || 
                (c >= 'a' && c <= 'z') || 
                c == '-' || c == '_'))
        {
            return "";
        }
    }
    
    return boundary;
    return "";
}

std::vector<std::string> PostMultiPartHandler::_splitByBoundary(const std::string& body)
{
    std::vector<std::string> result;
    std::string delimiter = "--" + _boundary;
    
    size_t start = body.find(delimiter);
    if (start == std::string::npos) return result;
    
    start += delimiter.length();
    
    while (true)
    {
        size_t end = body.find(delimiter, start);
        if (end == std::string::npos) break;
        
        std::string part = body.substr(start, end - start);
        if (!part.empty())
        {
            result.push_back(part);
        }
        
        start = end + delimiter.length();
    }
    
    return result;
}

MultipartPart PostMultiPartHandler::_parsePart(const std::string& partData)
{
    MultipartPart part;
    
    size_t headerEndPos = partData.find("\r\n\r\n");
    if (headerEndPos == std::string::npos)
    {
        return part;
    }
    
    std::string headerSection = partData.substr(0, headerEndPos);
    std::string bodySection = partData.substr(headerEndPos + 4);
    
    std::map<std::string, std::string> headers = _parseHeaders(headerSection);
    
    if (headers.find("Content-Disposition") != headers.end())
    {
        part.name = _parseContentDisposition(headers["Content-Disposition"], "name");
        part.filename = _parseContentDisposition(headers["Content-Disposition"], "filename");
        part.isFile = !part.filename.empty();
    }
    
    if (headers.find("Content-Type") != headers.end())
    {
        part.contentType = headers["Content-Type"];
    }
    
    part.content = bodySection;
    
    return part;
}

std::string PostMultiPartHandler::_generateSafeFilename(const std::string& originalName)
{
    std::string safe = originalName;
    
    for (size_t i = 0; i < safe.length(); ++i)
    {
        char c = safe[i];
        if (c == '/' || c == '\\' || c == ':' || c == '*' || 
            c == '?' || c == '"' || c == '<' || c == '>' || 
            c == '|' || c == '\0' || c < 32)
        {
            safe[i] = '_';
        }
    }
    
    while (!safe.empty() && (safe[0] == '.' || safe[0] == ' '))
        safe.erase(0, 1);
    
    while (!safe.empty() && safe[safe.length() - 1] == ' ')
        safe.erase(safe.length() - 1);
    
    time_t now = time(NULL);
    std::ostringstream oss;
    oss << now << "_";
    
    if (safe.empty() || safe.length() > 200)
        oss << "uploaded_file";
    else
        oss << safe;
    
    return oss.str();
}

bool PostMultiPartHandler::_isValidFileType(const std::string& contentType)
{
    static const char* allowedTypes[] = {
        "text/plain",
        "text/html",
        "text/css",
        "text/javascript",
        "image/jpeg",
        "image/png",
        "image/gif",
        "image/webp",
        "application/pdf",
        "application/json",
        "application/xml",
        NULL
    };
    
    for (int i = 0; allowedTypes[i] != NULL; ++i)
    {
        if (contentType == allowedTypes[i])
            return true;
    }
    
    return false;
}

int PostMultiPartHandler::saveUploadedFiles()
{
	int created = 0;

    for (std::vector<MultipartPart>::iterator it = _parts.begin(); it != _parts.end(); ++it)
    {
        if (!it->isFile)
            continue;
            
        if (!_isValidFileType(it->contentType))
        {
            Logger::log(Logger::WARNING, "Invalid file type: " + it->contentType);
            continue;
        }
        
        std::string safeFilename = _generateSafeFilename(it->filename);
        std::string fullPath = _uploadDirectory + "/" + safeFilename;
        
        std::ofstream file(fullPath.c_str(), std::ios::binary);
        if (!file.is_open())
        {
            Logger::log(Logger::DEBUG, "Cannot open file for writing: " + fullPath);
            return -1;
        }
        
        file.write(it->content.c_str(), it->content.length());
        if (file.fail())
        {
            Logger::log(Logger::DEBUG, "Failed to write file: " + fullPath);
            file.close();
            return -1;
        }
        
        file.close();
		_files_saved.push_back(safeFilename);
		created++;
        Logger::log(Logger::INFO, "File saved: " + fullPath);
    }

	return (created);
}

std::string PostMultiPartHandler::report()
{
	std::string res;

	res += "The server created the following files:\n";
	for (unsigned int i = 0; i < _files_saved.size(); i++)
	{
		res += ("- " + _files_saved[i] + "\n");
	}

	return res;
}

std::map<std::string, std::string> PostMultiPartHandler::_parseHeaders(const std::string& headerSection)
{
    std::map<std::string, std::string> headers;
    std::istringstream stream(headerSection);
    std::string line;
    
    while (std::getline(stream, line))
    {
        if (line.empty() || line == "\r")
            continue;
            
        if (line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);
            
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos)
        {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            trim(value);
            headers[key] = value;
        }
    }

    return headers;
}

std::string PostMultiPartHandler::_parseContentDisposition(const std::string& contentDisposition,
    const std::string& attribute)
{
    size_t pos = contentDisposition.find(attribute + "=\"");
    if (pos == std::string::npos)
        return "";
    
    pos += attribute.length() + 2; // Skip attribute="
    size_t endPos = contentDisposition.find("\"", pos);
    if (endPos == std::string::npos)
        return "";
    
    return contentDisposition.substr(pos, endPos - pos);
}
