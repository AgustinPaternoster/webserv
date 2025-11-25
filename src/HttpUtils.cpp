/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:42:11 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/11/25 18:11:42 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpUtils.hpp"
#include "Common.hpp"


void print_servers(const std::vector<t_server> &servers) {
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "\n============================" << std::endl;
        std::cout << "=== SERVER " << i + 1 << " CONFIGURATION ===" << std::endl;
        std::cout << "============================" << std::endl;

        const t_server &server = servers[i];

        std::cout << "Port: " << server.port << std::endl;
        std::cout << "Root: " << server.root << std::endl;
        std::cout << "Index: " << server.index << std::endl;
        std::cout << "Client Max Body Size: " << server.client_max_body_size << std::endl;
    }
    std::cout << "========= END OF SERVERS =========" << std::endl;
}

std::string extract_port_from_host(std::string& host) {
    size_t colon_pos = host.find(':');
    if (colon_pos != std::string::npos) {
        return host.substr(colon_pos + 1);
    }
    return "80";
}

size_t  get_port_www(Config &config, const HttpRequest& par)
{
	const std::vector<t_server> & servers = config.getServers();
	const std::string req_port = par.getPort();
	std::string rew = trim(req_port);
	rew = extract_port_from_host(rew);
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const t_server &server = servers[i];
		if (rew == server.port)
		{
			return i;
		}
	}
	return -1;
}

std::string get_error_page(const std::map<std::string,std::string>& error_pages, const std::string& code) 
{
    std::map<std::string,std::string>::const_iterator it = error_pages.find(code);
    if (it != error_pages.end()) {
        return it->second;
    }
    return "";
}

int stringToMethod(const std::string &s)
{
    if (s == "GET")    return GET;
    if (s == "POST")   return POST;
    if (s == "DELETE") return DELETE;
    if (s == "PUT")    return PUT;
    if (s == "PATCH")  return PATCH;

    return -1;
}