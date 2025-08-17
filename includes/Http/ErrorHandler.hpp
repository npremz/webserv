/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 10:38:22 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 16:03:39 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include "../Config/BlocServer.hpp"
# include "../Requests/HttpLexer.hpp"

class ErrorHandler
{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;

    public:
        ErrorHandler(BlocServer* ctx, const BlocLocation* location_ctx, 
            HttpLexer::parsedRequest req);
        ~ErrorHandler();

        // empty if none
        std::string getCustomErrorPath(unsigned int code);

        std::string createCustomError(unsigned int code, std::string error_page,
            std::string error_msg);
        std::string createError(unsigned int code, std::string error,
            std::string bodyStr);
        std::string sendError(std::string error);
        std::string handleLexerErrors();
};

#endif