/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:20:27 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/05 13:04:53 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "CgiTasks.hpp"

Socket::Socket(): socket_fd(-1), server_info(NULL) {
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	client_addr_size = sizeof(client_addr);
}

Socket::Socket(const Socket &obj) {

	*this = obj;
};

Socket &Socket::operator=(const Socket &obj) {
	
	if (this != &obj) {
        socket_fd = obj.socket_fd;
        port = obj.port;
        hints = obj.hints;
        client_addr = obj.client_addr;
        client_addr_size = obj.client_addr_size;
        server_info = NULL;
    }
	return (*this);
};

Socket::~Socket() {
	
	if (server_info) {
		freeaddrinfo(server_info);
		server_info = NULL;
	}

	if (socket_fd != -1) {	
		close(socket_fd);
		socket_fd = -1;
	}
};

int	Socket::set_addrinfo() {

	int status = getaddrinfo(NULL, port.c_str(), &hints, &server_info);
	return (status);
}

int	Socket::create_socket() {

	this->socket_fd = socket(server_info->ai_family,
							server_info->ai_socktype,
							server_info->ai_protocol);
	if (socket_fd != -1) {

		int yes = 1;
		setsockopt(socket_fd, SOL_SOCKET, 
				SO_REUSEADDR, &yes, sizeof(yes));
		fcntl(socket_fd, F_SETFL, O_NONBLOCK);
	}
	return (this->socket_fd);
}

int	Socket::binding() {

	int bind_result = bind(socket_fd,
							server_info->ai_addr,
							server_info->ai_addrlen);
	return (bind_result);
}

int	Socket::listening() {

	int listen_result = listen(socket_fd, 10);
	
	return (listen_result);
}

int	Socket::accepting() {

	int client_fd = accept(socket_fd,
						(struct sockaddr *)&client_addr,
						&client_addr_size);
	return (client_fd);
}

void	Socket::clean_server_info() {

	freeaddrinfo(server_info);
}

void	Socket::print_error() {

	write(2, strerror(errno), strlen(strerror(errno)));
	write(2, "\n", 1);
}

int	Socket::getsocket_fd() const {

	return (socket_fd);
}

int g_signal = true;

void    signal_handler(int signum) {

    (void)signum;
    g_signal = false;
}

int	Socket::webserver_init(Config &config) {

	std::vector<Socket *>	sockets;
	std::vector<t_server>	servers = config.getServers();
	std::map<int, std::string>	client_requests;
	std::vector<struct pollfd> poll_fds;
	CgiTask CgiJobs;
	config.CgiJobs = CgiJobs;
	

	if (socket_creation(sockets, servers))
		return (1);
	add_sockets_poll(sockets, poll_fds);
	std::signal(SIGINT, signal_handler);
	
	while (g_signal) {
		
		std::cout << "🕐 Waiting for connections..." << std::endl;
		
		int event_ready = poll_events_ready(poll_fds);
		
		if(event_ready == 1)
			break ;
		if(event_ready == 2)
			continue;
		connect_to_clients(poll_fds, sockets, client_requests, config);
	}

	for (size_t i = 0; i < sockets.size(); i++) {
		delete sockets[i];
	}

	return (0);
}
