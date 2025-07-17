/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:34 by npremont          #+#    #+#             */
/*   Updated: 2025/07/16 18:57:40 by npremont         ###   ########.fr       */
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
class Response;

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
        Response*       _rep;
        ServerManager*  _server;
        size_t          _bytes_to_cgi_stdin;
        bool            _isSendingError;

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
        void    addCGIEpollOut(int cgi_fd);
        void    writeRequestBodyToCGI(int cgi_fd);
        
        void    handleRequest();
        void    handleResponse(bool isCGIResponse = false, int cgi_fd = 0);
        void    handleSend();
        void    sendError(std::string error);

        int     getSockerFd() const; 
};
#endif
