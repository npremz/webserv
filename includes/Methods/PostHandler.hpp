/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:01:30 by npremont          #+#    #+#             */
/*   Updated: 2025/08/20 15:28:34 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTHANDLER_HPP
# define POSTHANDLER_HPP

# include <ctime>
# include <fstream>

# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHandler.hpp"
# include "../Requests/CGI.hpp"
# include "../Server/Client.hpp"
# include "./PostMultiPartHandler.hpp"

class PostHandler
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        ErrorHandler*               _err;
        Client*                     _rep_parent;
        std::string                 _fullpath;

        std::string                 _handleUpload(std::string uploadDir);
        std::string                 _handleNativePost();

        std::string                 _handlePlainText();
        std::string                 _handleMultiPart();

    public:
        PostHandler(BlocServer* ctx, const BlocLocation* location_ctx,
            HttpLexer::parsedRequest req, ErrorHandler* err, Client* _parent);
        ~PostHandler();

        std::string handleRequest();
        
};

#endif
