/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/06/18 18:17:20 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Client.hpp"

Client::Client(int fd, RouterMap& router, ServerManager* server) :
    _socket_fd(fd),
    _router(router),
    _response_len(0),
    _response_sent(0),
    _response_ctx(NULL),
    _server(server),
    isFinished(false)
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

bool    Client::_isCGI()
{   
    std::string request_path = _response_ctx->getRootPath() + _lexer.getRequest().path;
    
    size_t dot_pos = request_path.find_last_of('.');
    std::string ext = request_path.substr(dot_pos);

    if (".php" == ext || ".py" == ext)
        return (true);
    return (false);
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
            Logger::log(Logger::DEBUG, "Request parsed");
            if (_isCGI())
            {
                break;
            }
            handleResponse();
            Logger::log(Logger::DEBUG, "Response created");
            break;
        }
        else if (c_status == HttpLexer::ERR)
            Logger::log(Logger::ERROR, "Invalid Request => lexer error");
    }
}

void    Client::handleResponse()
{
    Response rep(_response_ctx, _lexer.getRequest());
    _response_str = rep.createResponseSTR();
    _prepareAndSend();
}

void    Client::_addEpollout()
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = _socket_fd;
    epoll_ctl(_server->getEpollFd(), EPOLL_CTL_MOD, _socket_fd, &ev);
}

void    Client::_removeEpollout()
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _socket_fd;
    epoll_ctl(_server->getEpollFd(), EPOLL_CTL_MOD, _socket_fd, &ev); 
}

void    Client::_prepareAndSend()
{
    _response_len  = _response_str.size();
    _response_sent = 0;
    _addEpollout();
}

void    Client::handleSend()
{
    while (_response_sent < _response_len) {
        Logger::log(Logger::DEBUG, "Sending response...");
        size_t to_send = std::min(_response_str.size() - _response_sent, (size_t)MAX_CHUNK_SIZE);
        ssize_t n = send(_socket_fd, _response_str.data() + _response_sent, to_send, MSG_NOSIGNAL);

        if (n > 0) {
            _response_sent += n;
            if (_response_sent == _response_len) {
                _removeEpollout();
                Logger::log(Logger::DEBUG, "Response totally sent");
                isFinished = true;
            }
        } else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            Logger::log(Logger::DEBUG, "Response partially sent");
            break;
        } else {
            Logger::log(Logger::DEBUG, "CATASTROPHE");
            // A faire: clean client + fermeture.
            break;
        }
    }
}
