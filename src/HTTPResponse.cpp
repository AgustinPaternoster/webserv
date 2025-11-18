/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:48:31 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/18 13:21:05 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPResponse.hpp"

/*Private*/

void	HttpReponse::upd_headers()
{
	//Update time
	this->getHeaders().set_htpp("Date", getDateHttp())

	//Update Content-legnth
	if (!this->getBody().empty())
	{
		std::ostringstream oss;
		oss << _response.getBody().length();
		this->getHeaders().set_http("Content-Length", oss.str());
	}
}

std::string	HttpReponse::getDateHTTP() const
{
	time_t now = time(0);
	struct tm* ptm = gmtime(&now); //covert hour

	struct tm aux = *ptm; //copy 
	char buf[100]; //buffer
	size_t len = strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &aux);

	if (len > 0)
		return std::string(buf);
	else
		return "";
}

std::string	HttpReponse::readFile(const std::string& fileP) const
{
	std::ifstream file(fileP.c_str(), std::ios::binary);
	if (!file.is_open())
		return "";
	
	std::ostringstream oss;
	oss << file.rdbuf();  // .rdbuf() returns the pointer to the underlying buffer stream
	return oss.str();
}

/*Public*/
HttpResponse::HttpResponse() : _version("HTTP/1.1"), _statusCode(200), _reason("OK")
{
	this->getHeaders().set_http("Server", "Webserv/1.0");
}

HttpResponse::HttpResponse(int statuscode) : _version("HTTP/1.1"), _statusCode(statuscode)
{
	_reason = HttpStatusCode::getReason(statuscode);
	this->getHeaders().set_http("Server", "Webserv/1.0");
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

void	HttpResponse::setHeader(const std::string& name, const std::string& value)
{
	this->getHeaders().set_http(name, value);
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

HttpResponse	HttpResponse::build()
{
	upd_headers();
	return *this;
}

HttpResponse& HttpResponse::setContent(const std::string& type)
{
	this->getHeaders().set_http("Content-Type", type);
	return *this;
}

HttpResponse& HttpResponse::setBodyFile(const std::string& file)
{
	std::string content = readFile(file);
	if (!content.empty())
	{
		if (file.find(".html") != std::string::npos)
			setContent("text/html");
		else if (file.find(".json") != std::string::npos)
			setContent("application/json");
		else if (file.find(".css") != std::string::npos)
			setContent("text/css");
		else if (file.find(".js") != std::string::npos)
			setContent("application/javascript");
		else if (file.find(".png") != std::string::npos)
			setContent("image/png");
		else if (file.find(".jpg") != std::string::npos || file.find(".jpeg") != std::string::npos)
			setContent("image/jpeg");
		else
			setContent("application/octet-stream");
		this->setBody(content);
	}
	else
	{
		_statusCode = 404;
		_reason = HttpStatusCode::getReason(404);
		this->getHeaders().set_http("Server", "Webserv/1.0");
		this->setBody("File not found");
		this->setContent("text/plain");
	}
	return *this;
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


void	HttpResponse::execute(HttpRequest par, Config &config)
{
	if (isvalidmethod(par, config))
	{
		_statusCode = 405;
		_reason = HttpStatusCode::getReason(405);
		this->getHeaders().set_http("Server", "Webserv/1.0");
		this->setBody("File not found");
		this->setContent("text/plain");
	}
	if (isGCI(par, config)) //only flag
	switch (expression)
	{
	case constant expression:
		/* code */
		break;
	
	default:
		_statusCode = 501;
		_reason = HttpStatusCode::getReason(501);
		this->getHeaders().set_http("Server", "Webserv/1.0");
		this->setBody("File not found");
		this->setContent("text/plain");;
	}
}