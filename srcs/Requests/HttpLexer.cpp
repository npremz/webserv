/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/05/20 15:54:56 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/HttpLexer.hpp"

HttpLexer::HttpLexer() : _state(START_LINE), _req_size(0)
{}

HttpLexer::~HttpLexer()
{}

HttpLexer::ParseState HttpLexer::_parseStartLine()
{
	std::string::size_type 	pos;
	std::string 			method;
	std::string 			target;
	std::string 			httpv;
	std::string 			ver;
	
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
	else if (method == "HEAD")
		_req.method = HTTP_HEAD;
	else if (method == "POST")
		_req.method = HTTP_POST;
	else if (method == "PUT")
		_req.method = HTTP_PUT;
	else if (method == "DELETE")
		_req.method = HTTP_DELETE;
	else if (method == "CONNECT")
		_req.method = HTTP_CONNECT;
	else if (method == "OPTIONS")
		_req.method = HTTP_OPTIONS;
	else if (method == "TRACE")
		_req.method = HTTP_TRACE;
	else if (method == "PATCH")
		_req.method = HTTP_PATCH;
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
	if (httpv.substr(0, pos + 1) != "HTTP/")
	{
		_req.endstatus = 400;
		return (PARSE_ERROR);
	}
	_req.httpver = httpv.substr(pos + 1);
	return (GOOD);
}

std::vector<std::string>	HttpLexer::_splitHeader(std::string header_block)
{
	std::vector<std::string>	lines;
    size_t						start = 0;

    while (start < header_block.length()) {
        size_t end = header_block.find("\r\n", start);
        size_t sep_len = 2;

        if (end == std::string::npos) {
            lines.push_back(header_block.substr(start));
            break;
        }
        lines.push_back(header_block.substr(start, end - start));
        start = end + sep_len;
    }
    return lines;
}

HttpLexer::ParseState HttpLexer::_parseHeaders()
{	
	std::string::size_type 	pos = _buf.find("\r\n");

	if (pos == std::string::npos)
		return (PAUSE);
	return (GOOD);
	
	std::vector<std::string> header_lines = _splitHeader(_buf);
}

HttpLexer::Status HttpLexer::feed(const char *data, size_t len)
{
	if ((_req_size += len) > MAX_CLIENT_SIZE
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
				_state = DONE;
		default:
			break;
		}
	}
	if (_state == ERROR)
		return (ERR);
	return (COMPLETE);
}
