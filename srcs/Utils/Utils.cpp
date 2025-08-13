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
    try {
        char* env = new char[var.size() + 1];
        std::strcpy(env, var.c_str());
        return env;
    }
    catch (const std::exception& e)
    {
        Logger::log(Logger::ERROR, "CGI initialisation failed.");
    }
    return NULL;
}

static const char* defaultReasonPhrase(int code) {
    switch (code) {
        // 1xx
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        // 2xx
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 203: return "Non-Authoritative Information";
        case 204: return "No Content";
        case 205: return "Reset Content";
        case 206: return "Partial Content";
        // 3xx
        case 300: return "Multiple Choices";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 303: return "See Other";
        case 304: return "Not Modified";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";
        // 4xx
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 402: return "Payment Required";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 406: return "Not Acceptable";
        case 407: return "Proxy Authentication Required";
        case 408: return "Request Timeout";
        case 409: return "Conflict";
        case 410: return "Gone";
        case 411: return "Length Required";
        case 412: return "Precondition Failed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 416: return "Range Not Satisfiable";
        case 417: return "Expectation Failed";
        case 418: return "I'm a teapot";
        case 421: return "Misdirected Request";
        case 422: return "Unprocessable Entity";
        case 425: return "Too Early";
        case 426: return "Upgrade Required";
        case 428: return "Precondition Required";
        case 429: return "Too Many Requests";
        case 431: return "Request Header Fields Too Large";
        case 451: return "Unavailable For Legal Reasons";
        // 5xx
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        case 511: return "Network Authentication Required";
    }
    return "Unknown";
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
    {
        std::istringstream ss(status_value);
        int code = 0;
        ss >> code;

        std::string reason;
        std::getline(ss, reason);

        if (code <= 0)
        {
            code = 500;
            reason = defaultReasonPhrase(code);
        }
        else if (reason.empty())
        {
            reason = defaultReasonPhrase(code);
        }

        std::ostringstream oss;
        oss << "HTTP/1.1 " << code << ' ' << reason << "\r\n";
        status_line = oss.str();
    }

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

static std::string trim_left_spaces(const std::string &s) {
    std::string::size_type i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
    return s.substr(i);
}

static bool is_ctl_except_ht(unsigned char c) {
    return ((c <= 31 && c != '\t') || c == 127);
}

bool validateCgiResponse(const std::string &raw, std::string *why) {
    if (why) why->clear();
    if (raw.empty()) { if (why) *why = "Empty response from CGI"; return false; }

    std::string::size_type sep = std::string::npos;
    std::string::size_type p1 = raw.find("\r\n\r\n");
    std::string::size_type p2 = raw.find("\n\n");
    if (p1 != std::string::npos) sep = p1 + 4;
    else if (p2 != std::string::npos) sep = p2 + 2;
    if (sep == std::string::npos) { if (why) *why = "No new line after headers"; return false; }

    std::string head = raw.substr(0, sep);
    std::string body = raw.substr(sep);

    if (head.size() >= 4 && head.compare(head.size()-4, 4, "\r\n\r\n") == 0) head.erase(head.size()-4);
    else if (head.size() >= 2 && head.compare(head.size()-2, 2, "\n\n") == 0) head.erase(head.size()-2);

    bool seenContentLen = false;
    long contentLen     = -1;

    std::string::size_type start = 0;
    int header_count = 0;
    while (start < head.size()) {
        std::string::size_type end = head.find('\n', start);
        std::string line = (end == std::string::npos) ? head.substr(start) : head.substr(start, end - start + 1);

        if (!line.empty() && line[line.size()-1] == '\n') line.erase(line.size()-1, 1);
        if (!line.empty() && line[line.size()-1] == '\r') line.erase(line.size()-1, 1);

        if (line.empty()) break;

        Logger::log(Logger::DEBUG, line);

        std::string::size_type colon = line.find(':');
        if (colon == std::string::npos || colon == 0) {
            if (why) *why = "Invalid header";
            return false;
        }

        for (std::string::size_type i = 0; i < colon; ++i) {
            unsigned char c = (unsigned char)line[i];
            if (is_ctl_except_ht(c) || c == ' ' || c == '\t') {
                if (why) *why = "Invalid header name";
                return false;
            }
        }

        std::string value = trim_left_spaces(line.substr(colon + 1));
        for (std::string::size_type i = 0; i < value.size(); ++i) {
            if (is_ctl_except_ht((unsigned char)value[i])) {
                if (why) *why = "Illagal char in header value";
                return false;
            }
        }

        const std::string name = line.substr(0, colon);
        std::string lname; lname.reserve(name.size());
        for (std::string::size_type i = 0; i < name.size(); ++i)
            lname.push_back((char)std::tolower((unsigned char)name[i]));

        if (lname == "content-length") {
            if (seenContentLen) { if (why) *why = "multiple content-length"; return false; }
            seenContentLen = true;

            if (value.empty()) { if (why) *why = "Empty Content-Length"; return false; }
            long v = 0;
            for (std::string::size_type i = 0; i < value.size(); ++i) {
                unsigned char c = (unsigned char)value[i];
                if (!std::isdigit(c)) { if (why) *why = "Invalid Content-Length"; return false; }
                int d = c - '0';
                if (v > (LONG_MAX - d) / 10) { if (why) *why = "Content-Length too big"; return false; }
                v = v * 10 + d;
            }
            contentLen = v;
        }

        if (end == std::string::npos) break;
        start = end + 1;

        if (++header_count > MAX_HEADERS) { if (why) *why = "Too many headers"; return false; }
    }

    if (seenContentLen) {
        if (contentLen < 0) { if (why) *why = "Negative Content-Length"; return false; }
    }

    return true;
}

inline bool has_illegal_uri_chars(const std::string& uri)
{
    if (uri.empty()) return true;

    for (std::string::size_type i = 0; i < uri.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(uri[i]);

        if (c <= 31 || c == 127 || c >= 128 || c == ' ') return true;

        if (c == '%') {
            if (i + 2 >= uri.size()) return true;
            const unsigned char h1 = static_cast<unsigned char>(uri[i+1]);
            const unsigned char h2 = static_cast<unsigned char>(uri[i+2]);
            const bool is_hex1 = (std::isdigit(h1) || (h1 >= 'A' && h1 <= 'F') || (h1 >= 'a' && h1 <= 'f'));
            const bool is_hex2 = (std::isdigit(h2) || (h2 >= 'A' && h2 <= 'F') || (h2 >= 'a' && h2 <= 'f'));
            if (!is_hex1 || !is_hex2) return true;
            i += 2;
            continue;
        }

        if (std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') continue;

        if (c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' ||
            c == '*' || c == '+' || c == ',' || c == ';' || c == '=') continue;

        if (c == ':' || c == '@' || c == '/' || c == '?') continue;

        return true;
    }
    return false;
}
