/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcools <gcools@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 16:44:16 by gcools            #+#    #+#             */
/*   Updated: 2025/05/22 16:44:16 by gcools           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_HPP
# define GET_HPP

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <dirent.h>
# include <sys/stat.h>

# include "../../includes/Utils/utils.hpp"

std::string handleGET(const std::string& uriPath, const std::string& root, bool autoindex);

#endif
