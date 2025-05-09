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

BlocServer::BlocServer() : 
    _ip(0),
    _port(2025),
    _client_max_body_size(4096),
    _server_name("waf.com")
{}

BlocServer::BlocServer(const BlocServer& other) : 
    _ip(other._ip),
    _port(other._port),
    _client_max_body_size(other._client_max_body_size),
    _server_name(other._server_name)
{
    this->_error_pages = other._error_pages;
    //this->_location_blocs = other._location_blocs;
}

BlocServer& BlocServer::operator=(const BlocServer& other)
{
    if (this != &other)
    {
        this->_ip = other._ip;
        this->_port = other._port;
        this->_client_max_body_size = other._client_max_body_size;
        this->_server_name = other._server_name;
        this->_error_pages = other._error_pages;
        //this->_location_blocs = other._location_blocs;
    }
    return (*this);
}

BlocServer::~BlocServer()
{}

void    BlocServer::parseBloc(std::ifstream& file)
{
    std::string line;
    
    while (std::getline(file, line))
    {
        trim(line);

        if (line.empty() || line.at(0) == '#')
            continue ;

        Logger::log(Logger::DEBUG, "BLOCSERVER " + line);

        std::vector<std::string> tokens = ws_split(line);
        if (((tokens[0] == "location" && tokens[2] == "{") && tokens.size() == 3))
        {
            BlocLocation location;
            location.parseLocation(file);
        }
    }
}