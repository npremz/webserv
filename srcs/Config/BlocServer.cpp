/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlocServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 22:46:28 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 22:46:28 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Config/BlocServer.hpp"

BlocServer::BlocServer(std::vector<std::string> bloc) : 
    _get(true),
    _post(false),
    _delete(false),
    _autoindex(false),
    _root_path("."),
    _client_max_body_size(MAX_CLIENT_SIZE)
{
    this->_initFunctionTable();
    this->_parseBloc(bloc);
}

BlocServer::~BlocServer()
{}

bool    BlocServer::operator==(const BlocServer& other) const
{
    return (_ip_tab == other._ip_tab && _server_names == other._server_names);
}

void    BlocServer::_initFunctionTable()
{
    this->_function_table["listen"] = &BlocServer::_handleListen;
    this->_function_table["server_name"] = &BlocServer::_handleServerName;
    this->_function_table["allow_methods"] = &BlocServer::_handleMethods;
    this->_function_table["client_max_body_size"] = &BlocServer::_handleClientMaxBodySize;
    this->_function_table["index"] = &BlocServer::_handleIndex;
    this->_function_table["root"] = &BlocServer::_handleRoot;
    this->_function_table["autoindex"] = &BlocServer::_handleAutoIndex;
    this->_function_table["error_page"] = &BlocServer::_handleErrors;
}

int    BlocServer::_isLocation(std::vector<std::string> tokens)
{
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (tokens[i] == "location")
            return static_cast<int>(i);
    }
    return -1;
}

void    BlocServer::_handleListen(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    size_t dual_dots_pos = tokens[1].find(':');
    if (dual_dots_pos == std::string::npos)
    {
        if (!isNumeric(tokens[1]))
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
        std::istringstream  iss(tokens[1]);
        unsigned int        port;
        if (iss >> port && port < 65535)
        {
            s_ip_port ip_port = {0, port};
            this->_ip_tab.push_back(ip_port);
        }
        else
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
    }
    else
    {
        std::string ip_str = tokens[1].substr(0, dual_dots_pos);
        
        unsigned int ip = ipStringToInt(ip_str);
        if (ip == 4294967295)
            Logger::log(Logger::ERROR, "invalid config file. => near " + ip_str);

        s_ip_port ip_port;
        ip_port.ip = ip;

        std::string port_str = tokens[1].substr(dual_dots_pos + 1);\
        if (!isNumeric(port_str))
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
        std::istringstream  iss(port_str);
        unsigned int        port;
        if (iss >> port && port < 65536)
        {
            ip_port.port = port;
            this->_ip_tab.push_back(ip_port);
        }
        else
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
    }
}

void    BlocServer::_handleServerName(std::vector<std::string> tokens)
{
    if (tokens.size() < 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        size_t dot = (*it).find('.');
        if (dot == std::string::npos)
            Logger::log(Logger::ERROR, "invalid config file. => near " + *it);
        this->_server_names.push_back(*it);
    }
}

void    BlocServer::_handleIndex(std::vector<std::string> tokens)
{
    if (tokens.size() < 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        this->_index.push_back(*it);
    }
}

void    BlocServer::_handleMethods(std::vector<std::string> tokens)
{
    this->_get = false;
    this->_post = false;
    this->_delete = false;

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        if (*it == "GET")
        {
            if (this->_get == true)
                Logger::log(Logger::ERROR, "invalid config file. => " + *it);
            this->_get = true;
        }
        else if (*it == "POST")
        {
            if (this->_post == true)
                Logger::log(Logger::ERROR, "invalid config file. => " + *it);
            this->_post = true;
        }
        else if (*it == "DELETE")
        {
            if (this->_delete == true)
                Logger::log(Logger::ERROR, "invalid config file. => " + *it);
            this->_delete = true;
        }
        else
            Logger::log(Logger::ERROR, "invalid config file. => unknown method");
    }
}

void    BlocServer::_handleRoot(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    if (!isDirectory(tokens[1]) || !isReadable(tokens[1]))
        Logger::log(Logger::ERROR, "invalid config file. => invalid path \"" + tokens[1] + "\"");

    this->_root_path = tokens[1];
}

void    BlocServer::_handleAutoIndex(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    if (tokens[1] == "on")
        this->_autoindex = true;
    else if (tokens[1] == "off")
        this->_autoindex = false;
    else
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
}

void    BlocServer::_handleClientMaxBodySize(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    unsigned int val;
    char         c;
    char         last = (tokens[1])[tokens[1].size() - 1];

    if (last >= '0' && last <= '9')
    {
        if (!isNumeric(tokens[1].substr(0, tokens[1].size() - 1)))
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
        std::istringstream iss(tokens[1]);
        if (iss >> val)
        {
            this->_client_max_body_size = val;
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::ERROR, "invalid config file. => too big " + tokens[1]);
        }
        else
        {
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
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
                Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::ERROR, "invalid config file. => too big " + tokens[1]);
        }
        else
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[1]);
    }
}

void    BlocServer::_handleErrors(std::vector<std::string> tokens)
{
    if (tokens.size() < 3)
        Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[0]);

    std::vector<int> codes;
    size_t i = 1;
    for (; i < tokens.size() - 1; ++i) {
        if (!isNumeric(tokens[i]))
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[i]);
        int code;
        std::istringstream iss(tokens[i]);
        iss >> code;
        if (code < 400 || code > 599)
            Logger::log(Logger::ERROR, "invalid config file. => near " + tokens[i]);
        codes.push_back(code);
    }
    if (!isReadable(_root_path + tokens[i]))
        Logger::log(Logger::ERROR, "invalid config file. => unreadable " + _root_path + tokens[i]);
    std::string page = tokens[i];
    
    for (size_t j = 0; j < codes.size(); ++j) {
        this->_error_pages[codes[j]] = page;
    }

}

void    BlocServer::_tokensRedirect(std::vector<std::string> tokens)
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
        Logger::log(Logger::ERROR, "invalid config file. => \"" + tokens[0] + "\" unknown parameter");
    }
}

void    BlocServer::_parseBloc(std::vector<std::string> bloc)
{
    for (std::vector<std::string>::iterator it = bloc.begin();
        it != bloc.end(); ++it)
    {
        if (*it == "location" && *(it + 2) == "{")
        {
            std::string                 location_path = *(it + 1);
            std::vector<std::string>    location_content;

            it += 3;
            while (it != bloc.end() && *it != "}")
            {
                location_content.push_back(*it);
                ++it;
            }
            if (it == bloc.end())
                Logger::log(Logger::ERROR, "invalid config file. => expected '}'");
            BlocLocation location(this, location_path, location_content);
            this->_location_blocs.push_back(location);
        }
        else
        {
            std::vector<std::string>    tokens;
            
            while (it != bloc.end() && *it != ";")
            {
                tokens.push_back(*it);
                ++it;
            }
            this->_tokensRedirect(tokens);
            if (it == bloc.end())
                Logger::log(Logger::ERROR, "invalid config file. => expected ';' or '}'");
        }
    }
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

void BlocServer::print(int indent) const
{
    printIndent(indent);
    std::cout << "BlocServer:" << std::endl;
    for (std::vector<s_ip_port>::const_iterator it = _ip_tab.begin(); it != _ip_tab.end(); ++it)
    {
        printIndent(indent+1); std::cout << "listen: " << ipIntToString((*it).ip) << ":" <<  (*it).port << std::endl;
    }
    printIndent(indent+1); std::cout << "server_name: ";
    for (std::vector<std::string>::const_iterator it = _server_names.begin(); it != _server_names.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    printIndent(indent+1); std::cout << "allowed methods: "
        << (_get ? "GET " : "") << (_post ? "POST " :"") << (_delete ? "DELETE " : "") << std::endl;
    printIndent(indent+1); std::cout << "autoindex: " << (_autoindex ? "on" : "off") << std::endl;
    printIndent(indent+1); std::cout << "index: ";
    for (std::vector<std::string>::const_iterator it = _index.begin(); it != _index.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    printIndent(indent+1); std::cout << "root_path: " << _root_path << std::endl;
    printIndent(indent+1); std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;


    printIndent(indent+1); std::cout << "error_pages:" << std::endl;
    for (std::map<int,std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it) {
        printIndent(indent+2); std::cout << it->first << " => " << it->second << std::endl;
    }
    printIndent(indent+1); std::cout << "locations:" << std::endl;
    for (std::vector<BlocLocation>::const_iterator it = _location_blocs.begin(); it != _location_blocs.end(); ++it) {
        it->print(indent+2);
    }
}

// Getters

const std::vector<s_ip_port>& BlocServer::getIpTab() const 
{
    return _ip_tab;
}

const std::vector<std::string>& BlocServer::getServerNames() const 
{
    return _server_names;
}

const std::map<int, std::string>& BlocServer::getErrorPages() const 
{
    return _error_pages;
}

const std::vector<std::string>& BlocServer::getIndex() const 
{
    return _index;
}

bool BlocServer::getGetMethod() const 
{
    return _get;
}

bool BlocServer::getPostMethod() const 
{
    return _post;
}

bool BlocServer::getDeleteMethod() const 
{
    return _delete;
}

bool BlocServer::getAutoindex() const 
{
    return _autoindex;
}

const std::string& BlocServer::getRootPath() const 
{
    return _root_path;
}

size_t BlocServer::getClientMaxBodySize() const 
{
    return _client_max_body_size;
}

const std::vector<BlocLocation>& BlocServer::getLocationBlocs() const 
{
    return _location_blocs;
}

