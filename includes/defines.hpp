/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 12:06:12 by npremont          #+#    #+#             */
/*   Updated: 2025/05/08 12:06:12 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINE_HPP
# define DEFINE_HPP

# define MAX_BUF_SIZE 65536
# define C_BLACK   "\033[30m"
# define C_RED     "\033[31m"
# define C_GREEN   "\033[32m"
# define C_YELLOW  "\033[33m"
# define C_BLUE    "\033[34m"
# define C_MAGENTA "\033[35m"
# define C_CYAN    "\033[36m"
# define C_WHITE   "\033[37m"
# define C_GRAY    "\033[90m"
# define C_LIGHT_GRAY "\033[37m"
# define C_BEIGE   "\033[93m"
# define C_RESET   "\033[0m"

# define MAX_CLIENT_SIZE 67108864
# define MAX_CHUNK_SIZE 8192
# define EPOLL_TIMEOUT 1000 //miliseconds
# define EPOLL_MAX_EVENTS 128
# define REQUEST_TIMEOUT 10 //seconds
# define MAX_PORT 65536
# define LISTEN_QUEUE 128
# define MAX_STARTLINE_SIZE 2048
# define MAX_HEADERS 1024

# define DEBUG_MODE 1

#endif
