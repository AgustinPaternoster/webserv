/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:42:37 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/11 15:32:39 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _HTTPUTILS_HPP_
# define _HTTPUTILS_HPP_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "Config.hpp"
#include "HTTPRequestParser.hpp"
#include "ResponseBuilder.hpp"

size_t  get_port_www(Config &config, const HttpRequest& par);
std::string get_error_page(const std::map<std::string,std::string>& error_pages, const std::string& code);


#endif