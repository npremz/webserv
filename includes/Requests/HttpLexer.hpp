/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armetix <armetix@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/05/20 11:18:20 by armetix          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPLEXER_HPP
# define HTTPLEXER_HPP

# include <string>
# include <map>
# include "../Utils/utils.hpp"

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
	unsigned int	httpver;
	unsigned int	httpsubver;
	std::map<std::string, std::string, CiLess> headers;
	bool	ischunked;
	size_t	expectedoctets;
	size_t	receivedoctets;
	unsigned int	endstatus;
	size_t	headerbytes;
	};
	
	Status		feed(const char *data, size_t len);
	HttpLexer();
	~HttpLexer();
	const parsedRequest &getrequest() const;
	
private:
	ParseState		_parseStartLine();
	std::string 	_buf;
	State 			_state;
	parsedRequest	_req;

};

#endif
