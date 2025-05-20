/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcools <gcools@student.42.fr               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 12:18:49 by marvin            #+#    #+#             */
/*   Updated: 2025/05/14 12:18:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class CGI {
private:
        std::string _script;
        std::string _query;
        std::string _addr;
        std::string _port;
        std::string _server_name;
        std::string _server_port;
        
public:
    CGI(std::string script, std::string query, std::string addr, std::string port,
        std::string name, std::string srv_port);
        
    void execute();

};

