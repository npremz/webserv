/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/05/22 13:09:59 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Client.hpp"

Client::Client(int fd, RouterMap& router) :
    _socket_fd(fd),
    _router(router),
    isFinished(false)
{}

Client::~Client()
{}

void    Client::handleRequest()
{
    long  byte_rec = 0;
    while ((byte_rec = recv(_socket_fd, _buf, 1024, 0)) > 0)
    {
        HttpLexer::Status   c_status;

        c_status = _lexer.feed(_buf, byte_rec);
        if (c_status == HttpLexer::COMPLETE)
        {
            isFinished = true;
            break;
        }
        else if (c_status == HttpLexer::ERR)
            Logger::log(Logger::ERROR, "Invalid Request code: jmenfou");
    }
}
