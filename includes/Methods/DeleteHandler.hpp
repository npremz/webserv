/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 21:01:30 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:27:51 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETEHANDLER_HPP
# define DELETEHANDLER_HPP

# include "../Http/ErrorHandler.hpp"
# include "../Http/ResponseHandler.hpp"
# include "../Requests/CGI.hpp"
# include "../Server/Client.hpp"

class DeleteHandler{
    private:
        BlocServer*                 _ctx;
        const BlocLocation*         _location_ctx;
        HttpLexer::parsedRequest    _req;
        ErrorHandler*               _err;
        Client*                     _rep_parent;

    public:
        DeleteHandler(BlocServer* ctx, const BlocLocation* location_ctx,
            HttpLexer::parsedRequest req, ErrorHandler* err, Client* _parent);
        ~DeleteHandler();

        std::string handleRequest();
};

#endif
