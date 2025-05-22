/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GET.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcools <gcools@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 16:44:13 by gcools            #+#    #+#             */
/*   Updated: 2025/05/22 16:44:13 by gcools           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GET.hpp"

bool isDirectory(const std::string& path) {
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode));
}

std::string generateAutoIndex() { // TO DO

}

std::string handleGET(const std::string& uriPath, const std::string& root, bool autoindex) {
    std::string fullPath = root + uriPath;

    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) == 0) {
        if (S_ISDIR(pathStat.st_mode)) {
            if (fullPath.back() != '/')
                fullPath += "/";

            std::string indexPath = fullPath + "index.html";
            std::ifstream indexFile(indexPath.c_str());
            if (indexFile.is_open()) {
                std::ostringstream ss;
                ss << indexFile.rdbuf();
                return ss.str();
            } else if (autoindex) {
                return generateAutoIndex(); // TO DO
            } else {
                return "<h1>403 Forbidden</h1>";
            }
        } else {
            std::ifstream file(fullPath.c_str(), std::ios::binary);
            if (!file.is_open())
                return "<h1>403 Forbidden</h1>";
            std::ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
    }
    return "<h1>404 Not Found</h1>";
}

