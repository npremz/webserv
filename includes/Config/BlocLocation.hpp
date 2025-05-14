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
    private:
        BlocServer* _parent;

        bool        _get;
        bool        _post;
        bool        _delete;
        bool        _autoindex;
        bool        _upload_enable;
        std::string _cgi_extension;
        std::string _cgi_pass;
        std::string _root_path;
        std::string _index;
        int         _redirect_code;
        std::string _redirect_path;

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
        BlocLocation(BlocServer* parent);
        //BlocLocation(const BlocLocation& other);
        //BlocLocation& operator=(const BlocLocation& other);
        ~BlocLocation();

        void    parseLocation(std::ifstream& file);
};

#endif
