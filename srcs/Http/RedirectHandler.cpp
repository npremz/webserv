/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 15:53:55 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 15:55:17 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Http/RedirectHandler.hpp"

std::string RedirectHandler::createRedirect(unsigned int code, const std::string& url)
{
    std::string status_msg;

    if (code == 300)
        status_msg = "Multiple Choices";
    else if (code == 301)
        status_msg = "Moved Permanently";
    else if (code == 302)
        status_msg = "Found";
    else if (code == 303)
        status_msg = "See Other";
    else if (code == 305)
        status_msg = "Use Proxy";
    else if (code == 306)
        status_msg = "(Unused)";
    else if (code == 307)
        status_msg = "Temporary Redirect";

    std::ostringstream oss_body;
    oss_body << "<html><head><title>" << code << status_msg << "</title></head><body><h1>"
             << code << " " << status_msg
             << "</h1><p>The document has moved <a href=" 
             << url
             << ">here</a>."
             << "</p></body></html>"
             << "\r\n\r\n";

    std::ostringstream oss_header;
    oss_header << "HTTP/1.1 " << code << " " << status_msg << "\r\n"
               << "Location: " << url << "\r\n"
               << "Content-Type: text/html\r\n"
               << "Content-Length: " << oss_body.str().size() << "\r\n"
               << "\r\n";
    
    std::string response = oss_header.str() + oss_body.str();
    return (response);
}