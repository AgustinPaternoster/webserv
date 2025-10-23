/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHeaders.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 20:16:22 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/10/23 20:50:21 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPHEADERS_HPP
# define HTTPHEADERS_HPP

# include <string>
# include <map>
# include <algorithm>
# include <sstream>

/**
 * @class HttpHeaders
 * @brief Gestiona headers HTTP con comparación case-insensitive
 */

class HttpHeaders
{
	private:
			std::map<std::string, std::string>	_header;
			std::string							toLower(const std::string& str) const;

	public:
			HttpHeaders();
			~HttpHeaders();

			//setters
			void		set_http(const std::string& name, const std::string &value);
			std::string	get_http(const std::string& name) const;

			//booleans
			bool		has_http(const std::string& name) const;
			bool		is_empty() const;

			//aux
			void		remove_http(const std::string& name); 
			void		clear(); //clean all headers
			const		std::map<std::string, std::string>& get_all_http() const;
			std::string toString() const;
			size_t		size() const;



};

#endif