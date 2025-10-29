/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrodrigu <yrodrigu@student.42barcelo>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:17:53 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/10/29 19:00:38 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Common.hpp"

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
//		Socket(const Socket &obj);
//		Socket	&operator=(const Socket &obj);
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
		void	clean_server_info();
		void	print_error();
		static int	webserver_init();
};
