/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:17:27 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/12/17 12:40:12 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequestParser.hpp"

//utils
std::string trim(const std::string &str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
        start++;

    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
        end--;

    return str.substr(start, end - start);
}

//Public
RequestParser::RequestParser() : _state(PARSING_REQUEST_LINE), _contentLen(0)
{
}

RequestParser::~RequestParser() {}

void	RequestParser::feedData(const std::string& data)
{
	_buffer += data;
	parseBuffer();
}

void	RequestParser::parseBuffer()
{
	bool progress = true;
	
	while (_state != COMPLETE && _state != ERROR && !_buffer.empty() && progress)
	{
		size_t prevSize = _buffer.size();

		if(_state == PARSING_REQUEST_LINE)
			RequestLineBuffer();
		else if (_state == PARSING_HEADERS)
			HeaderBuffer();
		else if (_state == PARSING_BODY)
			BodyBuffer();

		progress = (_buffer.size() != prevSize);
	}
}

void	RequestParser::reset_parse()
{
	_state = PARSING_REQUEST_LINE;
	_buffer.clear();
	_request.clear();
	_contentLen = 0;
	_error.clear();
}

//debugging
void	RequestParser::printState() const
{
	std::cout << "\n=== RequestParser State ===" << std::endl;
    std::cout << "State: ";
    switch(_state) {
        case PARSING_REQUEST_LINE: 
            std::cout << "PARSING_REQUEST_LINE"; break;
        case PARSING_HEADERS: 
            std::cout << "PARSING_HEADERS"; break;
        case PARSING_BODY: 
            std::cout << "PARSING_BODY"; break;
        case COMPLETE: 
            std::cout << "COMPLETE"; break;
        case ERROR: 
            std::cout << "ERROR"; break;
    }
    std::cout << std::endl;
    std::cout << "Buffer size: " << _buffer.length() << " bytes" << std::endl;
    if (_state == ERROR) {
        std::cout << "Error: " << _error << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

bool	RequestParser::isComplete() const
{
	return _state == COMPLETE;
}

bool	RequestParser::hasError() const
{
	return _state == ERROR;
}

RequestParser::State	RequestParser::getState() const
{
	return _state;
}

const std::string& RequestParser::getErrorMsg() const
{
	return _error;
}

const HttpRequest& RequestParser::getRequest() const
{
	return _request;
}

//Private
size_t	RequestParser::findEnd(const std::string& data, size_t start) const
{
	//Tolerance CRLF
	size_t pos = data.find("\r\n", start);
	if (pos != std::string::npos)
		return pos;
	//Tolerance LF
	return data.find("\n", start);
}

std::string RequestParser::toLower(const std::string& str) const
{
	std::string result;
	result.reserve(str.size()); //protected
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		result += std::tolower(static_cast<unsigned char>(*it));
	return (result);
}

bool	RequestParser::isValidMethod(const std::string& method) const
{
	static const std::string methodsArr[] = {"GET", "POST", "HEAD", "DELETE", "PUT", "CONNECT"};
	static const std::vector<std::string> validMethods(methodsArr, methodsArr + 6);

	return std::find(validMethods.begin(), validMethods.end(), method) != validMethods.end();
}

bool	RequestParser::isValidToken(const std::string& token) const
{
	if (token.empty())
		return false;
	for (size_t i = 0; i < token.length(); ++i)
	{
		char	c = token[i];
		if (!std::isalnum(c) &&
			c != '!' && c != '#' && c != '$' && c != '%' && c != '&' &&
            c != '\'' && c != '*' && c != '+' && c != '-' && c != '.' &&
            c != '^' && c != '_' && c != '`' && c != '|' && c != '~')
			return false;
	}
	return true;
}

bool	RequestParser::parseRequestLine(const std::string& line)
{
	// Fisrt Space
	size_t	fSpace = line.find(' ');
	if (fSpace == std::string::npos)
	{
		_error = "No space found after method";
		return false;
	}
	
	// Second Space
	size_t	sSpace = line.find(' ', fSpace + 1);
	if (sSpace == std::string::npos)
	{
		_error = "No space found after URI";
		return false;
	}

	std::string method = line.substr(0, fSpace);
	std::string uri = line.substr(fSpace + 1, sSpace - fSpace - 1);
	std::string	version = line.substr(sSpace + 1);

	if (method.empty())
	{
		_error = "Error method";
		return false;
	}
	if (!isValidMethod(method))
	{
		_error = "Invalid method: " + method;
		return false;
	}

	if (uri.empty())
	{
		_error = "Empty URI";
		return false;
	}
	if (uri[0] != '/' && method != "CONNECT")
	{
		_error = "URI must start with '/'";
		return false;
	}

	if (version != "HTTP/1.1" && version != "HTTP/1.0")
	{
		_error = "Unsupported HTTP version: " + version;
		return false;
	}
	_request.setMethod(method);
	_request.setUri(uri);
	_request.setVersion(version);
	return true;
}

void	RequestParser::RequestLineBuffer()
{
	size_t pos = findEnd(_buffer, 0);
	if (pos == std::string::npos)
		return ; //Wait all data
	
	std::string line = _buffer.substr(0, pos);
	if (_buffer[pos] == '\r' && pos + 1 < _buffer.length() && _buffer[pos + 1] == '\n')
		_buffer.erase(0, pos + 2); //CRFL
	else
		_buffer.erase(0, pos + 1); //FL

	if (!parseRequestLine(line)) //Parsing request line
	{
		_state = ERROR;
		return ;
	}
	_state = PARSING_HEADERS; //Change status
}

bool	RequestParser::parseHeader(const std::string& line)
{
	if (line.empty())
		return true;
	
	size_t pos = line.find(':');
	if (pos == std::string::npos)
	{
		_error = "No ':' found in header line";
		return false;
	}

	std::string name = line.substr(0, pos); //get name
	std::string value = line.substr(pos + 1);

	name = trim(name); //delete space
	value = trim(value);

	if (name.empty())
	{
		_error = "Empty header name";
		return false;
	}
	if (!isValidToken(name))
	{
		_error = "Invalid header name: " + name;
		return false;
	}
	_request.getHeaders().set_http(name, value);

	std::string lName = toLower(name);
	if (lName == "content-length")
	{
		try
		{
			char *end_ptr = NULL;

			_contentLen = std::strtoull(value.c_str(), &end_ptr, 10); //Investigar
		}
		catch (...)
		{
			_error = "Invalid Content-Length value: " + value;
			return false;
		}
	}
	else if (lName == "host")
	{
		if (_request.getVersion() == "HTTP/1.1" && value.empty())
		{
			_error = "Empty Host header un HTTP/1.1 request";
			return false;
		}
	}
	return true;
}

void	RequestParser::HeaderBuffer()
{
	size_t	pos = findEnd(_buffer, 0);
	if (pos == std::string::npos)
		return ;
	
	std::string line = _buffer.substr(0, pos);
	if (_buffer[pos] == '\r' && pos + 1 < _buffer.length() && _buffer[pos + 1] == '\n')
		_buffer.erase(0, pos + 2); //CRFL
	else
		_buffer.erase(0, pos + 1); //FL

	if (line.empty())
	{
		if (_contentLen > 0)
			_state = PARSING_BODY;
		else
			_state = COMPLETE;
	}
	else
	{
		if (!parseHeader(line))
		{
			_state = ERROR;
			return ;
		}
	}
}

void RequestParser::BodyBuffer()
{
	if (_buffer.length() >= _contentLen)
	{
		_request.setBody(_buffer.substr(0, _contentLen));
		_buffer.erase(0, _contentLen);
		_state = COMPLETE;
	}
}
