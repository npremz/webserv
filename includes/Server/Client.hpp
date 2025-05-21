/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:34 by npremont          #+#    #+#             */
/*   Updated: 2025/05/21 13:08:53 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>

# include "../Config/BlocServer.hpp"
# include "../Utils/utils.hpp"
# include "../Requests/HttpLexer.hpp"

class Client
{
    private:
        HttpLexer   _lexer;
        int         _socket_fd;
        RouterMap   _router;
        char        _buf[1024];
        std::string _request;
        

    public:
        bool        isFinished;
       
        Client(int fd, RouterMap& router);
        ~Client();

        void    handleRequest();
        void    handleResponse();
};
#endif
