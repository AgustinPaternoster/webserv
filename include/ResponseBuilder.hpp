/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 12:56:38 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/18 13:19:14 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _RESPONSEBUILDER_HPP_
# define _RESPONSEBUILDER_HPP_

# include <string>
# include <ctime> //time_t, time, gmtime, tm
# include <sstream>
# include <fstream>
# include <iostream>
# include <HTTPResponse.hpp>
# include <HTTPStatusCode.hpp>

class ResponseBuilder
{
	private:
			HttpResponse	_response;
			void			upd_Headers();
			std::string		getDateHTTP() const;
			std::string		readFile(const std::string& file) const;

	public:
			ResponseBuilder();
			~ResponseBuilder();
			
			ResponseBuilder& setStatus(int code);
			ResponseBuilder& setHeader(const std::string& name, const std::string& value);
			ResponseBuilder& setBody(const std::string& std);
			ResponseBuilder& setContent(const std::string& type);
			ResponseBuilder& setBodyFile(const std::string& file);

			HttpResponse	build();
			void			reset();

};

#endif