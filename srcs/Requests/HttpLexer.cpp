/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: npremont <npremont@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:39:27 by armetix           #+#    #+#             */
/*   Updated: 2025/08/25 13:10:28 by npremont         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/Requests/HttpLexer.hpp"

HttpLexer::HttpLexer(u_int32_t ip) : _state(START_LINE)
{
	_req.endstatus = 200;
	_req.has_host = false;
	_req.ischunked = false;
	_req.expectedoctets = 0;
	_req.receivedoctets = 0;
	_req.ip_port.ip = ip;
	_req.ip_port.port = 0;
	_req.received_expected_100 = false;
}

HttpLexer::~HttpLexer()
{}

HttpLexer::ParseState HttpLexer::_handleStatusError(unsigned int endstatus,
	std::string errorStr, ParseState state)
{
	_req.endstatus = endstatus;
	_req.errorMsg = errorStr;
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
	std::string				bin;
	
	end = _buf.find("\r\n");
	if (end == std::string::npos)
		return (PAUSE);
	pos = end;

	std::istringstream iss(_buf.substr(0, pos));
	Logger::log(Logger::INFO, iss.str());
	iss >> method >> target >> httpv >> bin;
	if (method.empty() || target.empty() || httpv.empty() || !bin.empty())
	{
		if (bin.empty())
			return (_handleStatusError(400,
				"Malformed request line: missing method, URI or HTTP version.",
				PARSE_ERROR));
		else
			return (_handleStatusError(400,
				"Malformed request line: too many elements (expected: METHOD URI VERSION).",
				PARSE_ERROR));
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
		return (_handleStatusError(501,
			"The server does not support the functionality required to fulfill the request (unknown HTTP method).",
			PARSE_ERROR));
	}

	if (target.size() > MAX_STARTLINE_SIZE)
		return (_handleStatusError(414,
			"URI too long",
			PARSE_ERROR));
    if (target[0] != '/')
            return (_handleStatusError(400, "Illegal URI", PARSE_ERROR));

	if (hasIllegalURIChars(target))
		return (_handleStatusError(400, "Illegal chars in URI", PARSE_ERROR));

	_req.targetraw = target;
	pos = target.find("?");
	if (pos == std::string::npos)
		_req.path = target;
	else
	{
		_req.path = target.substr(0, pos);
		_req.query = target.substr(pos + 1);
	}

	for (std::string::iterator it = _req.path.begin(); it != _req.path.end(); )
	{
		if (*it == '/' && ((it + 1) != _req.path.end() && (*(it + 1) == '/')))
			_req.path.erase(it);
		else
			it++;
	}

	if (httpv.length() != 8)
		return (_handleStatusError(400,
			"Malformed request line: invalid HTTP version (expected: HTTP/1.1).",
			PARSE_ERROR));
	pos = httpv.find("/");
	if (pos == std::string::npos)
		return (_handleStatusError(400,
			"Malformed request line: invalid HTTP version (expected: HTTP/1.1).",
			PARSE_ERROR));
	if (httpv.substr(0, pos + 1) != "HTTP/")
		return (_handleStatusError(400,
			"Malformed request line: invalid HTTP version (expected: HTTP/1.1).",
			PARSE_ERROR));
	_req.httpver = httpv.substr(pos + 1);
	if (_req.httpver != "1.1")
		return (_handleStatusError(400,
			"Malformed request line: invalid HTTP version (expected: HTTP/1.1).",
			PARSE_ERROR));
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
	std::string lower_key = toLowercase(key);
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

	if (val.empty()) return false;

    for (std::string::size_type i = 0; i < val.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(val[i]);
        if (c <= 31 || c == 127 || c == ' ') return false;
        if (std::isalnum(c)) continue;
        if (c == '-' || c == '.' || c == ':' || c == '[' || c == ']') continue;
        
		return false;
    }

	size_t		dual_dots_pos = val.find(':');
	if (dual_dots_pos == std::string::npos)
		return (false);
	_req.host = val.substr(0, dual_dots_pos); 

	std::string port_str = val.substr(dual_dots_pos + 1);
	if (!isNumeric(port_str))
		return (false);
	std::istringstream  iss(port_str);
	unsigned int        port;
	if (iss >> port && port < MAX_PORT)
		_req.ip_port.port = port;
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

	if (header_lines.size() > MAX_HEADERS)
		return (_handleStatusError(400, "Headers limit exceeded",
				PARSE_ERROR));

	for (std::vector<std::string>::iterator it = header_lines.begin(); 
		it != header_lines.end(); ++it)
	{
		if ((*it).empty())
            continue;

		size_t		delim = (*it).find(':');
		if (delim == std::string::npos)
		{
			if (countWords(*it) > 1)
				return (_handleStatusError(400,
					"Malformed header: missing ':' delimiter.", PARSE_ERROR));
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
		
		Logger::log(Logger::DEBUG, "headerVal size: ");
		if (DEBUG_MODE)
			std::cout << val.size() << std::endl;

		if (val.size() > MAX_HEADER_VALUE_SIZE)
			return (_handleStatusError(431,
					"Request Header Fields Too Large",
					PARSE_ERROR));

		if (!isValidHeaderName(key))
			return (_handleStatusError(400,
					"Illegal char in header key",
					PARSE_ERROR));

		if (headerValHasIllegalChars(val))
			return (_handleStatusError(400,
					"Illegal char in header value",
					PARSE_ERROR));


		if (toLowercase(key) == "host")
		{
			if (!_isValidHostValue(val))
				return (_handleStatusError(400, "Invalid Host header.", PARSE_ERROR));
			_req.has_host = true;
			
			Logger::log(Logger::DEBUG, "Host detected: " + _req.host);
			Logger::log(Logger::DEBUG, "Request ip: " + ipPortToString(_req.ip_port));
		}
		else if (toLowercase(key) == "content-length")
		{
			if (!_isValidContentLengthValue(val))
				return (_handleStatusError(400,
					"Invalid Content-Length header: must be a valid integer.",
					PARSE_ERROR));
			Logger::log(Logger::DEBUG, "Content-length detected: " + val);
			if (_req.expectedoctets > MAX_CLIENT_SIZE)
				return (_handleStatusError(413,
					"The request is larger than the server is willing or able to process.",
					PARSE_ERROR));
		}
		else if (toLowercase(key) == "transfer-encoding")
		{
			if (val == "chunked")
				_req.ischunked = true;
			Logger::log(Logger::DEBUG, "transfer-encoding detected: " + val);
		}
        else if (toLowercase(key) == "content-type")
		{
			_req.contentType = val;
			Logger::log(Logger::DEBUG, "content-type detected: " + val);
		}
		else if (toLowercase(key) == "expect")
		{
			Logger::log(Logger::DEBUG, "Expect detected: " + val);
			if (val == "100-continue"){
				_req.received_expected_100 = true;
			}
		}

		HeaderMap::iterator key_in_map;
		if ((key_in_map = _req.headers.find(key)) == _req.headers.end())
		{
			_req.headers[key] = val;
		}
		else
		{
			if (_isNonDuplicableHeader(key_in_map->first))
				return (_handleStatusError(400,
					"Duplicate header detected: " + key_in_map->first,
					PARSE_ERROR));
			else
			{
				_req.headers[key_in_map->first] += "," + val; 
			}
		}
	}
	_buf.erase(0, pos + 4);
	return (GOOD);
}

HttpLexer::ParseState HttpLexer::_bodyParseCL()
{
	_req.receivedoctets = _buf.size();
	if (_buf.size() < _req.expectedoctets)
		return (PAUSE);
	else if (_buf.size() >= _req.expectedoctets)
	{
		_req.body.append(_buf, 0, _req.expectedoctets);
		_buf.erase(0, _req.expectedoctets);
		return (GOOD);
	}
	return (_handleStatusError(500, "Body parsing failed.", PARSE_ERROR));
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
		std::string chunk_size_line = _buf.substr(0, pos);
		
		if (!isValidHexString(chunk_size_line))
			return (_handleStatusError(400, "Illegal chars in chunk Size", PARSE_ERROR));
			
    	ss << chunk_size_line;
		ss >> std::hex >> chunk_size;
		if (ss.fail())
			return (_handleStatusError(400, "Invalid Chunk Size", PARSE_ERROR));

		if (chunk_size < 0)
			return (_handleStatusError(400, "Negative chunk Size", PARSE_ERROR));

		if (chunk_size == 0)
		{
			if (_buf.size() < pos + 4)
				return (PAUSE);
			if (_buf.substr(pos + 2, 2) != "\r\n")
        		return (_handleStatusError(400, "Trailer headers not supported", PARSE_ERROR));
			
			_buf.erase(0, pos + 4);
			if (!_buf.empty())
        		return (_handleStatusError(400, "Data after final chunk", PARSE_ERROR));
			return (GOOD);
		}

		if (_buf.size() < ((pos + 2) + chunk_size + 2))
			return (PAUSE);

		_req.body.append(_buf, pos + 2, chunk_size);
		_req.receivedoctets = _req.body.size();
		if (_req.receivedoctets > MAX_CLIENT_SIZE)
			return (_handleStatusError(413, "Content Too Large", PARSE_ERROR));
			
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
	if (_buf.size() + len - 4 > MAX_CLIENT_SIZE)
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
				{
					_req.endstatus = 400;
					_state = ERROR;
				}

				else if (_req.expectedoctets > 0 || _req.ischunked)
					_state = BODY;
				else
					_state = DONE;
				Logger::log(Logger::DEBUG, "Header parsing done");
				if (_req.received_expected_100
					&& (_req.expectedoctets > 0 || _req.ischunked))
				{
					Logger::log(Logger::DEBUG, "Reveived 'Expect: 100-continue'");
					return (MUST_CHECK);
				}
			}
			break;
		case BODY:
			Logger::log(Logger::DEBUG, "Body parsing...");
			parsing_state = _parseBody();
			if (parsing_state == GOOD)
			{
				Logger::log(Logger::DEBUG, "Body parsed: " + _req.body);
				Logger::log(Logger::DEBUG, "received octets");
				if (DEBUG_MODE)
					std::cout << "\r" << _req.receivedoctets << std::endl;
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

void HttpLexer::setEndStatus(size_t status)
{
	_req.endstatus = status;
}

size_t	HttpLexer::getEndStatus() const
{
	return (this->_req.endstatus);
}
