/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:34 by npremont          #+#    #+#             */
/*   Updated: 2025/06/19 19:03:48 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>

# include "../Config/BlocServer.hpp"
# include "../Server/ServerManager.hpp"
# include "../Utils/utils.hpp"
# include "../Requests/HttpLexer.hpp"
# include "../Requests/Response.hpp"
# include "../Requests/CGI.hpp"


class ServerManager;

class Client
{
    private:
        HttpLexer       _lexer;
        int             _socket_fd;
        RouterMap       _router;
        char            _buf[16384];
        std::string     _request;
        std::string     _response_str;
        size_t          _response_len;
        size_t          _response_sent;
        BlocServer*     _response_ctx;
        ServerManager*  _server;

        BlocServer*     _responseRouting();

        bool            _isCGI();
        
        void            _addEpollout();
        void            _removeEpollout();
        void            _prepareAndSend();
        public:
        bool        isFinished;
        
        Client(int fd, RouterMap& router, ServerManager* server);
        ~Client();
        
        void    addCGIEpollIn(int cgi_fd);
        
        void    handleRequest();
        void    handleResponse(bool isCGIResponse = false, int cgi_fd = 0);
        void    handleSend();
};
#endif
