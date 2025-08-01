/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:42:04 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:42:04 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server/ServerManager.hpp"

ServerManager* ServerManager::instance = NULL;

ServerManager::ServerManager()
{}

ServerManager::~ServerManager()
{}

void    ServerManager::signalHandler(int signum)
{
    if (signum == SIGINT)
    {
        std::cout << std::endl;
        Logger::log(Logger::INFO, "Stopping Webserv...");
        if (ServerManager::instance)
            ServerManager::instance->stop();
    }
    if (signum == SIGCHLD)
    {
        while (waitpid(-1, 0, WNOHANG) > 0) ;
    }
}

void    ServerManager::initConfig(std::string config_src)
{
    Logger::log(Logger::INFO, "Loading config...");
    _config.parse(config_src);
    Logger::log(Logger::INFO, "Config loaded successfully.");
}

void    ServerManager::_initRouter()
{
    _router = _config.initRouter();
}

bool    isDouble(const unsigned int val, std::vector<unsigned int> tab)
{
    for (std::vector<unsigned int>::iterator v_it = tab.begin(); v_it != tab.end(); v_it++)
    {
        if (val == *v_it)
            return true;
    }
    return false;
}

void    ServerManager::_initListenSockets()
{
    std::vector<unsigned int>    tmp;

    for (RouterMap::iterator it = _router.begin();
        it != _router.end(); ++it)
    {
        if (isDouble(it->first.port, tmp))
            continue;
        int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1)
            Logger::log(Logger::FATAL, "Initialisation error => socket error");
        if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => socket fcntl error");
        
        int yes = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => REUSEADDR error");

        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(it->first.port);
        sa.sin_addr.s_addr = INADDR_ANY;

        if (bind(socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => bind error " + std::string(strerror(errno)));
        if (listen(socket_fd, 128) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => listen error");
        
        _listen_sockets.push_back(socket_fd);
        tmp.push_back(it->first.port);
    }
}

void    ServerManager::_initExitPipe()
{
    if (pipe(_exit_pipe) == -1)
        Logger::log(Logger::FATAL, "Initialisation error => exit pipe error");
}

void    ServerManager::_initEpoll()
{
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1)
        Logger::log(Logger::FATAL, "Initialisation error => epoll creation error");
}

void    ServerManager::_linkEpollToListensFD()
{
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = *it;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, *it, &ev) == -1)
            Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl l_socket error");
    }
}

void    ServerManager::_linkEpollToExitPipe()
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _exit_pipe[0];
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _exit_pipe[0], &ev) == -1)
        Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl pipe error");
}

int     ServerManager::_isListenSocket(int event_fd)
{
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        if (event_fd == *it)
            return (*it);
    }
    return (-1);
}

void    ServerManager::_addClient(int fd) {
    _clients[fd] = new Client(fd, _router, this);
    _client_fds.push_back(fd);
}

void    ServerManager::_removeClient(int fd) {
    delete _clients[fd];
    _clients.erase(fd);
    _client_fds.remove(fd);
}

void    ServerManager::_closeAllClients() {
    for (std::map<int, Client*>::iterator it = _clients.begin();
        it != _clients.end();)
    {
        delete it->second;
        _clients.erase(it++);
    }
    for (std::list<int>::iterator it = _client_fds.begin();
         it != _client_fds.end(); ++it)
    {
        if (close(*it) == -1)
            Logger::log(Logger::FATAL, "close error");
    }
    _client_fds.clear();
}

void    ServerManager::addCGIlink(Client* client, int cgi_fd)
{
    _cgi_map.insert(std::make_pair(client, cgi_fd));
}

void    ServerManager::removeCGILink(int cgi_fd)
{
    for (std::multimap<Client*, int>::iterator it = _cgi_map.begin();
        it != _cgi_map.end(); )
    {
        if (cgi_fd == it->second)
            _cgi_map.erase(it++);
        else
            it++;
    }
}

Client*    ServerManager::_isCGIClient(int fd)
{
    for (std::multimap<Client*, int>::iterator it = _cgi_map.begin();
        it != _cgi_map.end(); it++)
    {
        if (fd == it->second)
            return (it->first);
    }
    return (NULL);
}

void    ServerManager::_cleanup()
{
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        if (close(*it) == -1)
            Logger::log(Logger::FATAL, "close error");
    }
    _closeAllClients();
    if (close(_exit_pipe[0]) == -1)
        Logger::log(Logger::FATAL, "close error");
    if (close(_exit_pipe[1]) == -1)
        Logger::log(Logger::FATAL, "close error");
    if (close(_epoll_fd) == -1)
        Logger::log(Logger::FATAL, "close error");

    Logger::log(Logger::INFO, "Webserv Stopped without issues.");
}

std::string print_listen_address(int socket_fd) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    memset(&sin, 0, sizeof(sin));

    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) == -1) {
        perror("getsockname");
        return NULL;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sin.sin_addr), ip, INET_ADDRSTRLEN);

    std::ostringstream oss;
    oss << ip
        << ":"
        << ntohs(sin.sin_port);
    return oss.str();
}

void    ServerManager::_logRunningInfos()
{
    Logger::log(Logger::INFO, "Webserv is running.");
    for (std::vector<int>::iterator it = _listen_sockets.begin();
        it != _listen_sockets.end(); ++it)
    {
        Logger::log(Logger::INFO, "Listening to " + print_listen_address(*it) + "...");
    }
    Logger::log(Logger::INFO, "Ctrl + C to clean stop the server.");
}

void    ServerManager::_sweepTimeout()
{
    time_t actual_time = time(NULL);
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end();)
    {
        if (actual_time - it->second->last_activity > 10)
        {
            try {
                it->second->last_activity = actual_time;
                if (it->second->state != Client::FINISHED)
                {
                    it->second->sendError("Server timeout");
                }
            }
            catch (const std::exception &e)
            {
                std::cout << e.what() << ". Couldn't send error to client." << std::endl;
            }
        }
        else
            ++it;
    }
}

void    ServerManager::_run()
{
    isRunning = true;
    while (isRunning)
    {
        struct epoll_event  events[128];
        int n = epoll_wait(_epoll_fd, events, 128, 1000);
        if (n == -1) {
            if (errno == EINTR) {
                if (!isRunning)
                    break;
                continue;
            }
            Logger::log(Logger::FATAL, "Initialisation error => epoll_wait error");
        }
        
        for (int i = 0; i < n; ++i)
        {
            if (events[i].data.fd == _exit_pipe[0])
            {
                isRunning = false;
                break;
            }
            int f_socket_fd;
            if ((f_socket_fd = _isListenSocket(events[i].data.fd)) != -1)
            {
                int c_socket_fd = accept(f_socket_fd, 0, 0);
                if (fcntl(c_socket_fd, F_SETFL, O_NONBLOCK) == -1)
                    Logger::log(Logger::FATAL, "Initialisation error => fcntl c_socket error");
                epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = c_socket_fd;
                if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, c_socket_fd, &ev) == -1)
                    Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl add c_socket error");
                _addClient(c_socket_fd);
            }
            else
            {
                Client* c_client;
                if ((c_client = _isCGIClient(events[i].data.fd)) != NULL) 
                {
                    c_client->last_activity = time(NULL);
                    try {
                        if (events[i].events & EPOLLIN || events[i].events & EPOLLHUP)
                        {
                            c_client->handleResponse(true, events[i].data.fd);
                        }
                        if (events[i].events & EPOLLOUT)
                        {
                            c_client->writeRequestBodyToCGI(events[i].data.fd);
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << e.what() << std::endl;
                        try {
                            if (c_client->state != Client::FINISHED)
                                c_client->sendError(e.what());
                        }
                        catch (const std::exception &e)
                        {
                            std::cout << e.what() << ". Couldn't send error to client." << std::endl;
                        }
                    }
                    c_client->last_activity = time(NULL);
                }
                else
                {
                    c_client = _clients[events[i].data.fd];
                    c_client->last_activity = time(NULL);
                    try 
                    {
                        if (events[i].events & EPOLLIN)
                        {
                            if (c_client->state == Client::DRAINING_BODY)
                            {
                                c_client->drainBody();
                            }
                            else
                            {
                                c_client->handleRequest();
                            }
                        }
                        if (events[i].events & EPOLLOUT)
                            c_client->handleSend();
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << e.what() << std::endl;
                        try {
                            if (c_client->state != Client::FINISHED)
                                c_client->sendError(e.what());
                        }
                        catch (const std::exception &e)
                        {
                            std::cout << e.what() << ". Couldn't send error to client." << std::endl;
                        }
                    }
                    c_client->last_activity = time(NULL);
                }
                if (c_client->state == Client::FINISHED)
                {
                    if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) == -1)
                        Logger::log(Logger::FATAL, "Initialisation error => epoll_ctl del c_socket error");
                    close(events[i].data.fd);
                    _removeClient(events[i].data.fd);
                }
            }
        }
        _sweepTimeout();
    }
    _cleanup();
}

void    ServerManager::run()
{
    if (!this->_config.getIsLoaded())
        Logger::log(Logger::FATAL, "Can't run server => config not loaded");

    _initRouter();
    _initListenSockets();
    _initExitPipe();
    _initEpoll();
    _linkEpollToListensFD();
    _linkEpollToExitPipe();
    _logRunningInfos();
    _run();
}

void    ServerManager::stop()
{
    if (write(_exit_pipe[1], "waf", 4) == -1)
        Logger::log(Logger::FATAL, "Write to fd error");
}

int   ServerManager::getEpollFd() const
{
    return (this->_epoll_fd);
}
