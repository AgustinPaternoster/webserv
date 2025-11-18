/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:15:11 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/18 13:03:21 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <iostream>
# include <sstream>
# include "HTTPHeaders.hpp"

/**
 * @class HttpRequest
 * Representa un HTTP request completo
 */

class HttpRequest
{
	private:
			std::string	_method; //Get, post, put, Delete
			std::string	_uri; //-path/to/resource
			std::string	_version; //HTTP/1.1
			std::string	_body; //optional
			HttpHeaders _headers;
	public:
			HttpRequest();
			~HttpRequest();

			//Getters
			const std::string& getMethod() const;
			const std::string& getUri() const;
			const std::string& getVersion() const;
			const std::string& getBody() const;
			const HttpHeaders& getHeaders() const;

			HttpHeaders& getHeaders();
			//Setters
			void	setMethod(const std::string& method);
			void	setUri(const std::string& uri);
			void	setVersion(const std::string& version);
			void	setHeaders(const HttpHeaders& headers);
			void	setBody(const std::string& body);
			std::string	getPort() const;

			//Utils
			void	clear();
			void	print_http() const;
			std::string	toString() const;
			HttpRequest fromString(const std::string& request);
};

#endif