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
    // 1. Obtenir l'heure actuelle
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[9]; // HH:MM:SS + '\0'

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // 2. Formatter dans HH:MM:SS
    sprintf(buffer, "%02d:%02d:%02d", 
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec);

    // 3. Affichage du timecode avant le level
    std::cout << "[" << buffer << "] ";

    switch (level)
    {
        case Logger::FATAL:
            std::cout << C_RED << "[FATAL] ";
            break ;
        case Logger::INFO:
            std::cout << C_CYAN << "[INFO] ";
            break ;
        case Logger::ERROR:
            std::cout << C_YELLOW << "[ERROR] ";
            break ;
        case Logger::DEBUG:
            std::cout << C_LIGHT_GRAY << "[DEBUG] ";
            break ;
        case Logger::WARNING:
            std::cout << C_BEIGE << "[WARNING] ";
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
    if (level == DEBUG && !DEBUG_MODE)
        return ;
    if (level == FATAL)
    {
        Logger::addLevel(level);
        throw std::runtime_error(msg);
    } 
    else if (level == ERROR)
    {
        Logger::addLevel(level);
        throw std::runtime_error(msg);
    }
    Logger::printLog(level, msg);
}
