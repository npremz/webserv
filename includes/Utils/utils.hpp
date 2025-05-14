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

struct CiLess
{
	bool operator()(const std::string &a, const std::string &b) const
	{
		size_t max;
		int	i;
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

#endif