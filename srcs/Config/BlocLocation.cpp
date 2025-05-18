/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlocLocation.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 23:24:36 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 23:24:36 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Config/BlocLocation.hpp"
#include "../../includes/Config/BlocServer.hpp"

BlocLocation::BlocLocation(BlocServer* parent, std::string location_path,
    std::vector<std::string> content) : 
    _parent(parent),
    _location_path(location_path),
    _get(true),
    _post(true),
    _delete(true),
    _autoindex(false),
    _upload_enable(false),
    _client_max_body_size(4096)
{
    this->_return.is_set = false;
    this->_initFunctionTable();
    this->_handlingContent(content);
}

BlocLocation::~BlocLocation()
{}

void    BlocLocation::_initFunctionTable()
{
    this->_function_table["allow_methods"] = &BlocLocation::_handleMethods;
    this->_function_table["root"] = &BlocLocation::_handleRoot;
    this->_function_table["autoindex"] = &BlocLocation::_handleAutoIndex;
    this->_function_table["index"] = &BlocLocation::_handleIndex;
    this->_function_table["upload_enable"] = &BlocLocation::_handleUploadEnable;
    this->_function_table["upload_path"] = &BlocLocation::_handleUploadPath;
    this->_function_table["cgi_extension"] = &BlocLocation::_handleCGIExt;
    this->_function_table["cgi_pass"] = &BlocLocation::_handleCGIPass;
    this->_function_table["return"] = &BlocLocation::_handleRedirect;
    this->_function_table["client_max_body_size"] = &BlocLocation::_handleClientMaxBodySize;
}

void    BlocLocation::_handleMethods(std::vector<std::string> tokens)
{
    this->_get = false;
    this->_post = false;
    this->_delete = false;

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        if (*it == "GET")
        {
            if (this->_get == true)
                Logger::log(Logger::FATAL, "invalid config file. => " + *it);
            this->_get = true;
        }
        else if (*it == "POST")
        {
            if (this->_post == true)
                Logger::log(Logger::FATAL, "invalid config file. => " + *it);
            this->_post = true;
        }
        else if (*it == "DELETE")
        {
            if (this->_delete == true)
                Logger::log(Logger::FATAL, "invalid config file. => " + *it);
            this->_delete = true;
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. => unknown method");
    }
}

void    BlocLocation::_handleRoot(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (!isDirectory(tokens[1]) || !isReadable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. => invalid path \"" + tokens[1] + "\"");

    this->_root_path = tokens[1];
}

void    BlocLocation::_handleAutoIndex(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (tokens[1] == "on")
        this->_autoindex = true;
    else if (tokens[1] == "off")
        this->_autoindex = false;
    else
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
}

void    BlocLocation::_handleIndex(std::vector<std::string> tokens)
{
    if (tokens.size() < 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        this->_index.push_back(*it);
    }
}

void    BlocLocation::_handleUploadEnable(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (tokens[1] == "on")
        this->_upload_enable = true;
    else if (tokens[1] == "off")
        this->_upload_enable = false;
    else
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
}

void    BlocLocation::_handleUploadPath(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (!isDirectory(tokens[1]) || !isReadable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. => invalid path \"" + tokens[1] + "\"");

    this->_upload_path = tokens[1];
}

void    BlocLocation::_handleCGIExt(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (tokens[1] == ".php")
        this->_cgi_pass = tokens[1];
    else if (tokens[1] == ".py")
        this->_cgi_pass = tokens[1];
    else
        Logger::log(Logger::FATAL, "invalid config file. => unsupported cgi " + tokens[1]);
}

void    BlocLocation::_handleCGIPass(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    if (isDirectory(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. => path is a directory: " + tokens[1]);
    
    if (!isExecutable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. => not executable: " + tokens[1]);

    this->_cgi_pass = tokens[1];
}

void    BlocLocation::_handleRedirect(std::vector<std::string> tokens)
{
    if (tokens.size() != 3)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    std::istringstream iss(tokens[1]);
    int code;
    iss >> code;

    if (code < 300 || code > 307)
        Logger::log(Logger::FATAL, "invalid config file. => invalid code " + tokens[1]);

    this->_return.code = code;

    if (!(tokens[2].rfind("http://", 0) == 0)
        && !(tokens[2].rfind("https://", 0) == 0))
    {
        std::string url = this->_parent->getRootPath() + tokens[2];
        if (!isReadable(url))
            Logger::log(Logger::FATAL, "invalid config file. => not readable: " + url);
        this->_return.url = url;
        this->_return.is_set = true;
    }
    else
    {
        this->_return.url = tokens[2];
        this->_return.is_set = true;
    }
}

void    BlocLocation::_handleClientMaxBodySize(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[0]);

    unsigned int val;
    char         c;
    char         last = (tokens[1])[tokens[1].size() - 1];

    std::cout << last << std::endl;

    if (last >= '0' && last <= '9')
    {
        if (!isNumeric(tokens[1].substr(0, tokens[1].size() - 1)))
            Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
        std::istringstream iss(tokens[1]);
        if (iss >> val)
        {
            this->_client_max_body_size = val;
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::FATAL, "invalid config file. => too big " + tokens[1]);
        }
        else
        {
            Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
        }
    } 
    else 
    {
        std::istringstream iss(tokens[1]);
        if (iss >> val >> c && c == last)
        {

            if (c == 'K' || c == 'k')
                this->_client_max_body_size = val * 1024;
            else if (c == 'M' || c == 'm')
                this->_client_max_body_size = val * (1024 * 1024);
            else
                Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::FATAL, "invalid config file. => too big " + tokens[1]);
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. => near " + tokens[1]);
    }
}

void    BlocLocation::_handlingContent(std::vector<std::string> content)
{
    for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); ++it)
    {
        std::vector<std::string>    tokens;
            
        while (it != content.end() && *it != ";")
        {
            tokens.push_back(*it);
            ++it;
        }
        this->_tokensRedirect(tokens);
        if (it == content.end())
            Logger::log(Logger::FATAL, "invalid config file. => expected ';'");
    }
}

void    BlocLocation::_tokensRedirect(std::vector<std::string> tokens)
{
    if (tokens.size() < 1)
        return;
    std::map<std::string, HandlerFunc>::iterator it = _function_table.find(tokens[0]);
    if (it != _function_table.end())
    {
        HandlerFunc handler = it->second;
        (this->*handler)(tokens);
    }
    else
    {
        Logger::log(Logger::FATAL, "invalid config file. => \"" + tokens[0] + "\" unknown parameter");
    }
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
}

void BlocLocation::print(int indent) const
{
    printIndent(indent);
    std::cout << "BlocLocation:" << std::endl;

    printIndent(indent+1); std::cout << "location_path: " << _location_path << std::endl;
    printIndent(indent+1); std::cout << "allowed methods: "
        << (_get ? "GET " : "") << (_post ? "POST " :"") << (_delete ? "DELETE " : "") << std::endl;
    printIndent(indent+1); std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
    printIndent(indent+1); std::cout << "upload_enable: " << (_upload_enable ? "on" : "off") << std::endl;
    printIndent(indent+1); std::cout << "upload_path: " << _upload_path << std::endl;
    printIndent(indent+1); std::cout << "cgi_extension: " << _cgi_extension << std::endl;
    printIndent(indent+1); std::cout << "cgi_pass: " << _cgi_pass << std::endl;
    printIndent(indent+1); std::cout << "root_path: " << _root_path << std::endl;
    printIndent(indent+1); std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;

    printIndent(indent+1); std::cout << "index: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;

    printIndent(indent+1); std::cout << "redirection: ";
    if (_return.is_set)
        std::cout << "code " << _return.code << " url " << _return.url << std::endl;
    else
        std::cout << "none" << std::endl;
}

//Getters

BlocServer* BlocLocation::getParent() const 
{
    return _parent;
}

const std::string& BlocLocation::getLocationPath() const 
{
    return _location_path;
}

bool BlocLocation::getGetMethod() const 
{
    return _get;
}

bool BlocLocation::getPostMethod() const 
{
    return _post;
}

bool BlocLocation::getDeleteMethod() const 
{
    return _delete;
}

bool BlocLocation::getAutoindex() const 
{
    return _autoindex;
}

bool BlocLocation::getUploadEnable() const 
{
    return _upload_enable;
}

const std::string& BlocLocation::getUploadPath() const 
{
    return _upload_path;
}

const std::string& BlocLocation::getCGIExtension() const 
{
    return _cgi_extension;
}

const std::string& BlocLocation::getCGIPass() const 
{
    return _cgi_pass;
}

const std::string& BlocLocation::getRootPath() const 
{
    return _root_path;
}

const std::vector<std::string>& BlocLocation::getIndex() const 
{
    return _index;
}

int BlocLocation::getRedirectCode() const 
{
    return _return.code;
}

const std::string& BlocLocation::getRedirectUrl() const 
{
    return _return.url;
}

bool BlocLocation::isRedirectSet() const 
{
    return _return.is_set;
}

size_t BlocLocation::getClientMaxBodySize() const 
{
    return _client_max_body_size;
}
