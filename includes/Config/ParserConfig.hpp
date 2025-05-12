/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:58:11 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:58:11 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSERCONFIG_HPP
# define PARSERCONFIG_HPP

# include <iostream>
# include <string>
# include <fstream>
# include "../../includes/Config/BlocServer.hpp"

class ParserConfig {
    private:
        std::string             _file_src;
        std::vector<BlocServer> _servers;

    public:
        ParserConfig();
        ~ParserConfig();

        void    parse(std::string file_src);
};

#endif