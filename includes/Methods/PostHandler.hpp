/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:01:30 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:24:01 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTHANDLER_HPP
# define POSTHANDLER_HPP

# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHandler.hpp"
# include "../Requests/CGI.hpp"
# include "../Server/Client.hpp"

class PostHandler
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        ErrorHandler*               _err;
        Client*                     _rep_parent;

        std::string                 _handleUpload(std::string uploadDir);
    
    public:
        PostHandler(BlocServer* ctx, const BlocLocation* location_ctx,
            HttpLexer::parsedRequest req, ErrorHandler* err, Client* _parent);
        ~PostHandler();

        std::string handleRequest();
        
};

#endif
