/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:20:27 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/03 11:46:09 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Socket.hpp"


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

int	g_signal = true;

void    signal_handler(int signum) {

    (void)signum;
    g_signal = false;
}

bool	is_listening_socket(int fd, const std::vector<Socket *>& sockets) {
    for (size_t j = 0; j < sockets.size(); ++j) {
        if (fd == sockets[j]->getsocket_fd()) {
            return (true);
        }
    }
    return (false);
}

int	Socket::webserver_init(Config &config) {

	std::vector<Socket *>	sockets;
	std::vector<t_server>	servers = config.getServers();

	for (size_t i = 0; i < servers.size(); i++) {
	
		Socket	*socket = new Socket();

		socket->port = servers[i].port;
		int status = socket->set_addrinfo();
		
		if (status != 0) {
        	std::cout << gai_strerror(status) << std::endl;
        	delete socket;
			return (1);
    	}
		if (socket->create_socket() == -1 || socket->binding() == -1 || socket->listening() == -1)
    	{
        	socket->print_error();
			delete socket;
        	return (1);
    	}
		std::cout << "Socket in PORT: " << socket->getport() << std::endl;
		std::cout << "Socket fd: " << socket->getsocket_fd() << std::endl;
			sockets.push_back(socket);
	}
	
	std::vector<struct pollfd> poll_fds;
		
	for (size_t i = 0; i < sockets.size(); i++) {
		
			struct	pollfd socket_fd;
			socket_fd.fd = sockets[i]->getsocket_fd();
			socket_fd.events = POLLIN;
			socket_fd.revents = 0;
			poll_fds.push_back(socket_fd);
		}

		std::signal(SIGINT, signal_handler);
		
		while (g_signal) {
			
			std::cout << "🕐 Waiting for connections..." << std::endl;
			
			int events = poll(poll_fds.data(), poll_fds.size(), -1);
			
			if (events == -1) {
			
				if (errno == EINTR && g_signal == false) {
					std::cerr << "\nSignal INTERRUPTION CALLED\n";
					break ;
				}
				else if (errno != EINTR) {
					std::cerr << "Poll critical error: " << strerror(errno) << std::endl;
					break ;
				}
				else
					continue ;
			}

			for (size_t i = 0; i < poll_fds.size(); i++) {
				
				if (poll_fds[i].revents & POLLIN) {
				
				if (is_listening_socket(poll_fds[i].fd, sockets)) {
						
						int client_fd = sockets[i]->accepting();
						std::cout << "New CLient fd is created: " << client_fd << std::endl;
						struct pollfd new_polls;
	
						new_polls.fd = client_fd;
						new_polls.events = POLLIN;
						new_polls.revents = 0;
	
						poll_fds.push_back(new_polls);
		
				}
				else {
						
						std::cout << "client able to send data " << poll_fds[i].fd << std::endl;
					
						char	buffer[4096];
						
						int bytes = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);
						
						if (bytes > 0) {
						
							buffer[bytes] = '\0';
							std::cout << buffer;
							int sent_bytes = send(poll_fds[i].fd, get_http(), HTTP_LEN, 0);
							if (sent_bytes > 0) {
								close(poll_fds[i].fd);
								poll_fds.erase(poll_fds.begin() + i);
								i--;
							}
							else
								std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
						}
						if (bytes == 0) {
							
							close(poll_fds[i].fd);
							poll_fds.erase(poll_fds.begin() + i);
							i--;
						}
						if (bytes == -1) {

							close(poll_fds[i].fd);
							poll_fds.erase(poll_fds.begin() + i);
							i--;
							std::cerr << "Error in Recv(): " << strerror(errno) << std::endl;
						}
				}
				
				}
			}
		}

		for (size_t i = 0; i < sockets.size(); i++) {
			delete sockets[i];
		}
	return (0);
}
