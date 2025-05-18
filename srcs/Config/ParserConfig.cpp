/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 10:58:08 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 10:58:08 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Config/ParserConfig.hpp"

ParserConfig::ParserConfig() : _isLoaded(false), _file_src("")
{}

ParserConfig::~ParserConfig()
{}

std::vector<std::string>    ParserConfig::_tokenise(const std::string& input)
{
    std::vector<std::string> tokens;
    std::string current;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '#')
        {
            while (c != '\n' && c != '\0' && c != EOF)
                ++i;
        }
        else if (c == '{' || c == '}' || c == ';') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty())
        tokens.push_back(current);
    return tokens;
}

void    ParserConfig::parse(std::string file_src)
{
    this->_file_src = file_src;
    std::ifstream file(_file_src.c_str());

    if (!file.is_open())
    {
        Logger::log(Logger::FATAL, "unable to open config file.");
        return ;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string conf = buffer.str();

    std::vector<std::string> file_tokens = this->_tokenise(conf);

    for (std::vector<std::string>::iterator it = file_tokens.begin();
    it != file_tokens.end(); )
    {
        if (*it == "server")
        {
            if (++it == file_tokens.end() || *it != "{")
                Logger::log(Logger::FATAL, "invalid config file. => expected '{' after 'server'");
            ++it;
            int scope = 1;
            std::vector<std::string> bloc;
            while (it != file_tokens.end() && scope > 0)
            {
                if (*it == "{")
                    ++scope;
                else if (*it == "}")
                    --scope;
                if (scope > 0)
                {
                    bloc.push_back(*it);
                    ++it;
                }
            }
            if (scope != 0)
                Logger::log(Logger::FATAL, "invalid config file. => unmatched '{' in server block");
            ++it;

            BlocServer serv(bloc);
            this->_servers_ctx.push_back(serv);
        }
        else if (*it == "{" || *it == "}")
            Logger::log(Logger::FATAL, "invalid config file. => misplaced '{' or '}' at root scope");
        else
            Logger::log(Logger::FATAL, "invalid config file. => unexpected token at global scope: '" + *it + "'");
    }
    this->_isLoaded = true;
}

static void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
}

void ParserConfig::print(int indent) const
{
    printIndent(indent);
    std::cout << "ParserConfig:" << std::endl;

    printIndent(indent+1); std::cout << "file_src: " << _file_src << std::endl;
    printIndent(indent+1); std::cout << "servers:" << std::endl;
    for (std::vector<BlocServer>::const_iterator it = _servers_ctx.begin(); it != _servers_ctx.end(); ++it) {
        it->print(indent+2);
    }
}

RouterMap   ParserConfig::initRouter()
{
    RouterMap                       router;
    const std::vector<BlocServer>&  servers = _servers_ctx;

    for (std::vector<BlocServer>::const_iterator bloc_it = servers.begin();
        bloc_it != servers.end(); ++bloc_it)
    {
        const std::vector<s_ip_port>& ip_tab = bloc_it->getIpTab();

        for (std::vector<s_ip_port>::const_iterator ip_it = ip_tab.begin();
            ip_it != ip_tab.end(); ++ip_it)
        {
            if (router.find(*ip_it) != router.end())
            {
                bool isInMap = false;
                for (std::vector<BlocServer>::const_iterator it = router[*ip_it].begin();
                    it != router[*ip_it].end(); ++it)
                {
                    if (*it == *bloc_it)
                        isInMap = true;
                }
                if (!isInMap)
                    router[*ip_it].push_back(*bloc_it);
            }
            else
            {
                std::vector<BlocServer> tab;
                tab.push_back(*bloc_it);
                router[*ip_it] = tab;
            }
        }
    }
    return (router);
}

//Getters

bool    ParserConfig::getIsLoaded() const
{
    return _isLoaded;
}

const std::vector<BlocServer>&    ParserConfig::getServersCtx() const
{
    return _servers_ctx;
}
