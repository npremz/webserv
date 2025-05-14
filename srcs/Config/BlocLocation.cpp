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

BlocLocation::BlocLocation(BlocServer* parent) : 
    _parent(parent),
    _get(false),
    _post(false),
    _delete(false),
    _autoindex(false)
{}

BlocLocation::~BlocLocation()
{}

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
    (void)tokens;
}

void    BlocLocation::_tokensRedirect(std::vector<std::string> tokens)
{
    if (tokens[0] == "allow_methods")
        this->_handleMethods(tokens);
    else if (tokens[0] == "root")
        this->_handleRoot(tokens);
    else if (tokens[0] == "autoindex")
        this->_handleAutoIndex(tokens);
    else if (tokens[0] == "index")
        this->_handleIndex(tokens);
    else if (tokens[0] == "upload_enable")
    {

    } 
    else if (tokens[0] == "upload_path")
    {

    } 
    else if (tokens[0] == "cgi_extension")
    {

    } 
    else if (tokens[0] == "cgi_pass")
    {

    } 
    else if (tokens[0] == "redirect")
    {

    } 
    else 
    {
        Logger::log(Logger::FATAL,
            "invalid config file. -> \"" + tokens[0] + "\" unknown parameter");
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

// gerer les ;
// choper le data
