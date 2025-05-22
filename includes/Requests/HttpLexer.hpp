/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/05/22 12:30:40 by npremont         ###   ########.fr       */
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
		typedef std::multimap<std::string, std::string, CiLess> HeaderMultiMap;

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
			HeaderMap			non_duplicable_headers;
			HeaderMultiMap		duplicable_headers;
			bool				ischunked;
			size_t				expectedoctets;
			size_t				receivedoctets;
			unsigned int		endstatus;
			size_t				headerbytes;
		};

	private:
		std::string 				_buf;
		State 						_state;
		parsedRequest				_req;
		size_t						_req_size;
		
		ParseState					_handleStatusError(unsigned int endstatus, ParseState state);
		ParseState					_parseStartLine();
		ParseState					_parseHeaders();

		std::vector<std::string>	_splitHeader(std::string _buf);
		bool 						_isNonDuplicableHeader(const std::string& key);


	public:
		
		HttpLexer();
		~HttpLexer();
		
		Status				feed(const char *data, size_t len);
		const parsedRequest &getrequest() const;

};

#endif
