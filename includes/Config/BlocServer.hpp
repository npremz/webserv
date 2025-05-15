/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlocServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 22:34:28 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 22:34:28 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCSERVER_HPP
# define BLOCSERVER_HPP

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <fstream>

# include "../../includes/Config/BlocLocation.hpp"
# include "../../includes/Utils/utils.hpp"
# include "../../includes/Logger/Logger.hpp"


class BlocServer {
    private:
        int                         _ip;
        int                         _port;
        int                         _client_max_body_size;
        std::string                 _server_name;
        std::string                 _root_path;
        std::map<int, std::string>  _error_pages;
        std::vector<BlocLocation>   _location_blocs;

    public:
        BlocServer();
        BlocServer(const BlocServer& other);
        BlocServer& operator=(const BlocServer& other);
        ~BlocServer();

        void    parseBloc(std::ifstream& file);

        // Getters
        std::string getRootPath();
};

#endif
