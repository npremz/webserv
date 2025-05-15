/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlocLocation.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 23:21:19 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 23:21:19 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCLOCATION_HPP
# define BLOCLOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <fstream>

# include "../../includes/Logger/Logger.hpp"
# include "../../includes/Utils/utils.hpp"

class BlocServer;

class BlocLocation 
{
    struct redirection_config {
        int         code;
        std::string url;
        bool        is_set;
    };

    typedef void (BlocLocation::*HandlerFunc)(std::vector<std::string>);

    private:
        BlocServer*                         _parent;
        std::string                         _location_path;
        std::map<std::string, HandlerFunc>  _function_table;

        bool                                _get;
        bool                                _post;
        bool                                _delete;
        bool                                _autoindex;
        bool                                _upload_enable;
        std::string                         _upload_path;
        std::string                         _cgi_extension;
        std::string                         _cgi_pass;
        std::string                         _root_path;
        std::vector<std::string>            _index;
        redirection_config                  _return;

        void        _initFunctionTable();
        void        _handlingContent(std::vector<std::string> content);
        void        _tokensRedirect(std::vector<std::string> tokens);

        void        _handleMethods(std::vector<std::string> tokens);
        void        _handleRoot(std::vector<std::string> tokens);
        void        _handleAutoIndex(std::vector<std::string> tokens);
        void        _handleIndex(std::vector<std::string> tokens);
        void        _handleUploadEnable(std::vector<std::string> tokens);
        void        _handleUploadPath(std::vector<std::string> tokens);
        void        _handleCGIExt(std::vector<std::string> tokens);
        void        _handleCGIPass(std::vector<std::string> tokens);
        void        _handleRedirect(std::vector<std::string> tokens);

    public:
        BlocLocation(BlocServer* parent, std::string location_path,
            std::vector<std::string> content);
        //BlocLocation(const BlocLocation& other);
        //BlocLocation& operator=(const BlocLocation& other);
        ~BlocLocation();

        void print(int indent) const;
};

#endif
