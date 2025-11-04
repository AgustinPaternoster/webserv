/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:17:53 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/04 16:13:45 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"

extern int g_signal;

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

bool	is_listening_socket(int fd,
		const std::vector<Socket *>& sockets);
int		socket_creation(std::vector<Socket *> &sockets,
		std::vector<t_server> &servers);
void	close_pollfd(std::vector<struct pollfd> &poll_fds, size_t &i);
void	add_sockets_poll(std::vector<Socket *> &sockets,
		std::vector<struct pollfd> &poll_fds);
void	connect_to_clients(std::vector<struct pollfd> &poll_fds,
		std::vector<Socket *> &sockets,
		std::map<int, std::string> &client_requests);
int		poll_events_ready(std::vector<struct pollfd> &poll_fds);
int		process_requests(std::vector<struct pollfd> &poll_fds,
		std::map<int, std::string> &client_requests, size_t &i);
