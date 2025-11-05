/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:24:59 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/04 16:39:03 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPRESPONSE_HPP_
# define _HTTPRESPONSE_HPP_

# include <string>
# include <sstream>
# include <iostream>
# include "HTTPHeaders.hpp"

/**
 * @class HttpResponse
 * Representa un HTTP response completo
 */

class HttpResponse
{
	private:
			std::string	_version; //HTTP/1,1
			int			_statusCode; //20X, 30X, 40X, 50X
			std::string	_reason; //ok, Not Found
			HttpHeaders	_headers;
			std::string	_body;

	public:
			HttpResponse();
			HttpResponse(int statuscode);
			~HttpResponse();

			//Getters
			const std::string&	getVersion() const;
			const std::string&	getReason() const;
			int					getStatus() const;
			const std::string&	getBody() const;
			const HttpHeaders& getHeaders() const;

			HttpHeaders& getHeaders();

			//Setters
			void	setVersion(const std::string& version);
			void	setStatusCode(int code);
			void	setReason(const std::string& phrase);
			void	setHeaders(const HttpHeaders& headers);
			void	setBody(const std::string& vody);

			//Utils
			std::string toString() const;
			void		clear();
			void		printResponse();

};

#endif