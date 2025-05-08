/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:58:08 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:58:08 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Config/ParserConfig.hpp"
#include "../../includes/Logger/Logger.hpp"

ParserConfig::ParserConfig() : _file_src("")
{}

ParserConfig::~ParserConfig()
{}

void    ParserConfig::Parse(std::string file_src)
{
    this->_file_src = file_src;
    std::ifstream file(_file_src.c_str());

    if (!file.is_open())
    {
        Logger::log(Logger::FATAL, "unable to open config file.");
        return ;
    }
}