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
        for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it)
        {
            std::cout << *it << std::endl;
        }
    }
}

// gerer les ;
// choper le data