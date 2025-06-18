/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 17:12:58 by npremont          #+#    #+#             */
/*   Updated: 2025/06/18 17:23:03 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <vector>
# include <iostream>

class CGI
{
    private:
        std::vector<char *> _env_tab;
        
        char*               _makeEnvVar(const std::string& key, const std::string& value);
        void                _initEnvTab();

    public:
        CGI();
        ~CGI();
};

#endif
