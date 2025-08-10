/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 14:11:47 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 21:48:37 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Http/ResponseHandler.hpp"

std::string ResponseHandler::createResponse(unsigned int code, std::string msg,
    const std::string& bodyStr, std::string content_type)
{
    std::ostringstream oss_header;
    oss_header << "HTTP/1.1 " << code << " " << msg << "\r\n";
    if (!content_type.empty())
        oss_header << "Content-Type: " << content_type << "\r\n";
    oss_header << "Content-Length: " << bodyStr.size() << "\r\n";
    oss_header << "\r\n"; 

    std::string response = oss_header.str();
    response.append(bodyStr.data(), bodyStr.size());

    return (response);
}
