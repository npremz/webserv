/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/05/20 13:49:11 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/HttpLexer.hpp"

HttpLexer::HttpLexer() : _state(START_LINE)
{
	_req.receivedoctets = 0;
}

HttpLexer::~HttpLexer()
{}

HttpLexer::ParseState HttpLexer::_parseStartLine()
{
	std::string::size_type pos;
	std::string method;
	std::string target;
	std::string httpv;
	std::string ver;
	
	pos = _buf.find("\r\n");
	if (pos == std::string::npos)
		return (PAUSE);
	std::istringstream iss(_buf.substr(0, pos));
	iss >> method >> target >> httpv;
	if (method.empty() || target.empty() || httpv.empty())
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	if (method == "GET")
		_req.method = HTTP_GET;
	else if (method == "POST")
		_req.method = HTTP_POST;
	else if (method == "DELETE")
		_req.method = HTTP_DELETE;
	else
	{
		_req.method = HTTP_UNKNOWN;
		_req.endstatus = 405;
		return (PARSE_ERROR);
	}
	_req.targetraw = target;
	pos = target.find("?");
	if (pos == std::string::npos)
		_req.path = target;
	else
	{
		_req.path = target.substr(0, pos);
		_req.query = target.substr(pos + 1);
	}
	if (httpv.length() != 8)
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	pos = httpv.find("/");
	if (pos == std::string::npos)
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	if (httpv.substr(0, pos) != "HTTP/")
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	_req.httpver = httpv.substr(pos + 1);
	return (GOOD);
}

HttpLexer::ParseState HttpLexer::_parseHeaders()
{
	return (GOOD);
}

HttpLexer::Status HttpLexer::feed(const char *data, size_t len)
{
	if ((_req.receivedoctets += len) > MAX_CLIENT_SIZE
		|| _buf.size() + len > MAX_CLIENT_SIZE)
	{	
		_state = ERROR;
		_req.endstatus = 413;
		Logger::log(Logger::ERROR, "Max client size exceeded.");
	}
	_buf.append(data, len);
	while (_state != DONE && _state != ERROR)
	{
		switch (_state)
		{
		case START_LINE:
			if (_parseStartLine() == GOOD)
				_state = HEADERS;
			break;
		case HEADERS:
			if (_parseHeaders() == GOOD)
				_state = BODY;
		default:
			break;
		}
	}
	if (_state == ERROR)
		return (ERR);
	return (COMPLETE);
}
