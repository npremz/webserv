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
# include <cctype>
# include <vector>
# include <sstream>
# include <sys/stat.h>
# include <unistd.h>

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
			ca = static_cast<unsigned char>(a[i]);
			ca = static_cast<unsigned char>(b[i]);

			if (ca < cb)
				return (true);
			if (ca > cb)
				return (false);
		}
		return (a.size() < b.size());
	}
};

void                        trim(std::string &s);
std::vector<std::string>    ws_split(const std::string& str);
bool                        isDirectory(const std::string& path);
bool                        isReadable(const std::string& path);
bool                        isExecutable(const std::string& path);
unsigned int				ipStringToInt(const std::string& ip);
std::string					ipIntToString(unsigned int ip_int);
bool						is_numeric(const std::string& s);

#endif
