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
    _client_max_body_size(4096),
    _root_path("./server_files")
{
    this->_initFunctionTable();
    this->_parseBloc(bloc);
}

BlocServer::BlocServer(const BlocServer& other) : 
    _ip_tab(other._ip_tab),
    _client_max_body_size(other._client_max_body_size),
    _server_names(other._server_names),
    _root_path("./server_files")
{
    this->_error_pages = other._error_pages;
    this->_location_blocs = other._location_blocs;
}

BlocServer& BlocServer::operator=(const BlocServer& other)
{
    if (this != &other)
    {
        this->_ip_tab = other._ip_tab;
        this->_client_max_body_size = other._client_max_body_size;
        this->_server_names = other._server_names;
        this->_error_pages = other._error_pages;
        this->_location_blocs = other._location_blocs;
    }
    return (*this);
}

BlocServer::~BlocServer()
{}

void    BlocServer::_initFunctionTable()
{
    this->_function_table["listen"] = &BlocServer::_handleListen;
    this->_function_table["server_name"] = &BlocServer::_handleServerName;
    this->_function_table["client_max_body_size"] = &BlocServer::_handleClientMaxBodySize;
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
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    size_t dual_dots_pos = tokens[1].find(':');
    if (dual_dots_pos == std::string::npos)
    {
        if (!is_numeric(tokens[1]))
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
        std::istringstream  iss(tokens[1]);
        unsigned int        port;
        if (iss >> port && port < 65535)
        {
            BlocServer::s_ip_port ip_port = {0, port};
            this->_ip_tab.push_back(ip_port);
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
    }
    else
    {
        std::string ip_str = tokens[1].substr(0, dual_dots_pos);
        
        unsigned int ip = ipStringToInt(ip_str);
        if (ip == 4294967295)
            Logger::log(Logger::FATAL, "invalid config file. -> near " + ip_str);

        BlocServer::s_ip_port ip_port;
        ip_port.ip = ip;

        std::string port_str = tokens[1].substr(dual_dots_pos + 1);\
        if (!is_numeric(port_str))
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
        std::istringstream  iss(port_str);
        unsigned int        port;
        if (iss >> port && port < 65535)
        {
            ip_port.port = port;
            this->_ip_tab.push_back(ip_port);
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
    }
}

void    BlocServer::_handleServerName(std::vector<std::string> tokens)
{
    if (tokens.size() < 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        size_t dot = (*it).find('.');
        if (dot == std::string::npos)
            Logger::log(Logger::FATAL, "invalid config file. -> near " + *it);
        this->_server_names.push_back(*it);
    }
}

void    BlocServer::_handleClientMaxBodySize(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    unsigned int val;
    char         c;
    char         last = (tokens[1])[tokens[1].size() - 1];

    std::cout << last << std::endl;

    if (last >= '0' && last <= '9')
    {
        if (!is_numeric(tokens[1].substr(0, tokens[1].size() - 1)))
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
        std::istringstream iss(tokens[1]);
        if (iss >> val)
        {
            this->_client_max_body_size = val;
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::FATAL, "invalid config file. -> too big " + tokens[1]);
        }
        else
        {
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
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
                Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
            if (this->_client_max_body_size > MAX_CLIENT_SIZE)
                Logger::log(Logger::FATAL, "invalid config file. -> too big " + tokens[1]);
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
    }
}

void    BlocServer::_handleErrors(std::vector<std::string> tokens)
{
    if (tokens.size() < 3)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    std::vector<int> codes;
    size_t i = 1;
    for (; i < tokens.size() - 1; ++i) {
        if (!is_numeric(tokens[i]))
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[i]);
        int code;
        std::istringstream iss(tokens[i]);
        iss >> code;
        if (code < 300 || code > 599)
            Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[i]);
        codes.push_back(code);
    }
    std::string page = tokens[i];
    
    for (size_t j = 0; j < codes.size(); ++j) {
        this->_error_pages[codes[j]] = page;
    }

}

void    BlocServer::_tokensRedirect(std::vector<std::string> tokens)
{
    std::map<std::string, HandlerFunc>::iterator it = _function_table.find(tokens[0]);
    if (it != _function_table.end())
    {
        HandlerFunc handler = it->second;
        (this->*handler)(tokens);
    }
    else
    {
        Logger::log(Logger::FATAL, "invalid config file. -> \"" + tokens[0] + "\" unknown parameter");
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
            while (*it != "}")
            {
                location_content.push_back(*it);
                ++it;
            }
            BlocLocation location(this, location_path, location_content);
            this->_location_blocs.push_back(location);
        }
        else
        {
            std::vector<std::string>    tokens;
            
            while (*it != ";")
            {
                tokens.push_back(*it);
                ++it;
            }
            this->_tokensRedirect(tokens);
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
    for (std::vector<BlocServer::s_ip_port>::const_iterator it = _ip_tab.begin(); it != _ip_tab.end(); ++it)
    {
        printIndent(indent+1); std::cout << "listen: " << ipIntToString((*it).ip) << ":" <<  (*it).port << std::endl;
    }
    printIndent(indent+1); std::cout << "server_name: ";
    for (std::vector<std::string>::const_iterator it = _server_names.begin(); it != _server_names.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
    printIndent(indent+1); std::cout << "client_max_body_size: " << _client_max_body_size << std::endl;
    printIndent(indent+1); std::cout << "root_path: " << _root_path << std::endl;

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

std::string BlocServer::getRootPath()
{
    return (this->_root_path);
}
