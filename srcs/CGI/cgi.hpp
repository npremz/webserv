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
        std::string script;
        std::string query;
        std::string addr;
        std::string port;
        std::string server_name;
        std::string server_port;
        
public:
    CGI(std::string script, std::string query, std::string addr, std::string port,
        std::string name, std::string srv_port);
        
    void execute();

};

