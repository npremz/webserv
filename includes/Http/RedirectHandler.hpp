/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 15:46:14 by npremont          #+#    #+#             */
/*   Updated: 2025/08/10 15:58:23 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIRECTHANDLER_HPP
# define REDIRECTHANDLER_HPP

# include <iostream>
# include <sstream>

class RedirectHandler
{
    private:
    public:
        static std::string createRedirect(unsigned int code, const std::string& url);
};

#endif