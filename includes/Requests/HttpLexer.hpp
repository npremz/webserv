/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/05/20 13:45:46 by npremont         ###   ########.fr       */
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

		enum HttpMethod {
			HTTP_GET,
			HTTP_POST,
			HTTP_DELETE,
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
			HttpMethod	method;
			std::string	targetraw;
			std::string	path;
			std::string query;
			std::string httpver;
			std::map<std::string, std::string, CiLess> headers;
			bool	ischunked;
			size_t	expectedoctets;
			size_t	receivedoctets;
			unsigned int	endstatus;
			size_t	headerbytes;
		};
		
		HttpLexer();
		~HttpLexer();
		
		Status				feed(const char *data, size_t len);
		const parsedRequest &getrequest() const;
		
	private:
		ParseState		_parseStartLine();
		ParseState		_parseHeaders();
		std::string 	_buf;
		State 			_state;
		parsedRequest	_req;

};

#endif
