/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPStatusCode.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:57:26 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/05 12:24:58 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPStatusCode.hpp"

std::map<int, std::string> HttpStatusCode::initReason()
{
	std::map<int, std::string> phrases;

	// 1xx
	phrases[100] = "Continue";
	phrases[101] = "Switching Protocols";
	
	// 2xx
	phrases[200] = "OK";
	phrases[201] = "Created";
	phrases[202] = "Accepted";
	phrases[204] = "No Content";
	phrases[206] = "Partial Content";
	
	// 3xx
	phrases[301] = "Moved Permanently";
	phrases[302] = "Found";
	phrases[303] = "See Other";
	phrases[304] = "Not Modified";
	phrases[307] = "Temporary Redirect";
	phrases[308] = "Permanent Redirect";
	
	// 4xx
	phrases[400] = "Bad Request";
	phrases[401] = "Unauthorized";
	phrases[403] = "Forbidden";
	phrases[404] = "Not Found";
	phrases[405] = "Method Not Allowed";
	phrases[408] = "Request Timeout";
	phrases[409] = "Conflict";
	phrases[410] = "Gone";
	phrases[411] = "Length Required";
	phrases[413] = "Payload Too Large";
	phrases[414] = "URI Too Long";
	phrases[415] = "Unsupported Media Type";
	phrases[429] = "Too Many Requests";
	
	// 5xx
	phrases[500] = "Internal Server Error";
	phrases[501] = "Not Implemented";
	phrases[502] = "Bad Gateway";
	phrases[503] = "Service Unavailable";
	phrases[504] = "Gateway Timeout";
	phrases[505] = "HTTP Version Not Supported";
	
	return phrases;
}


//Getters
std::string	HttpStatusCode::getReason(int code)
{
	std::map<int, std::string> ph = initReason();
	std::map<int, std::string>::iterator it = ph.find(code);
	if (it != ph.end())
		return it->second;
	return "Unknown Status";
}

bool HttpStatusCode::isInformational(int code)
{
	return ((code >= 100) & (code < 200));
}

bool HttpStatusCode::isSuccess(int code)
{
	return ((code >= 200) & (code < 300));
}

bool HttpStatusCode::isRedirection(int code)
{
	return ((code >= 300) & (code < 400));
}

bool HttpStatusCode::isClientError(int code)
{
	return ((code >= 400) & (code < 500));
}

bool HttpStatusCode::isServerError(int code)
{
	return ((code >= 500) & (code < 600));
}

bool HttpStatusCode::isError(int code)
{
	return ((code >= 400) & (code < 600));
}