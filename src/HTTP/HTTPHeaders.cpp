/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeaders.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:30:48 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/04 18:23:48 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/HTTP/HTTPHeaders.hpp"

//Private
std::string HttpHeaders::toLower(const std::string& str) const
{
	std::string result;
	result.reserve(str.size()); //protected
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
		result += std::tolower(static_cast<unsigned char>(*it));
	return (result);
}

//Public
HttpHeaders::HttpHeaders()
{
}

HttpHeaders::~HttpHeaders()
{
}

void	HttpHeaders::set_http(const std::string& name, const std::string &value)
{
	_header[toLower(name)] = value;
}

std::string	HttpHeaders::get_http(const std::string& name) const
{
	std::map<std::string, std::string>::const_iterator it = _header.find(toLower(name));
	if (it != _header.end())
		return it->second;
	else
		return "";
}

//booleans

bool	HttpHeaders::has_http(const std::string& name) const
{
	return _header.find(toLower(name)) != _header.end();
}

bool	HttpHeaders::is_empty() const
{
	return _header.empty();
}

// Utils
void	HttpHeaders::remove_http(const std::string& name)
{
	_header.erase(toLower(name));
}

void	HttpHeaders::clear()
{
	_header.clear();
}

size_t	HttpHeaders::size() const
{
	return _header.size();
}

std::string	HttpHeaders::toString() const
{
	std::ostringstream os;
	std::map<std::string, std::string>::const_iterator it;
	
	for (it = _header.begin(); it != _header.end(); ++it)
		os << it->first << ": " << it->second << "\r\n";
	return os.str();
}

const std::map<std::string, std::string>& HttpHeaders::get_all_http() const
{
	return _header;
}
