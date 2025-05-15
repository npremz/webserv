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

ParserConfig::ParserConfig() : _file_src("")
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
            {
                ++i;
                c = input[i];
            }
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
        it != file_tokens.end(); ++it)
    {
        if (*it == "server" && *(it + 1) == "{")
        {
            std::vector<std::string>    bloc;
            it += 2;

            while (it + 1 != file_tokens.end() && !(*it == "}" && *(it + 1) == "server"))
            {
                bloc.push_back(*it);
                it++;
            }

            BlocServer serv(bloc);
            this->_servers.push_back(serv);
        }
    }
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
    for (std::vector<BlocServer>::const_iterator it = _servers.begin(); it != _servers.end(); ++it) {
        it->print(indent+2);
    }
}

