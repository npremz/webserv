/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:02:27 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 15:02:27 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <cctype>
# include <vector>
# include <sstream>
# include <sys/stat.h>
# include <unistd.h>
# include <map>
# include <cstring>
# include <cstdlib>
# include <limits.h>

# include "../Logger/Logger.hpp"

class BlocServer;

struct CiLess
{
	bool operator()(const std::string &a, const std::string &b) const
	{
		size_t max;
		size_t	i;
		unsigned char	ca;
		unsigned char	cb;

		max = b.size();
		if (a.size() < b.size())
			max = a.size();
		i = 0;
		while (i < max)
		{
			ca = std::tolower(static_cast<unsigned char>(a[i]));
			cb = std::tolower(static_cast<unsigned char>(b[i]));

			if (ca < cb)
				return (true);
			if (ca > cb)
				return (false);
			i++;
		}
		return (a.size() < b.size());
	}
};

struct s_ip_port
{
    unsigned int    ip;
    unsigned int    port;

	bool operator==(const s_ip_port& other) const
	{
		return (ip == other.ip && port == other.port);
	}
};

struct s_ip_portCompare
{
	bool operator()(const s_ip_port& left, const s_ip_port& right)
	{
		if (left.ip < right.ip) return true;
        if (left.ip > right.ip) return false;
        return left.port < right.port;
	}
};

typedef std::map<s_ip_port, std::vector<BlocServer>, s_ip_portCompare> RouterMap;

void                        trim(std::string &s);
std::vector<std::string>    ws_split(const std::string& str);
bool						starts_with(const std::string& s, const std::string& prefix);
std::string					to_lowercase(const std::string& input);
int							countWords(const std::string& str);

bool                        isDirectory(const std::string& path);
bool                        isReadable(const std::string& path);
bool                        isExecutable(const std::string& path);
bool						isNumeric(const std::string& s);

unsigned int				ipStringToInt(const std::string& ip);
std::string					ipIntToString(unsigned int ip_int);
std::string 				ipPortToString(const s_ip_port& ip_port);

char*                   	makeEnvVar(const std::string& key, const std::string& value);
std::string					buildHttpResponseFromCGI(const std::string& cgi_output);
bool						validateCgiResponse(const std::string &raw, std::string *why);
std::string					normalize_path(const std::string& document_root,
								const std::string& request_path);

bool				    	has_illegal_uri_chars(const std::string& uri);


#endif
