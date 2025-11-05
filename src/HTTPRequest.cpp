/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:02:17 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/05 12:26:57 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPRequest.hpp"

HttpRequest::HttpRequest() : _version("HTTP/1.1")
{
}

HttpRequest::~HttpRequest()
{
}

//Getters
const std::string& HttpRequest::getMethod() const
{
	return (_method);
}

const std::string& HttpRequest::getUri() const
{
	return (_uri);
}

const std::string& HttpRequest::getVersion() const
{
	return (_version);
}

const std::string& HttpRequest::getBody() const
{
	return (_body);
}

const HttpHeaders& HttpRequest::getHeaders() const
{
	return (_headers);
}

HttpHeaders& HttpRequest::getHeaders()
{
	return (_headers);
}

//Setters
void	HttpRequest::setMethod(const std::string& method)
{
	_method = method;
}

void	HttpRequest::setUri(const std::string& uri)
{
	_uri = uri;
}

void	HttpRequest::setVersion(const std::string& version)
{
	_version = version;
}

void HttpRequest::setHeaders(const HttpHeaders& headers)
{
	_headers = headers;
}

void HttpRequest::setBody(const std::string& body)
{
	_body = body;
}


//Utils
void HttpRequest::clear()
{
	_method.clear();
	_uri.clear();
	_version = "HTTP/1.1";
	_headers.clear();
	_body.clear();
}

void HttpRequest::print_http() const
{
	std::cout << "=== HTTP Request ===" << std::endl;
	std::cout << "Method: " << _method << std::endl;
	std::cout << "URI: " << _uri << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Headers (" << _headers.size() << "):" << std::endl;
	
	const std::map<std::string, std::string>& allHeaders = _headers.get_all_http();
	std::map<std::string, std::string>::const_iterator it;
	for (it = allHeaders.begin(); it != allHeaders.end(); ++it) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}
	
	if (!_body.empty()) {
		std::cout << "Body length: " << _body.length() << " bytes" << std::endl;
		if (_body.length() < 200) {
			std::cout << "Body: " << _body << std::endl;
		}
	}
	std::cout << "===================" << std::endl;
}

std::string  HttpRequest::toString() const
{
	std::ostringstream oss;

	//Request line
	oss << _method << " " << _uri << " " << _version << "\r\n";

	//headers
	oss << _headers.toString();

	oss << "\r\n";
	if (!_body.empty())
		oss << _body;
	return oss.str();
}