/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:34 by npremont          #+#    #+#             */
/*   Updated: 2025/08/11 11:59:21 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <ctime>

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
    public: 
        enum RequestState { IDLE = 0, SENDING_ERROR, DRAINING_BODY, FINISHED };

    private:
        HttpLexer*      _lexer;
        int             _socket_fd;
        u_int32_t       _ip;
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
        size_t          _body_drained;
        size_t          _expected_body_size;
        
        BlocServer*     _responseRouting();
        
        bool            _isCGI();
        
        void            _addEpollout();
        void            _removeEpollout();
        void            _prepareAndSend();
    public:
        time_t          last_activity;
        RequestState    state;
        bool            timed_out;
        
        Client(int fd, u_int32_t ip, RouterMap& router, ServerManager* server);
        ~Client();
        
        void    addCGIEpollIn(int cgi_fd);
        void    addCGIEpollOut(int cgi_fd);
        void    writeRequestBodyToCGI(int cgi_fd);
        
        void    handleRequest();
        void    handleResponse(bool isCGIResponse = false, int cgi_fd = 0);
        void    handleSend();
        void    sendError(std::string error);
        void    drainBody();

        int                 getSockerFd() const; 
        const HttpLexer*    getLexer() const;
};
#endif
