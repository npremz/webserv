/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BlocLocation.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 23:21:19 by npremont          #+#    #+#             */
/*   Updated: 2025/05/09 23:21:19 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCLOCATION_HPP
# define BLOCLOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <fstream>

# include "../../includes/Logger/Logger.hpp"
# include "../../includes/Utils/Utils.hpp"



class BlocLocation 
{
    private:

    public:
        BlocLocation();
        //BlocLocation(const BlocLocation& other);
        //BlocLocation& operator=(const BlocLocation& other);
        ~BlocLocation();

        void    parseLocation(std::ifstream& file);
};

#endif