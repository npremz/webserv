/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 11:44:34 by npremont          #+#    #+#             */
/*   Updated: 2025/07/18 17:57:41 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/CGI.hpp"

CGI::CGI(std::string method, HttpLexer::parsedRequest req,
    const std::string& content_type, BlocServer* ctx, const BlocLocation* location_ctx,
    Client* parent) :
    _method(method),
    _req(req),
    _ctx(ctx),
    _location_ctx(location_ctx),
    _client(parent),
    _content_type(content_type)
{
    (void)_ctx;
}

CGI::~CGI()
{}

void    CGI::_initEnvTab()
{
    _env_tab.push_back(makeEnvVar("REQUEST_METHOD", _method));
    _env_tab.push_back(makeEnvVar("SCRIPT_FILENAME", _file_name));
    _env_tab.push_back(makeEnvVar("SCRIPT_NAME", _location_ctx->getCGIPass()));
    _env_tab.push_back(makeEnvVar("QUERY_STRING", _req.query));
    _env_tab.push_back(makeEnvVar("GATEWAY_INTERFACE", "CGI/1.1"));
    _env_tab.push_back(makeEnvVar("SERVER_PROTOCOL", "HTTP/1.1"));
    _env_tab.push_back(makeEnvVar("SERVER_SOFTWARE", "Webserv/0.1"));
    _env_tab.push_back(makeEnvVar("SERVER_NAME", "localhost"));
    _env_tab.push_back(makeEnvVar("DOCUMENT_ROOT", _location_ctx->getRootPath() + _location_ctx->getLocationPath()));
    _env_tab.push_back(makeEnvVar("REDIRECT_STATUS", "200"));
    _env_tab.push_back(makeEnvVar("REMOTE_ADDR", "127.0.0.1"));
    _env_tab.push_back(makeEnvVar("UPLOAD_ENABLE", _location_ctx->getUploadEnable()));
    _env_tab.push_back(makeEnvVar("UPLOAD_PATH", _location_ctx->getUploadPath()));
    
    if (_req.contentType.size() > 0)
        _env_tab.push_back(makeEnvVar("CONTENT_TYPE", _req.contentType));
    if (_req.expectedoctets > 0)
    {
        std::ostringstream oss;
        oss << _req.expectedoctets;
        _env_tab.push_back(makeEnvVar("CONTENT_LENGTH", oss.str()));
    }
    
    size_t      slash_pos = _req.path.find_last_of("/");
    std::string target_file = _req.path.substr(slash_pos + 1);
    _env_tab.push_back(makeEnvVar("REQUEST_URI", target_file));
    
    
    std::string host_string = ipPortToString(_req.host);
    size_t pos = host_string.find(":");
    _env_tab.push_back(makeEnvVar("SERVER_PORT", host_string.substr(pos + 1)));


    for (HttpLexer::HeaderMap::iterator it = _req.headers.begin();
        it != _req.headers.end(); it++)
    {
        std::string env = "HTTP_";
        std::string header_name = it->first;
        for (size_t i = 0; i < header_name.size(); ++i) {
            char c = header_name[i];
            if (c == '-')
                env += '_';
            else
                env += std::toupper(c);
        }
        _env_tab.push_back(makeEnvVar(env, it->second));
    }

    _env_tab.push_back(NULL);
    
    // Logger::log(Logger::DEBUG, "CGI env:");
    // for (std::vector<char *>::iterator it = _env_tab.begin();
    //     it != _env_tab.end() - 1; it++)
    // {
    //     Logger::log(Logger::DEBUG, "\t" + std::string(*it));
    // }
}

void CGI::_initArgv()
{
    _file_name = _req.path.substr(_req.path.find_last_of("/") + 1);

    if ((_method == "POST" || _method == "DELETE") && !isDirectory(_file_name))
    {
        _argv.push_back(const_cast<char *>(_location_ctx->getCGIPass().c_str()));
        _argv.push_back(const_cast<char *>(_file_name.c_str()));
    }
    else
        _argv.push_back(const_cast<char *>(_location_ctx->getCGIPass().c_str()));
    if (_method == "GET")
        _argv.push_back(const_cast<char *>(_file_name.c_str()));
    _argv.push_back(NULL);

    // Logger::log(Logger::DEBUG, "CGI argv:");
    // for (std::vector<char *>::iterator it = _argv.begin();
    //     it != _argv.end() - 1; it++)
    // {
    //     Logger::log(Logger::DEBUG, "\t" + std::string(*it));
    // }
}

void    CGI::exec()
{
    if (pipe(_cgi_pipe_output) == -1)
        Logger::log(Logger::ERROR, "Output pipe error.");

    if (_method == "POST" && pipe(_cgi_pipe_input) == -1)
        Logger::log(Logger::ERROR, "Input pipe error.");

    pid_t pid;
    if ((pid = fork()) == -1)
        Logger::log(Logger::ERROR, "Fork error.");

    if (pid == 0)
    {
        if (_method == "POST")
        {
            dup2(_cgi_pipe_input[0], STDIN_FILENO);
            close(_cgi_pipe_input[0]);
            close(_cgi_pipe_input[1]);
        }
        close(_cgi_pipe_output[0]);
        dup2(_cgi_pipe_output[1], STDOUT_FILENO);
        dup2(_cgi_pipe_output[1], STDERR_FILENO);
        close(_cgi_pipe_output[1]);
        _initArgv();
        _initEnvTab();

        size_t      slash_pos = _req.path.find_last_of("/");
        std::string cgi_path = _req.path.substr(0, slash_pos);
        if (chdir((_location_ctx->getRootPath() + cgi_path).c_str()) != 0)
            exit(1);

        execve(_location_ctx->getCGIPass().c_str(), _argv.data(), _env_tab.data());
        exit(1);
    }
    Logger::log(Logger::DEBUG, "Main process passed fork.");
    if (_method == "POST")
    {
        _client->addCGIEpollOut(_cgi_pipe_input[1]);
        close(_cgi_pipe_input[0]);
    }
    close(_cgi_pipe_output[1]);
    _client->addCGIEpollIn(_cgi_pipe_output[0]);
}
