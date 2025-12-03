/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:42:37 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/26 13:00:51 by apaterno         ###   ########.fr       */
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
#include <cstdlib>

size_t  get_port_www(Config &config, const HttpRequest& par);
std::string get_error_page(const std::map<std::string,std::string>& error_pages, const std::string& code);
int stringToMethod(const std::string &s);
std::string getServerPort(int client_fd);
size_t parseSize(const std::string &sizeStr);
std::string joinPaths(const std::string& part1, const std::string& part2);


#endif