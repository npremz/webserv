/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:36:31 by npremont          #+#    #+#             */
/*   Updated: 2025/08/12 18:33:59 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/Client.hpp"

Client::Client(int fd, u_int32_t ip, RouterMap& router, ServerManager* server) :
    _socket_fd(fd),
    _ip(ip),
    _router(router),
    _response_len(0),
    _response_sent(0),
    _response_ctx(NULL),
    _rep(NULL),
    _server(server),
    _bytes_to_cgi_stdin(0),
    last_activity(time(NULL)),
    state(IDLE),
    timed_out(false)
{
    Logger::log(Logger::INFO, "Accepted client from " + ipIntToString(_ip));
    try{
        _lexer = new HttpLexer(_ip);
    }
    catch (const std::exception& e)
    {
        Logger::log(Logger::ERROR, "Client initialisation failed: " + std::string(e.what()));
    }
}

Client::~Client()
{
    Logger::log(Logger::INFO, "Closing connection with client from " + ipIntToString(_ip));
    delete _rep;
    delete _lexer;
}

void    Client::sendError(std::string error)
{
    state = SENDING_ERROR;
    if (error == "Server timeout")
        _lexer->setEndStatus(408);
    if (error == "Bad Request")
        _lexer->setEndStatus(400);
    _response_ctx = _responseRouting();
    Response rep(_response_ctx, _lexer->getRequest(), this);
    _response_str = rep.sendError(error);
    _prepareAndSend();
}

void    Client::drainBody()
{
    char buf[4096];
    while (_body_drained < _expected_body_size) {
        ssize_t n = recv(_socket_fd, buf, sizeof(buf), 0);
        if (n > 0) {
            _body_drained += n;
        } else if (n == 0) {
            break;
        } else if (n == -1){
            break;
        } else {
            break;
        }
    }
    if (_body_drained >= _expected_body_size) {
        state = FINISHED;
    }
}

BlocServer*     Client::_responseRouting()
{
    BlocServer* ctx_to_return = NULL;
    const HttpLexer::parsedRequest& req = _lexer->getRequest();
    for (RouterMap::iterator it = _router.begin(); it != _router.end(); it++)
    {
        if (req.ip_port.port == it->first.port)
        {
            for (std::vector<BlocServer>::iterator ctx_it = it->second.begin();
                ctx_it != it->second.end(); ++ctx_it)
            {
                for (std::vector<s_ip_port>::const_iterator ip_it = ctx_it->getIpTab().begin();
                    ip_it != ctx_it->getIpTab().end(); ++ip_it)
                {
                    Logger::log(Logger::DEBUG, "Request ip: " + ipPortToString(req.ip_port));
                    Logger::log(Logger::DEBUG, "BlocServer ip: " + ipPortToString(*ip_it));
                    if (req.ip_port.ip == ip_it->ip)
                    {
                        if (ctx_to_return == NULL)
                            ctx_to_return = &(*ctx_it);
                        for (std::vector<std::string>::const_iterator host_it = (*ctx_it).getServerNames().begin();
                            host_it != (*ctx_it).getServerNames().end(); host_it++)
                        {
                            if ((*host_it) == req.host)
                                return &(*ctx_it);
                        }
                    }
                }
            }
        }
    }
    if (ctx_to_return != NULL)
        return (ctx_to_return);
    for (RouterMap::iterator it = _router.begin(); it != _router.end(); it++)
    {
        if (req.ip_port.port == it->first.port)
        {
            for (std::vector<BlocServer>::iterator ctx_it = it->second.begin();
                ctx_it != it->second.end(); ++ctx_it)
            {
                for (std::vector<s_ip_port>::const_iterator ip_it = ctx_it->getIpTab().begin();
                    ip_it != ctx_it->getIpTab().end(); ++ip_it)
                {
                    if (0 == ip_it->ip)
                    {
                        Logger::log(Logger::DEBUG, "Ip port routed: " + ipPortToString(*ip_it));
                        return &(*ctx_it);
                    }
                }
            }
        }
    }
    return NULL;
}

bool    Client::_isCGI()
{   
    std::string request_path = _response_ctx->getRootPath() + _lexer->getRequest().path;
    
    size_t dot_pos = request_path.find_last_of('.');
    std::string ext = request_path.substr(dot_pos);

    if (".php" == ext || ".py" == ext)
        return (true);
    return (false);
}

void    Client::writeRequestBodyToCGI(int cgi_fd)
{
    ssize_t bytes_written = 0;
    size_t write_size = (_bytes_to_cgi_stdin + MAX_CHUNK_SIZE > _lexer->getRequest().expectedoctets)
        ? _lexer->getRequest().expectedoctets - _bytes_to_cgi_stdin
        : MAX_CHUNK_SIZE;

    if ((bytes_written = write(cgi_fd, _lexer->getRequest().body.data() + _bytes_to_cgi_stdin, write_size)) > 0)
    {    
        last_activity = time(NULL);
        _bytes_to_cgi_stdin += bytes_written;
        if (_bytes_to_cgi_stdin == _lexer->getRequest().expectedoctets)
        {
            if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_DEL, cgi_fd, NULL) == -1)
                Logger::log(Logger::ERROR, "Error closing cgi writing fd after writing request body to CGI STDIN.");
            close(cgi_fd);
            _server->removeCGILink(cgi_fd);
            Logger::log(Logger::DEBUG, "Request body written to CGI STDIN.");
        }
    }
    else if (bytes_written == -1)
    {
        if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_DEL, cgi_fd, NULL) == -1)
            Logger::log(Logger::ERROR, "Error closing cgi writing fd after error writing request body to CGI STDIN.");
        close(cgi_fd);
        Logger::log(Logger::ERROR, "Error writing request body to CGI STDIN.");
    }
}

void    Client::handleRequest()
{
    long  byte_rec = 0;
    if ((byte_rec = recv(_socket_fd, _buf, 1024, 0)) > 0)
    {
        HttpLexer::Status   c_status;

        last_activity = time(NULL);
        c_status = _lexer->feed(_buf, byte_rec);
        if (c_status == HttpLexer::MUST_CHECK)
        {
            _response_ctx = _responseRouting();
            if (_response_ctx == NULL)
                Logger::log(Logger::ERROR, "Invalid Request => host not supported");
            Logger::log(Logger::DEBUG, "Checking if request will be rejected...");
            handleChecking();
            return ;
        }
        if (c_status == HttpLexer::COMPLETE)
        {
            _response_ctx = _responseRouting();
            if (_response_ctx == NULL)
            {
                _lexer->setEndStatus(400);
                Logger::log(Logger::ERROR, "Invalid Request => host not supported");
            }
            Logger::log(Logger::DEBUG, "Request parsed");
            try
            {
                _rep = new Response(_response_ctx, _lexer->getRequest(), this);
            }
            catch (const std::exception& e)
            {
                Logger::log(Logger::ERROR, "Response initialisation failed: " + std::string(e.what()));
            }
            handleResponse();
            return ;
        }
        else if (c_status == HttpLexer::ERR)
            Logger::log(Logger::ERROR, "Invalid Request => lexer error");
    }
    else if (byte_rec == -1)
        Logger::log(Logger::ERROR, "Request reading error => closing connection.");
    else
        return;
}

void    Client::handleChecking()
{
    Response rep_checker(_response_ctx, _lexer->getRequest(), this);
    _response_str = rep_checker.checkRequest();
    last_activity = time(NULL);
    _prepareAndSend();
}

void    Client::handleResponse(bool isCGIResponse, int cgi_fd)
{
    if (isCGIResponse)
    {
        _response_str = _rep->createCGIResponseSTR(cgi_fd);
        last_activity = time(NULL);
        if (_response_str == "Not complete.")
        {
            Logger::log(Logger::DEBUG, "CGI pipe reading not completed, back to epoll");
            return;
        }
        if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_DEL, cgi_fd, NULL) == -1)
            Logger::log(Logger::ERROR, "Error while deleting cgi pipe from epoll.");
        close(cgi_fd);
        _server->removeCGILink(cgi_fd);
    }
    else
    {
        _response_str = _rep->createResponseSTR();
        if (_response_str == "CGI")
        {
            Logger::log(Logger::DEBUG, "Main process paused response, listening to child for cgi.");
            return ;
        }
    }
    Logger::log(Logger::DEBUG, "Response created");
    _prepareAndSend();
}

void    Client::addCGIEpollOut(int cgi_fd)
{
    if (fcntl(cgi_fd, F_SETFL, O_NONBLOCK) == -1)
        Logger::log(Logger::ERROR, "Initialisation error => fcntl cgi_pipe error");
    _server->addCGIlink(this, cgi_fd);
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = cgi_fd;
    if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_ADD, cgi_fd, &ev) == -1)
        Logger::log(Logger::ERROR, "Exec error => epoll_ctl cgi_pipe error");
}

void    Client::addCGIEpollIn(int cgi_fd)
{
    if (fcntl(cgi_fd, F_SETFL, O_NONBLOCK) == -1)
        Logger::log(Logger::ERROR, "Initialisation error => fcntl cgi_pipe error");
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
    if (state == SENDING_ERROR)
        ev.events = EPOLLOUT;
    else
        ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = _socket_fd;
    if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_MOD, _socket_fd, &ev) == -1)
        Logger::log(Logger::ERROR, "Exec error => epoll_ctl addEpollout");
}

void    Client::_removeEpollout()
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _socket_fd;
    if (epoll_ctl(_server->getEpollFd(), EPOLL_CTL_MOD, _socket_fd, &ev) == -1)
        Logger::log(Logger::ERROR, "Exec error => epoll_ctl removeEpollout");
}

void    Client::_prepareAndSend()
{
    _response_len  = _response_str.size();
    _response_sent = 0;
    _addEpollout();
}

void    Client::handleSend()
{
    if (_response_sent < _response_len)
    {
        Logger::log(Logger::DEBUG, "Sending response...");
        size_t to_send = std::min(_response_str.size() - _response_sent, (size_t)MAX_CHUNK_SIZE);
        ssize_t n = send(_socket_fd, _response_str.data() + _response_sent, to_send, MSG_NOSIGNAL);

        if (n > 0)
        {
            last_activity = time(NULL);
            _response_sent += n;
            if (_response_sent == _response_len)
            {
                _removeEpollout();
                Logger::log(Logger::DEBUG, "Response totally sent");
                if (state == SENDING_ERROR && _lexer->getRequest().expectedoctets > 0
                    && _lexer->getEndStatus() != 408)
                {
                    state = DRAINING_BODY;
                    _body_drained = _lexer->getRequest().receivedoctets;
                    _expected_body_size = _lexer->getRequest().expectedoctets;
                }
                else if (state == ACCEPTING_CONTINUE)
                {
                    state = IDLE;
                    Logger::log(Logger::DEBUG, "100 Continue sent, waiting for more.");
                }
                else
                {
                    state = FINISHED;
                    Logger::log(Logger::DEBUG, "Client State set to FINISHED.");
                }
            }
        }
        else if (n == -1)
        {
            _lexer->setEndStatus(500);
            _removeEpollout();
            state = FINISHED;
            Logger::log(Logger::ERROR, "Response sending error => closing connection.");
        }
    }
}

int Client::getSockerFd() const
{
    return (this->_socket_fd);
}

const HttpLexer*    Client::getLexer() const
{
    return (this->_lexer);
}
