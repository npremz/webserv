/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 13:19:20 by npremont          #+#    #+#             */
/*   Updated: 2025/06/17 14:10:01 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <istream>
# include <iostream>

# include "../Utils/utils.hpp"
# include "../Config/BlocServer.hpp"
# include "../Requests/HttpLexer.hpp"
# include "../Logger/Logger.hpp"

class Response
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;

        std::string                 _createError(unsigned int code, std::string error, std::string bodyStr);
        std::string                 _createResponse(unsigned int code, std::string msg, std::string bodyStr);

        std::string                 _handleLexerErrors();
        bool                        _isMethodSupportedByRoute();
        bool                        _isLocation();

    public:
        Response(BlocServer* ctx, HttpLexer::parsedRequest req);
        ~Response();

        std::string createResponseSTR();
};

#endif
