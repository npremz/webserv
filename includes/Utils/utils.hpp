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

void                        trim(std::string &s);
std::vector<std::string>    ws_split(const std::string& str);

#endif