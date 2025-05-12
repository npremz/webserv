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

void    BlocLocation::_tokensRedirect(std::vector<std::string> tokens)
{
    switch (tokens[0])
    {
        case "allow_methods":
            break ;
        case "root":
            break ;
        case "autoindex":
            break ;
        case "index":
            break ;
        case "upload_enable":
            break ;
        case "upload_path":
            break ;
        case "cgi_extension":
            break ;
        case "cgi_pass":
            break ;
        case "redirect":
            break ;
        default:
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