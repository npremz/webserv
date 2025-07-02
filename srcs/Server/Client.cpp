/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/07/01 11:56:07 by npremont         ###   ########.fr       */
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
    if ((byte_rec = recv(_socket_fd, _buf, 1024, 0)) > 0)
    {
        HttpLexer::Status   c_status;

        c_status = _lexer.feed(_buf, byte_rec);
        if (c_status == HttpLexer::COMPLETE)
        {
            _response_ctx = _responseRouting();
            if (_response_ctx == NULL)
                Logger::log(Logger::ERROR, "Invalid Request => host not supported");
            Logger::log(Logger::DEBUG, "Request parsed");
            handleResponse();
            return ;
        }
        else if (c_status == HttpLexer::ERR)
            Logger::log(Logger::ERROR, "Invalid Request => lexer error");
    }
    else if (byte_rec == -1)
        Logger::log(Logger::ERROR, "Request reading error => closing connection.");
}

void    Client::handleResponse(bool isCGIResponse, int cgi_fd)
{
    Response rep(_response_ctx, _lexer.getRequest(), this);
    if (isCGIResponse)
    {
        _response_str = rep.createCGIResponseSTR(cgi_fd);
        close(cgi_fd);
        _server->removeCGILink(cgi_fd);
    }
    else
    {
        _response_str = rep.createResponseSTR();
        if (_response_str == "CGI")
        {
            Logger::log(Logger::DEBUG, "Main process paused response, listening to child for cgi.");
            return ;
        }
    }
    Logger::log(Logger::DEBUG, "Response created");
    _prepareAndSend();
}

void    Client::addCGIEpollIn(int cgi_fd)
{
    _server->addCGIlink(this, cgi_fd);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = cgi_fd;
    if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_ADD, cgi_fd, &ev) == -1)
        Logger::log(Logger::ERROR, "Exec error => epoll_ctl cgi_pipe error");
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
    if (_response_sent < _response_len) {
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
        } else if (n == -1) {
            Logger::log(Logger::ERROR, "Response sending error => closing connection.");
            return;
        }
    }
}
