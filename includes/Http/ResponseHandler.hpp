/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 14:08:28 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 23:43:56 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP

# include "../Config/BlocServer.hpp"

class ResponseHandler
{
    private:
    public:
        static std::string createResponse(unsigned int code, std::string msg,
            const std::string& bodyStr, std::string content_type = "");
};

#endif
