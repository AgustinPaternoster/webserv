/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequestParser.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:19:20 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/12/19 18:59:36 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPREQUESTPARSER_HPP_
# define _HTTPREQUESTPARSER_HPP_

# include <iostream>
# include <algorithm>
# include <string>
# include <cctype>
# include <sstream>
# include <vector>
# include <string>
# include <cstdlib>
# include "HTTPRequest.hpp"

class RequestParser
{
	public:
		enum State{
			PARSING_REQUEST_LINE, //state 1: Parssing GET /uri
			PARSING_HEADERS, // state 2: Parsing headers
			PARSING_BODY,
			COMPLETE, //State final: Complete
			ERROR
		};

	private:
		State		_state;
		std::string	_buffer;
		HttpRequest	_request;
		size_t		_contentLen;
		std::string	_error;
		bool		parseRequestLine(const std::string& line);
		bool		parseHeader(const std::string& line);
		void		RequestLineBuffer(); //Parsing form line
		void		HeaderBuffer(); //Parsing form Header 
		void		BodyBuffer(); //Parsing form body
		size_t		findEnd(const std::string& data, size_t start = 0) const;
		std::string	toLower(const std::string& str) const;
		bool		isValidMethod(const std::string& method) const;
		bool		isValidToken(const std::string& token) const;

	public:
		RequestParser();
		~RequestParser();

		void				feedData(const std::string& data);
		void				parseBuffer();
		void				reset_parse();

		bool				isComplete() const;
		bool				hasError() const;
		State				getState() const;
		const std::string&	getErrorMsg() const;
		const HttpRequest&	getRequest() const;
		void				printState() const;
		bool				isValidHeaderValue(const std::string& value);
};

std::string trim(const std::string &str);
#endif
