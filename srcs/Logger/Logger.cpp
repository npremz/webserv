/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:53:53 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 11:53:53 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Logger/Logger.hpp"

void    Logger::addLevel(Logger::level level)
{
    switch (level)
    {
        case Logger::FATAL:
            std::cout << C_RED << "[FATAL] ";
            break ;
        case Logger::INFO:
            std::cout << C_CYAN << "[INFO] ";
            break ;
    }
}

void    Logger::printLog(Logger::level level, std::string msg)
{
    Logger::addLevel(level);
    std::cout << msg  << C_RESET << std::endl;
}

void    Logger::log(Logger::level level, std::string msg)
{
    if (level == FATAL)
    {
        Logger::addLevel(level);
        throw std::runtime_error(msg);
    }
    Logger::printLog(level, msg);
}