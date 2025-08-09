/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/08/03 13:44:17 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPLEXER_HPP
# define HTTPLEXER_HPP

# include <string>
# include <map>

# include "../Utils/utils.hpp"
# include "../../includes/defines.hpp"
# include "../Logger/Logger.hpp"

class HttpLexer
{ 
	public:
		typedef std::map<std::string, std::string, CiLess> HeaderMap;

		enum HttpMethod {
			HTTP_GET,
			HTTP_HEAD,
			HTTP_POST,
			HTTP_PUT,
			HTTP_DELETE,
			HTTP_CONNECT,
			HTTP_OPTIONS,
			HTTP_TRACE,
			HTTP_PATCH,
			HTTP_UNKNOWN
		};

		enum State {
			START_LINE,
			HEADERS,
			BODY,
			BODY_CHUNK_SIZE,
			BODY_CHUNK_DATA,
			DONE,
			ERROR
		};
		
		enum Status {
			NEED_MORE,
			COMPLETE,
			ERR	
		};
		
		enum ParseState {
			GOOD,
			PAUSE,
			PARSE_ERROR	
		};
		
		struct parsedRequest {
			HttpMethod			method;
			std::string			targetraw;
			std::string			path;
			std::string 		query;
			std::string 		httpver;
			std::string			errorMsg;
			HeaderMap			headers;
			bool				ischunked;
			size_t				expectedoctets;
			size_t				receivedoctets;
			unsigned int		endstatus;
            std::string         contentType;
			std::string			body;
			size_t				headerbytes;
			bool				has_host;
			std::string			host;
			s_ip_port			ip_port;
		};

	private:
		std::string 				_buf;
		State 						_state;
		parsedRequest				_req;
		size_t						_req_size;
		
		ParseState					_handleStatusError(unsigned int endstatus, std::string errorStr, ParseState state);
		ParseState					_parseStartLine();
		ParseState					_parseHeaders();
		ParseState					_parseBody();
		ParseState					_bodyParseChunked();
		ParseState					_bodyParseCL();
		
		std::vector<std::string>	_splitHeader(std::string _buf);
		bool 						_isNonDuplicableHeader(const std::string& key);
        bool        				_isValidHostValue(const std::string& val);
		bool						_isValidContentLengthValue(const std::string& val);
		
	public:
		std::string 				server_root;

		HttpLexer(u_int32_t ip);
		~HttpLexer();
		
		Status				feed(const char *data, size_t len);
		void				setEndStatus(size_t status); 
		const parsedRequest &getRequest() const;

};

#endif
