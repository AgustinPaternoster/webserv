/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 13:07:05 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/18 13:16:36 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"

//Private

void ResponseBuilder::upd_Headers()
{
	//Update time
	_response.getHeaders().set_http("Date", getDateHTTP());

	//Update Content-legnth
	if (!_response.getBody().empty())
	{
		std::ostringstream oss;
		oss << _response.getBody().length();
		_response.getHeaders().set_http("Content-Length", oss.str());
	}
}

std::string	ResponseBuilder::getDateHTTP() const
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

std::string	ResponseBuilder::readFile(const std::string& fileP) const
{
	std::ifstream file(fileP.c_str(), std::ios::binary);
	if (!file.is_open())
		return "";
	
	std::ostringstream oss;
	oss << file.rdbuf();  // .rdbuf() returns the pointer to the underlying buffer stream
	return oss.str();
}


//Public
ResponseBuilder::ResponseBuilder()
{
	_response.setVersion("HTTP/1.1");
	_response.setStatusCode(200);
	_response.getHeaders().set_http("Server", "Webserv/1.0");
}

ResponseBuilder::~ResponseBuilder()
{
}

ResponseBuilder& ResponseBuilder::setStatus(int code)
{
	_response.setStatusCode(code);
	_response.setReason(HttpStatusCode::getReason(code));
	return *this;
}


ResponseBuilder& ResponseBuilder::setBody(const std::string& std)
{
	_response.setBody(std);
	return *this;
}

ResponseBuilder& ResponseBuilder::setHeader(const std::string& name, const std::string& value)
{
	_response.getHeaders().set_http(name, value);
	return *this;
}

ResponseBuilder& ResponseBuilder::setContent(const std::string& type)
{
	_response.getHeaders().set_http("Content-Type", type);
	return *this;
}

ResponseBuilder& ResponseBuilder::setBodyFile(const std::string& file)
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
		_response.setBody(content);
	}
	else
	{
		setStatus(404);
		setBody("File not found");
		setContent("text/plain");
	}
	return *this;
}

HttpResponse	ResponseBuilder::build()
{
	upd_Headers();
	return _response;
}

/*
void	ResponseBuilder::reset()
{
	_response.clear();
	_response.setVersion("HTTP/1.1");
	_response.setStatusCode(200);
	_response.getHeaders().set_http("Server", "Webserv/1.0");
}*/