/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:02:09 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 15:02:09 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Utils/utils.hpp"

void    trim(std::string &s)
{
    std::string::iterator it = s.begin();
    while (it != s.end() && std::isspace(static_cast<unsigned char>(*it)))
        ++it;
    s.erase(s.begin(), it);

    if (!s.empty())
    {
        std::string::reverse_iterator rit = s.rbegin();
        while (rit != s.rend() && std::isspace(static_cast<unsigned char>(*rit)))
            ++rit;
        s.erase(rit.base(), s.end());
    }
}

std::vector<std::string>    ws_split(const std::string& str)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool    isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

bool    isReadable(const std::string& path)
{
    return (access(path.c_str(), R_OK) == 0);
}

bool    isExecutable(const std::string& path)
{
    return (access(path.c_str(), X_OK) == 0);
}

bool    isNumeric(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

unsigned int ipStringToInt(const std::string& ip) {
    unsigned int a, b, c, d;
    char dot;
    std::istringstream iss(ip);
    if (iss >> a >> dot >> b >> dot >> c >> dot >> d)
    {
        if (a > 255 
            || b > 255
            || c > 255
            || d > 255)
            return 4294967295; //error
        return (a << 24) | (b << 16) | (c << 8) | d;
    }
    return 4294967295; //error
}

std::string ipIntToString(unsigned int ip_int)
{
    std::ostringstream oss;
    oss << ((ip_int >> 24) & 0xFF) << "."
        << ((ip_int >> 16) & 0xFF) << "."
        << ((ip_int >> 8) & 0xFF) << "."
        << (ip_int & 0xFF);
    return oss.str();
}

std::string ipPortToString(const s_ip_port& ip_port)
{
    std::ostringstream oss;
    oss << ipIntToString(ip_port.ip)
        << ":"
        << ip_port.port;
    return (oss.str());
}

