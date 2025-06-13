/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/06/13 10:04:45 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Requests/HttpLexer.hpp"

HttpLexer::HttpLexer() : _state(START_LINE), _req_size(0)
{
	_req.has_host = false;
}

HttpLexer::~HttpLexer()
{}

HttpLexer::ParseState HttpLexer::_handleStatusError(unsigned int endstatus, ParseState state)
{
	_req.endstatus = endstatus;
	_state = ERROR;
	return (state);
}

HttpLexer::ParseState HttpLexer::_parseStartLine()
{
	std::string::size_type 	pos;
	std::string::size_type 	end;
	std::string 			method;
	std::string 			target;
	std::string 			httpv;
	std::string 			ver;
	
	end = _buf.find("\r\n");
	if (end == std::string::npos)
		return (PAUSE);
	pos = end;

	std::istringstream iss(_buf.substr(0, pos));
	iss >> method >> target >> httpv;
	if (method.empty() || target.empty() || httpv.empty())
		return (_handleStatusError(400, PARSE_ERROR));
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
		return (_handleStatusError(405, PARSE_ERROR));
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
		return (_handleStatusError(400, PARSE_ERROR));
	pos = httpv.find("/");
	if (pos == std::string::npos)
		return (_handleStatusError(400, PARSE_ERROR));
	if (httpv.substr(0, pos + 1) != "HTTP/")
		return (_handleStatusError(400, PARSE_ERROR));
	_req.httpver = httpv.substr(pos + 1);
	_buf.erase(0, end);
	Logger::log(Logger::DEBUG, std::string(_req.method == HTTP_GET ? "GET" : "") + std::string(_req.method == HTTP_POST ? "POST" : "") + std::string(_req.method == HTTP_DELETE ? "DELETE" : "") + " " + _req.targetraw + " " + _req.httpver);
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
	std::string lower_key = to_lowercase(key);
    return lower_key == "host" ||
           lower_key == "content-length" ||
           lower_key == "content-type" ||
           lower_key == "content-encoding" ||
           lower_key == "content-range" ||
           lower_key == "user-agent" ||
           lower_key == "server";
}

bool        HttpLexer::_isValidHostValue(const std::string& val)
{
	size_t		dual_dots_pos = val.find(':');
	std::string ip_str = val.substr(0, dual_dots_pos);
        
	unsigned int ip = ipStringToInt(ip_str);
	if (ip == 4294967295)
		return (false);

	s_ip_port ip_port;
	ip_port.ip = ip;

	std::string port_str = val.substr(dual_dots_pos + 1);\
	if (!isNumeric(port_str))
		return (false);
	std::istringstream  iss(port_str);
	unsigned int        port;
	if (iss >> port && port < 65536)
	{
		ip_port.port = port;
		_req.host = ip_port;
	}
	else
		return (false);
	return (true);
}

bool	HttpLexer::_isValidContentLengthValue(const std::string& val)
{
	if (!isNumeric(val))
		return false;
	std::istringstream 	iss(val);
	size_t 				len;
	
	iss >> len;
	_req.expectedoctets = len;
	return (true);
}

HttpLexer::ParseState HttpLexer::_parseHeaders()
{	
	std::string::size_type 	pos = _buf.find("\r\n\r\n");

	if (pos == std::string::npos)
		return (PAUSE);

	std::vector<std::string> header_lines = _splitHeader(_buf.substr(0, pos));

	for (std::vector<std::string>::iterator it = header_lines.begin(); 
		it != header_lines.end(); ++it)
	{
		if ((*it).empty())
            continue;

		size_t		delim = (*it).find(':');
		if (delim == std::string::npos)
		{
			if (countWords(*it) > 1)
				return (_handleStatusError(400, PARSE_ERROR));
            continue;
		}

		std::string key = (*it).substr(0, delim);
		std::string val = (*it).substr(delim + 1);

		while (!key.empty() && (key[0] == ' ' || key[0] == '\t'))
            key.erase(0, 1);
        while (!key.empty() && (key[key.length() - 1] == ' ' || key[key.length() - 1] == '\t'))
            key.erase(key.length() - 1, 1);
        while (!val.empty() && (val[0] == ' ' || val[0] == '\t'))
            val.erase(0, 1);
        while (!val.empty() && (val[val.length() - 1] == ' ' || val[val.length() - 1] == '\t'))
            val.erase(val.length() - 1, 1);

        if (key.empty())
            continue;
			
    	Logger::log(Logger::DEBUG, "<" + key + ">:<" + val + ">");

		if (to_lowercase(key) == "host")
		{
			if (!_isValidHostValue(val))
				return (_handleStatusError(400, PARSE_ERROR));
			_req.has_host = true;
			Logger::log(Logger::DEBUG, "Host detected: " + ipPortToString(_req.host));
		}
		else if (to_lowercase(key) == "content-length")
		{
			if (!_isValidContentLengthValue(val))
				return (_handleStatusError(400, PARSE_ERROR));
			Logger::log(Logger::DEBUG, "Content-length detected: " + val);
		}
		else if (to_lowercase(key) == "transfer-encoding")
		{
			if (val == "chunked")
				_req.ischunked = true;
			Logger::log(Logger::DEBUG, "transfer-encoding detected: " + val);
		}
		else
		{
			HeaderMap::iterator key_in_map;
			
			if ((key_in_map = _req.headers.find(key)) == _req.headers.end())
			{
				_req.headers[key] = val;
			}
			else
			{
				if (_isNonDuplicableHeader(key_in_map->first))
					return (_handleStatusError(400, PARSE_ERROR));
				else
				{
					_req.headers[key_in_map->first] += "," + val; 
				}
			}
		}
	}
	_buf.erase(0, pos + 4);
	return (GOOD);
}

HttpLexer::ParseState HttpLexer::_bodyParseCL()
{
	if (_buf.size() < _req.content_lenght)
		return (PAUSE);
	else if (_buf.size() >= _req.content_lenght)
	{
		_req.body.append(_buf, 0, _req.content_lenght);
		_buf.erase(0, _req.content_lenght);
		return (GOOD);
	}
}

HttpLexer::ParseState HttpLexer::_bodyParseChunked()
{
	int chunk_size;
	std::string::size_type 	pos;

	while (true)
	{
		std::stringstream ss;
		pos = _buf.find("\r\n");
		if (pos == std::string::npos)
			return (PAUSE);
    	ss << _buf.substr(0, pos);
		ss >> std::hex >> chunk_size;
		if (ss.fail())
			return (_handleStatusError(400, PARSE_ERROR));
		if (chunk_size == 0)
		{
			if (_buf.size() < pos + 4)
				return (PAUSE);
			_buf.erase(0, pos + 4);
			return (GOOD);
		}
		if (_buf.size() < ((pos + 2) + chunk_size + 2))
			return (PAUSE);
		_req.body.append(_buf, pos + 2, chunk_size);
		_buf.erase(0, ((pos + 2) + chunk_size + 2));
	}
}

HttpLexer::ParseState HttpLexer::_parseBody()
{
	if (_req.ischunked)
		return (_bodyParseChunked());
	else
		return (_bodyParseCL());

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
	ParseState	parsing_state;
	while (_state != DONE && _state != ERROR)
	{
		switch (_state)
		{
		case START_LINE:
        	Logger::log(Logger::DEBUG, "Startline parsing...");
			parsing_state = _parseStartLine();
			if (parsing_state == GOOD)
			{
				_state = HEADERS;
        		Logger::log(Logger::DEBUG, "Startline parsing done.");
			}
			break;
		case HEADERS:
        	Logger::log(Logger::DEBUG, "Header parsing...");
			parsing_state = _parseHeaders();
			if (parsing_state == GOOD)
			{
				if (!_req.has_host)
					_state = ERROR;
				else
					_state = DONE;
        		Logger::log(Logger::DEBUG, "Header parsing done");
			}
			break;
		case BODY:
			Logger::log(Logger::DEBUG, "Body parsing...");
			parsing_state = _parseBody();
			if (parsing_state == GOOD)
			{
				_state = DONE;
        		Logger::log(Logger::DEBUG, "Body parsing done");
			}
			break;
		default:
			break;
		}
		if (parsing_state == PAUSE)
			return (NEED_MORE);
	}
	if (_state == ERROR)
		return (ERR);
	return (COMPLETE);
}

const HttpLexer::parsedRequest& HttpLexer::getRequest() const
{
	return (_req);
}
