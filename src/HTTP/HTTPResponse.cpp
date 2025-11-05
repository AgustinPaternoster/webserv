/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:48:31 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/04 13:32:49 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/HTTP/HTTPResponse.hpp"

HttpResponse::HttpResponse() : _version("HTTP/1.1"), _statusCode(200), _reason("OK")
{
}

HttpResponse::HttpResponse(int statuscode) : _version("HTTP/1.1"), _statusCode(statuscode)
{
	//Default
	_reason = "OK"; 
}

HttpResponse::~HttpResponse()
{
}

//Getters
const std::string& HttpResponse::getVersion() const
{
	return _version;
}

int HttpResponse::getStatus() const
{
	return _statusCode;
}

const std::string& HttpResponse::getReason() const
{
	return _reason;
}

const HttpHeaders& HttpResponse::getHeaders() const
{
	return _headers;
}

const std::string& HttpResponse::getBody() const
{
	return _body;
}

HttpHeaders& HttpResponse::getHeaders()
{
	return _headers;
}

//Setters
void	HttpResponse::setVersion(const std::string& version)
{
	_version = version;
}

void	HttpResponse::setStatusCode(int code)
{
	_statusCode = code;
}

void	HttpResponse::setReason(const std::string& phrase)
{
	_reason = phrase;
}

void	HttpResponse::setHeaders(const HttpHeaders& headers)
{
	_headers = headers;
}

void	HttpResponse::setBody(const std::string& body)
{
	_body = body;
}

//Utils
std::string HttpResponse::toString() const
{
	std::ostringstream oss;

	oss << _version << " " << _statusCode << " " << _reason << "\r\n";
	oss << _headers.toString();
	oss << "\r\n";
	if (!_body.empty())
		oss << _body;
	return oss.str();
}

void	HttpResponse::clear()
{
	_version = "HTTP/1.1";
	_statusCode = 200;
	_reason = "OK";
	_headers.clear();
	_body.clear();
}

void	HttpResponse::printResponse()
{
	std::cout << "=== HTTP Response ===" << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Status: " << _statusCode << " " 
				<< _reason << std::endl;
	std::cout << "Headers (" << _headers.size() << "):" << std::endl;
	
	const std::map<std::string, std::string>& allHeaders = 
		_headers.get_all_http();
	std::map<std::string, std::string>::const_iterator it;
	for (it = allHeaders.begin(); it != allHeaders.end(); ++it) {
		std::cout << "  " << it->first << ": " 
					<< it->second << std::endl;
	}
	
	if (!_body.empty()) {
		std::cout << "Body length: " << _body.length() 
					<< " bytes" << std::endl;
		if (_body.length() < 200) {
			std::cout << "Body: " << _body << std::endl;
		}
	}
	std::cout << "====================" << std::endl;
}