/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:24:59 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/12/18 13:14:57 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPRESPONSE_HPP_
# define _HTTPRESPONSE_HPP_

# include <string>
# include <sstream>
# include <iostream>
# include <ctime> //time_t, time, gmtime, tm
# include <fstream>
# include <dirent.h> //dir
# include <sys/stat.h>
# include <sys/types.h>
# include "HTTPHeaders.hpp"
# include "HTTPRequest.hpp"
# include "HTTPStatusCode.hpp"
# include "Socket.hpp"
# include "HttpUtils.hpp"

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
			void		upd_headers();
			std::string	getDateHttp() const;
			std::string readFile(const std::string& file) const;

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
			void	setHeader(const std::string& name, const std::string& value);
			void	setBody(const std::string& vody);

			//Utils
			std::string toString() const;
			void		clear();
			void		printResponse();

			//Algorithm
			HttpResponse	build();
			HttpResponse&	setContent(const std::string& type);
			HttpResponse&	setBodyFile(const std::string& file);
			std::string		execute_response(HttpRequest par, t_server server);
			std::string		handle_get(HttpRequest par, t_server server, int flag);
			std::string		handle_post(HttpRequest par, t_server server, int flag);
			std::string		handle_delete(HttpRequest par, t_server server, int flag);

			//aux
			bool			isvalidmethod(HttpRequest par, t_server server);
			bool			isFile(const std::string &path);
			bool			isDir(const std::string &path);
			bool			haveIndex(const std::string &path, t_server server);
			std::string		getIndexFile(const std::string &path, t_server server);
			std::string		autoIndexDir(const std::string &path);
			std::string		generateError(int code, const t_server& server, const std::string& msg);
			std::string		build_redict(HttpRequest par, t_server server);
};

#endif