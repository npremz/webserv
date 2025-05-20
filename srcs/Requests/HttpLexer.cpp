/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armetix <armetix@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/05/19 16:36:39 by armetix          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/HttpLexer.hpp"
#include "../../includes/defines.hpp"

HttpLexer::HttpLexer() : _buf(), _state(START_LINE), _req()
{
	
}

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
	ver = httpv.substr(pos + 1);
	pos = ver.find(".");
	if (pos == std::string::npos)
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	
	
}

HttpLexer::Status HttpLexer::feed(const char *data, size_t len)
{
	if (_buf.size() + len > MAX_BUF_SIZE)
	{
		_state = ERROR;
		_req.endstatus = 413;
	}
	_buf.append(data, len);
	while (_state != DONE && _state != ERROR)
	{
		switch (_state)
		{
		case START_LINE:
			_parseStartLine();
			break;
		
		default:
			break;
		}
	}
	if (_state == ERROR)
		return (ERR);
}