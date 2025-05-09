/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 11:53:50 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 11:53:50 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "../defines.hpp"
# include <iostream>
# include <unistd.h>

class Logger
{
    private:

    public:
        enum level {
            FATAL = 0,
            INFO,
            DEBUG
        };

        static void log(Logger::level level, std::string msg);

        static void printLog(Logger::level level, std::string msg);
        static void addLevel(Logger::level level);

};

#endif