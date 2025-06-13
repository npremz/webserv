/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:34 by npremont          #+#    #+#             */
/*   Updated: 2025/06/13 10:58:32 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>

# include "../Config/BlocServer.hpp"
# include "../Server/ServerManager.hpp"
# include "../Utils/utils.hpp"
# include "../Requests/HttpLexer.hpp"

class ServerManager;

class Client
{
    private:
        HttpLexer       _lexer;
        int             _socket_fd;
        RouterMap       _router;
        char            _buf[4096];
        std::string     _request;
        std::string     _response_str;
        size_t          _response_len;
        size_t          _response_sent;
        BlocServer*     _response_ctx;
        ServerManager*  _server;

        BlocServer*     _responseRouting();
        
        void            _addEpollout();
        void            _removeEpollout();
        void            _prepareAndSend();

    public:
        bool        isFinished;
       
        Client(int fd, RouterMap& router, ServerManager* server);
        ~Client();

        void    handleRequest();
        void    handleResponse();
        void    handleSend();

};
#endif
