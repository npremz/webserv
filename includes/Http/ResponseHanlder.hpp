/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHanlder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 14:08:28 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 14:15:18 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "../Config/BlocServer.hpp"

class ResponseHandler
{
    public:
        ResponseHandler();
        ~ResponseHandler();

        std::string createResponse(unsigned int code, std::string msg,
            const std::string& bodyStr, std::string content_type);
};

#endif