/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:17:53 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/04 14:47:29 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
class	Socket {

	private:
		int						socket_fd;
		std::string				port;
		struct addrinfo			hints;
		struct addrinfo			*server_info;
		struct sockaddr_storage	client_addr;
		socklen_t				client_addr_size;
		Socket(const Socket &obj);
		Socket	&operator=(const Socket &obj);
	public:
		Socket();
		~Socket();
		int		set_addrinfo();
		int		create_socket();
		int		binding();
		int		listening();
		int		accepting();
		int		getsocket_fd() const;
		std::string	getport() const {
		
			return (port);
		};
		void	set_port(std::string def_port) {
			port = def_port;
		};
		void	clean_server_info();
		void	print_error();
		static int	webserver_init(Config &config);
};
