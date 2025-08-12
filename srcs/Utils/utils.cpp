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
    
    while (iss >> token)
    {
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
    if (ip == "localhost")
        return 2130706433;
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

std::string to_lowercase(const std::string& input)
{
    std::string output = input;
    for (std::string::size_type i = 0; i < output.length(); ++i)
    {
        output[i] = static_cast<char>(std::tolower(output[i]));
    }
    return output;
}

int countWords(const std::string& str)
{
    int count = 0;
    bool inWord = false;

    for (std::string::size_type i = 0; i < str.length(); ++i)
    {
        if (str[i] != ' ' && !inWord)
        {
            inWord = true;
            ++count;
        } 
        else if (str[i] == ' ')
        {
            inWord = false;
        }
    }
    return count;
}

char* makeEnvVar(const std::string& key, const std::string& value)
{
    std::string var = key + "=" + value;
    char* env = new char[var.size() + 1];
    std::strcpy(env, var.c_str());
    return env;
}


std::string buildHttpResponseFromCGI(const std::string& cgi_output)
{
    std::istringstream iss(cgi_output);
    std::string line;
    std::vector<std::string> cgi_headers;
    std::string body;
    bool header_done = false;

    std::string status_line = "HTTP/1.1 200 OK\r\n";
    std::string status_value;

    while (std::getline(iss, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (!header_done) {
            if (line.empty()) {
                header_done = true;
                continue;
            }
            if (line.compare(0, 7, "Status:") == 0) {
                status_value = line.substr(7);
                status_value.erase(0, status_value.find_first_not_of(" \t"));
            } else {
                cgi_headers.push_back(line);
            }
        } else {
            body += line + "\n";
        }
    }

    if (!status_value.empty())
        status_line = "HTTP/1.1 " + status_value + "\r\n";

    std::string http_response;
    http_response += status_line;

    for (size_t i = 0; i < cgi_headers.size(); ++i) {
        http_response += cgi_headers[i] + "\r\n";
    }

    http_response += "\r\n";

    http_response += body;

    return http_response;
}

std::vector<std::string> split_path(const std::string& path) {
    std::vector<std::string> tokens;
    std::stringstream ss(path);
    std::string item;
    while (std::getline(ss, item, '/')) {
        tokens.push_back(item);
    }
    return tokens;
}

std::string normalize_path(const std::string& document_root, const std::string& request_path) {
    std::string full_path = document_root;
    if (!request_path.empty() && request_path[0] == '/')
        full_path += request_path;
    else {
        return "";
    }

    std::vector<std::string> tokens = split_path(full_path);
    std::vector<std::string> stack;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].empty() || tokens[i] == ".") {
            continue;
        } else if (tokens[i] == "..") {
            if (!stack.empty()) {
                stack.pop_back();
            }
        } else {
            stack.push_back(tokens[i]);
        }
    }
    std::string normalized;
    for (size_t i = document_root.size(); i && normalized[normalized.size()-1] == '/'; normalized.erase(normalized.size()-1)) ;
    for (size_t i = 0; i < stack.size(); ++i) {
        normalized += "/";
        normalized += stack[i];
    }

    Logger::log(Logger::DEBUG, "normalized path: " + normalized);

    if (normalized.compare(0, document_root.size(), document_root) != 0) {
        return "";
    }

    return normalized;
}

bool starts_with(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}
