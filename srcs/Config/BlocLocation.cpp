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

BlocLocation::BlocLocation()
{}

BlocLocation::~BlocLocation()
{}

void    BlocLocation::_handleMethods(std::vector<std::string> tokens)
{
    for (std::vector<std::string>::iterator it = tokens.begin() + 1; it < tokens.end(); ++it)
    {
        if (*it == "GET")
            this->_get = true;
        else if (*it == "POST")
            this->_post = true;
        else if (*it == "DELETE")
            this->_delete = true;
        else
            Logger::log(Logger::FATAL, "invalid config file.");
    }

    std::cout << this->_get << std::endl;
    std::cout << this->_post << std::endl;
    std::cout << this->_delete << std::endl;
}

void    BlocLocation::_tokensRedirect(std::vector<std::string> tokens)
{
    if (tokens[0] == "allow_methods")
    {
        this->_handleMethods(tokens);
    } 
    else if (tokens[0] == "root")
    {

    } 
    else if (tokens[0] == "autoindex")
    {

    } 
    else if (tokens[0] == "index")
    {

    } 
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
        Logger::log(Logger::FATAL, "invalid config file.");
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
