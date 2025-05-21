/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/05/21 10:42:58 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/HttpLexer.hpp"

HttpLexer::HttpLexer() : _state(START_LINE), _req_size(0)
{}

HttpLexer::~HttpLexer()
{}

HttpLexer::ParseState HttpLexer::_handleStatusError(unsigned int endstatus, ParseState state)
{
	_req.endstatus = endstatus;
	return (state);
}

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
	_req_size += pos + 2;
	if (_req_size > MAX_CLIENT_SIZE)
	{

	}
	std::istringstream iss(_buf.substr(0, pos));
	iss >> method >> target >> httpv;
	if (method.empty() || target.empty() || httpv.empty())
		_handleStatusError(400, PARSE_ERROR);
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
		_handleStatusError(405, PARSE_ERROR);
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
		_handleStatusError(400, PARSE_ERROR);
	pos = httpv.find("/");
	if (pos == std::string::npos)
		_handleStatusError(400, PARSE_ERROR);
	if (httpv.substr(0, pos + 1) != "HTTP/")
		_handleStatusError(400, PARSE_ERROR);
	_req.httpver = httpv.substr(pos + 1);
	_buf.erase(0, _req_size);
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

bool	HttpLexer::_isNonDuplicableHeader(const std::string& key) {
    return key == "Host" ||
           key == "Content-Length" ||
           key == "Content-Type" ||
           key == "Content-Encoding" ||
           key == "Content-Range" ||
           key == "User-Agent" ||
           key == "Server";
}

HttpLexer::ParseState HttpLexer::_parseHeaders()
{	
	std::string::size_type 	pos = _buf.find("\r\n");

	if (pos == std::string::npos)
		return (PAUSE);
	
	std::vector<std::string> header_lines = _splitHeader(_buf);

	for (std::vector<std::string>::iterator it = header_lines.begin(); 
		it != header_lines.end(); ++it)
	{
		size_t		delim = (*it).find(':');
		std::string key = (*it).substr(0, delim);
		std::string val = (*it).substr(delim + 1);

		while (!(*it).empty() && ((*it)[0] == ' ' || (*it)[0] == '\r'))
            (*it).erase(0, 1);
		
		if (_isNonDuplicableHeader(key))
		{
			if (_req.non_duplicable_headers.find(key) != _req.non_duplicable_headers.end())
				Logger::log(Logger::ERROR, "Invalid request. => unautorized duplicable header.");
			_req.non_duplicable_headers[key] = val;
		}
		else
			_req.duplicable_headers.insert(std::make_pair(key, val));
	}

	return (GOOD);
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
