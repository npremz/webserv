/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:01:30 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 21:49:33 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETHANDLER_HPP
# define GETHANDLER_HPP

# include <dirent.h>
# include <algorithm>

# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHandler.hpp"
# include "../Requests/CGI.hpp"
# include "../Server/Client.hpp"

class GetHandler{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        ErrorHandler*               _err;
        Client*                     _rep_parent;
        std::string                 _content_type;
        
        //returns "none" if unset
        std::string _testIndex(std::string URI);
        void        _initContentType(std::string file);
        std::string _generateAutoIndex(std::string fullpath);
        bool        _handleGetCGI();

    public:
        GetHandler(BlocServer* ctx, const BlocLocation* location_ctx,
            HttpLexer::parsedRequest req, ErrorHandler* err, Client* _parent);
        ~GetHandler();

        std::string handleRequest();
};

#endif
