/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 11:42:11 by nikitadorof       #+#    #+#             */
/*   Updated: 2025/12/09 15:25:11 by camurill         ###   ########.fr       */
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

std::string getServerPort(int client_fd)
{
    std::stringstream ss;
    sockaddr_in srv;
    socklen_t srv_len = sizeof(srv);
    getsockname(client_fd, (sockaddr*)&srv, &srv_len);
    int server_port = ntohs(srv.sin_port);
    ss << server_port;
    return(ss.str());
}

std::string joinPaths(const std::string& part1, const std::string& part2) 
{
    if (part1.empty()) return part2;
    if (part2.empty()) return part1;
    
    // Evita // o falta de /
    bool endSlash = (part1[part1.length() - 1] == '/');
    bool startSlash = (part2[0] == '/');

    if (endSlash && startSlash)
        return part1 + part2.substr(1);
    if (!endSlash && !startSlash)
        return part1 + "/" + part2;
    return part1 + part2;
}

size_t parseSize(const std::string &sizeStr) 
{
    if (sizeStr.empty())
        return 0;
    char *endPtr;
    size_t size = std::strtoul(sizeStr.c_str(), &endPtr, 10);
    if (*endPtr == '\0')
        return size;

    // Chequeamos el sufijo (M, K, G)
    char suffix = *endPtr;
    switch (suffix) {
        case 'k': case 'K':
            size *= 1024;
            break;
        case 'm': case 'M':
            size *= 1024 * 1024;
            break;
        case 'g': case 'G':
            size *= 1024 * 1024 * 1024;
            break;
        default:
            break;
    }
    return size;
}

std::string intToString(int value) 
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string getClientIP(int client_fd)
{
    std::string remote_addr = "0.0.0.0";
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (getpeername(client_fd, (struct sockaddr*)&addr, &addr_len) == 0)
    {
        char ip_str[INET6_ADDRSTRLEN];
        if (addr.ss_family == AF_INET)
        {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            if (inet_ntop(AF_INET, &(s->sin_addr), ip_str, sizeof(ip_str)) != NULL) 
            {
                remote_addr = ip_str;
            }
        }
    }
    else
    {
        // gestionar error
        // std::cerr << "Error al obtener la dirección del peer (getpeername)." << std::endl;
    }
    return(remote_addr);
}