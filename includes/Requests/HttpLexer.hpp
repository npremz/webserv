/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armetix <armetix@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:03:24 by armetix           #+#    #+#             */
/*   Updated: 2025/05/12 15:13:46 by armetix          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPLEXER_HPP
# define HTTPLEXER_HPP

# include <string>

class HttpLexer
{
private:
	/* data */
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
	std::string _buf;
	State 		_state = START_LINE;
	Status		feed(const char *data, size_t len);
	HttpLexer();
	~HttpLexer();
};

#endif