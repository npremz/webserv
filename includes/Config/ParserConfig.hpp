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
# include "../../includes/Logger/Logger.hpp"
# include "../../includes/Utils/utils.hpp"

class ParserConfig {
    private:
        bool                        _isLoaded;
        std::string                 _file_src;
        std::vector<BlocServer>     _servers_ctx;

        std::vector<std::string>    _tokenise(const std::string& input);

    public:
        ParserConfig();
        ~ParserConfig();

        void        parse(std::string file_src);
        RouterMap   initRouter();

        void    print(int indent = 0) const;

        // Getters;
        bool                            getIsLoaded() const;
        const std::vector<BlocServer>&  getServersCtx() const;

};

#endif
