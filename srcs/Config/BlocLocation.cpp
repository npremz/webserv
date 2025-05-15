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

BlocLocation::BlocLocation(BlocServer* parent) : 
    _parent(parent),
    _get(false),
    _post(false),
    _delete(false),
    _autoindex(false),
    _upload_enable(false)
{
    this->_return.is_set = false;
    this->_initFunctionTable();
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
}

void    BlocLocation::_handleMethods(std::vector<std::string> tokens)
{
    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        if (*it == "GET")
        {
            if (this->_get == true)
                Logger::log(Logger::FATAL, "invalid config file. -> " + *it);
            this->_get = true;
        }
        else if (*it == "POST")
        {
            if (this->_post == true)
                Logger::log(Logger::FATAL, "invalid config file. -> " + *it);
            this->_post = true;
        }
        else if (*it == "DELETE")
        {
            if (this->_delete == true)
                Logger::log(Logger::FATAL, "invalid config file. -> " + *it);
            this->_delete = true;
        }
        else
            Logger::log(Logger::FATAL, "invalid config file. -> unknown method");
    }
}

void    BlocLocation::_handleRoot(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (!isDirectory(tokens[1]) || !isReadable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. -> invalid path \"" + tokens[1] + "\"");

    this->_root_path = tokens[1];
}

void    BlocLocation::_handleAutoIndex(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (tokens[1] == "on")
        this->_autoindex = true;
    else if (tokens[1] == "off")
        this->_autoindex = false;
    else
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
}

void    BlocLocation::_handleIndex(std::vector<std::string> tokens)
{
    if (tokens.size() < 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        this->_index.push_back(*it);
    }
}

void    BlocLocation::_handleUploadEnable(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (tokens[1] == "on")
        this->_upload_enable = true;
    else if (tokens[1] == "off")
        this->_upload_enable = false;
    else
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[1]);
}

void    BlocLocation::_handleUploadPath(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (!isDirectory(tokens[1]) || !isReadable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. -> invalid path \"" + tokens[1] + "\"");

    this->_upload_path = tokens[1];
}

void    BlocLocation::_handleCGIExt(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (tokens[1] == ".php")
        this->_cgi_pass = tokens[1];
    else if (tokens[1] == ".py")
        this->_cgi_pass = tokens[1];
    else
        Logger::log(Logger::FATAL, "invalid config file. -> unsupported cgi " + tokens[1]);
}

void    BlocLocation::_handleCGIPass(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    if (isDirectory(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. -> path is a directory: " + tokens[1]);
    
    if (!isExecutable(tokens[1]))
        Logger::log(Logger::FATAL, "invalid config file. -> not executable: " + tokens[1]);

    this->_cgi_pass = tokens[1];
}

void    BlocLocation::_handleRedirect(std::vector<std::string> tokens)
{
    if (tokens.size() != 3)
        Logger::log(Logger::FATAL, "invalid config file. -> near " + tokens[0]);

    std::istringstream iss(tokens[1]);
    int code;
    iss >> code;

    if (code < 300 || code > 307)
        Logger::log(Logger::FATAL, "invalid config file. -> invalid code " + tokens[1]);

    this->_return.code = code;

    if (!(tokens[2].rfind("http://", 0) == 0)
        && !(tokens[2].rfind("https://", 0) == 0))
    {
        std::string url = this->_parent->getRootPath() + tokens[2];
        if (!isReadable(url))
            Logger::log(Logger::FATAL, "invalid config file. -> not readable: " + url);
        this->_return.url = url;
        this->_return.is_set = true;
    }
    else
    {
        this->_return.url = tokens[2];
        this->_return.is_set = true;
    }
}

void    BlocLocation::_tokensRedirect(std::vector<std::string> tokens)
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

void    BlocLocation::parseLocation(std::ifstream& file)
{
    std::string bloc;
    std::getline(file, bloc, '}');

    std::istringstream  iss(bloc);
    std::string         line;
    while (getline(iss, line, ';'))
    {
        trim(line);

        if (line.empty() || line.at(0) == '#')
            continue ;

        Logger::log(Logger::DEBUG, "BLOCLOCATION " + line);

        std::vector<std::string> tokens = ws_split(line);
        this->_tokensRedirect(tokens);
    }
}
