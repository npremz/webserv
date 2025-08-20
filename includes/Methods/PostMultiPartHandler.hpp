/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMultiPartHandler.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 12:17:33 by npremont          #+#    #+#             */
/*   Updated: 2025/08/20 17:23:21 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_MULTIPART_HANDLER_HPP
# define POST_MULTIPART_HANDLER_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <fstream>
# include <sstream>
# include <ctime>
# include <cstdlib>

# include "../Requests/HttpLexer.hpp"

struct MultipartPart {
    std::string name;  
    std::string filename;
    std::string contentType;
    std::string content;
    bool isFile;
    
    MultipartPart() : isFile(false) {}
};

class PostMultiPartHandler {
    private:
        std::string                 _boundary;
        std::string                 _uploadDirectory;
        HttpLexer::parsedRequest    _req;
        std::vector<MultipartPart>  _parts;
        
        std::string                         _extractBoundary(const std::string& contentType);
        std::vector<std::string>            _splitByBoundary(const std::string& body);
        MultipartPart                       _parsePart(const std::string& partData);
        std::map<std::string, std::string>  _parseHeaders(const std::string& headerSection);
        std::string                         _parseContentDisposition(const std::string& headerValue, 
                                                const std::string& parameter);
        
        std::string                         _generateSafeFilename(const std::string& originalName);
        bool                                _isValidFileType(const std::string& contentType);

    public:
        PostMultiPartHandler(HttpLexer::parsedRequest req, const std::string& uploadDir);
        ~PostMultiPartHandler();
        
        bool                                parseMultipartData();
        bool                                saveUploadedFiles();
        
        void                                clear();
};

#endif
