/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/05/20 13:15:29 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Client.hpp"

Client::Client(int fd, RouterMap& router) : _socket_fd(fd), _router(router)
{}

Client::~Client()
{}

void    Client::handleRequest()
{
    size_t  byte_rec = 0;
    while ((byte_rec = recv(_socket_fd, _buf, 1024, 0)) > 0)
    {
        write(1, _buf, byte_rec);
        if (_lexer.feed(_buf, byte_rec) == HttpLexer::COMPLETE)
            break;
    }
}
