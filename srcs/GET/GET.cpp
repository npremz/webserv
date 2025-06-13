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

std::string generateAutoIndex(const std::string& fullPath, const std::string& uriPath) {
    std::ostringstream html;
    html << "<html><head><title>Index of " << uriPath << "</title></head><body>";
    html << "<h1>Index of " << uriPath << "</h1><ul>";

    DIR* dir = opendir(fullPath.c_str());
    if (!dir) return "<h1>403 Forbidden</h1>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        html << "<li><a href=\"" << uriPath << "/" << entry->d_name << "\">"
             << entry->d_name << "</a></li>";
    }
    closedir(dir);

    html << "</ul></body></html>";
    return html.str();

}

std::string handleGET(const std::string& uriPath, const std::string& root, bool autoindex) {
    std::string fullPath = root + uriPath;

    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) == 0) {
        if (S_ISDIR(pathStat.st_mode)) {
            if (*(fullPath.end()) != '/')
                fullPath += "/";

            std::string indexPath = fullPath + "index.html";
            std::ifstream indexFile(indexPath.c_str());
            if (indexFile.is_open()) {
                std::ostringstream ss;
                ss << indexFile.rdbuf();
                return ss.str();
            } else if (autoindex) {
                return generateAutoIndex(fullPath, uriPath);
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

/*
int main() {
    std::string root = "./www";        // Dossier racine du serveur
    bool autoindex = true;             // Activer l'autoindex si pas d'index.html
    
    // 1. Fichier existant
    std::cout << "----- Test : fichier existant -----\n";
    std::cout << handleGET("/test.html", root, autoindex) << "\n\n";
    
    // 2. Répertoire avec index.html
    std::cout << "----- Test : dossier avec index.html -----\n";
    std::cout << handleGET("/folder_with_index/", root, autoindex) << "\n\n";
    
    // 3. Répertoire sans index.html avec autoindex activé
    std::cout << "----- Test : dossier sans index.html avec autoindex ON -----\n";
    std::cout << handleGET("/folder_no_index/", root, true) << "\n\n";
    
    // 4. Répertoire sans index.html avec autoindex désactivé
    std::cout << "----- Test : dossier sans index.html avec autoindex OFF -----\n";
    std::cout << handleGET("/folder_no_index/", root, false) << "\n\n";
    
    // 5. Fichier inexistant
    std::cout << "----- Test : fichier inexistant -----\n";
    std::cout << handleGET("/does_not_exist.txt", root, autoindex) << "\n\n";
    
    return 0;
}
*/
