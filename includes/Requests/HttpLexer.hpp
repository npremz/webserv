/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armetix <armetix@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/05/14 13:39:09 by armetix          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPLEXER_HPP
# define HTTPLEXER_HPP

# include <string>
# include <map>
# include "../Utils/utils.hpp"

typedef struct s_parsedRequest
{
	HttpLexer::HttpMethod	method;
	std::string	targetraw;
	std::string	path;
	std::string query;
	unsigned int	httpversion;
	std::map<std::string, std::string, CiLess> headers;
	
} t_parsedRequest;


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
		DONE,
		ERR	
	};
	
	Status		feed(const char *data, size_t len);
	HttpLexer();
	~HttpLexer();
	
private:
	std::string _buf;
	State 		_state;

};

#endif