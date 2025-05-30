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

# include "../../includes/Utils/utils.hpp"
# include "../../includes/Config/BlocLocation.hpp"
# include "../../includes/Logger/Logger.hpp"

class BlocServer {

    typedef void (BlocServer::*HandlerFunc)(std::vector<std::string>);

    private:
        std::map<std::string, HandlerFunc>  _function_table;


        std::vector<s_ip_port>              _ip_tab;
        std::vector<std::string>            _server_names;
        std::map<int, std::string>          _error_pages;
        std::vector<std::string>            _index;
        bool                                _get;
        bool                                _post;
        bool                                _delete;
        bool                                _autoindex;
        std::string                         _root_path;
        size_t                              _client_max_body_size;
        std::vector<BlocLocation>           _location_blocs;
        
        void    _parseBloc(std::vector<std::string> bloc);
        
        int     _isLocation(std::vector<std::string> tokens);
        void    _handleLocation(std::ifstream& file, std::vector<std::string> tokens,
            int location_pos);
        void    _tokensRedirect(std::vector<std::string> tokens);
        void    _initFunctionTable();

        void    _handleListen(std::vector<std::string> tokens);
        void    _handleServerName(std::vector<std::string> tokens);
        void    _handleMethods(std::vector<std::string> tokens);
        void    _handleIndex(std::vector<std::string> tokens);
        void    _handleRoot(std::vector<std::string> tokens);
        void    _handleAutoIndex(std::vector<std::string> tokens);
        void    _handleClientMaxBodySize(std::vector<std::string> tokens);
        void    _handleErrors(std::vector<std::string> tokens);

    public:
        BlocServer(std::vector<std::string> bloc);
        ~BlocServer();

        bool    operator==(const BlocServer& other) const;

        void print(int indent) const;

        // Getters
        const std::vector<s_ip_port>&           getIpTab() const;
        const std::vector<std::string>&         getServerNames() const;
        const std::map<int, std::string>&       getErrorPages() const;
        const std::vector<std::string>&         getIndex() const;
        bool                                    getGetMethod() const;
        bool                                    getPostMethod() const;
        bool                                    getDeleteMethod() const;
        bool                                    getAutoindex() const;
        const std::string&                      getRootPath() const;
        size_t                                  getClientMaxBodySize() const;
        const std::vector<BlocLocation>&        getLocationBlocs() const;
};

#endif
