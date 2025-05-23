/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/05/23 11:03:12 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Client.hpp"

Client::Client(int fd, RouterMap& router) :
    _socket_fd(fd),
    _router(router),
    isFinished(false),
    _response_ctx(NULL)
{}

Client::~Client()
{}

BlocServer*     Client::_responseRouting()
{
    const HttpLexer::parsedRequest& req = _lexer.getRequest();
    for (RouterMap::iterator it = _router.begin(); it != _router.end(); it++)
    {
        if (req.host.port == it->first.port)
        {
            for (std::vector<BlocServer>::iterator ctx_it = it->second.begin();
                ctx_it != it->second.end(); ++ctx_it)
            {
                for (std::vector<s_ip_port>::const_iterator ip_it = ctx_it->getIpTab().begin();
                    ip_it != ctx_it->getIpTab().end(); ++ip_it)
                {
                    if (ip_it->ip == 0)
                        return &(*ctx_it);
                    if (req.host.ip == ip_it->ip)
                        return &(*ctx_it);
                }
            }
        }
    }
    return NULL;
}

void    Client::handleRequest()
{
    long  byte_rec = 0;
    while ((byte_rec = recv(_socket_fd, _buf, 1024, 0)) > 0)
    {
        HttpLexer::Status   c_status;

        c_status = _lexer.feed(_buf, byte_rec);
        if (c_status == HttpLexer::COMPLETE)
        {
            _response_ctx = _responseRouting();
            if (_response_ctx == NULL)
                Logger::log(Logger::ERROR, "Invalid Request => host not supported");
            break;
        }
        else if (c_status == HttpLexer::ERR)
            Logger::log(Logger::ERROR, "Invalid Request => lexer error");
    }
}

void    Client::handleResponse()
{
    isFinished = true;
}
