/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcools <gcools@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 11:54:43 by marvin            #+#    #+#             */
/*   Updated: 2025/05/14 11:54:43 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cgi.hpp"

CGI::CGI(std::string script, std::string query, std::string addr, std::string port,
         std::string name, std::string srv_port)
    : _script(script), _query(query), _addr(addr), _port(port), _server_name(name), _server_port(srv_port) {}

void CGI::execute() {
    std::string interpreter;
    if (_script.substr(_script.find_last_of(".") + 1) == "php") {
        interpreter = "php";
    } else if (_script.substr(_script.find_last_of(".") + 1) == "py") {
        interpreter = "python3";
    } else {
        std::cerr << "Type de script non supporté." << std::endl;
        return;
    }

    setenv("QUERY_STRING", _query.c_str(), 1);
    setenv("REMOTE_ADDR", _addr.c_str(), 1);
    setenv("REMOTE_PORT", _port.c_str(), 1);
    setenv("SERVER_NAME", _server_name.c_str(), 1);
    setenv("SERVER_PORT", _server_port.c_str(), 1);

    char* const argv[] = { const_cast<char*>(interpreter.c_str()), const_cast<char*>(_script.c_str()), NULL };

    pid_t pid = fork();
    if (pid == 0) {
        if (execvp(interpreter.c_str(), argv) == -1) {
            std::cerr << "Erreur lors de l'exécution de " << interpreter << " pour le script " << _script << std::endl;
            exit(1);
        }
    } else if (pid < 0) {
        std::cerr << "Erreur lors du fork" << std::endl;
    } else {
        waitpid(pid, NULL, 0);
    }
}

/* a changer pour faire une fonction qui recup les infos du parsing et qui exec le reste avec 
int main() {
    std::string script = "test.py";  // Parsing
    
    // Définir la requête CGI
    std::string query = "name=test&age=42";
    
    // Parsing 
    std::string addr = "127.0.0.1";
    std::string port = "12345";
    std::string server_name = "localhost";
    std::string server_port = "4242";
    
    CGI cgi(script, query, addr, port, server_name, server_port);
    cgi.execute();
    
    std::cout << "La requête CGI a été exécutée." << std::endl;
    
    return 0;
}
*/
