/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrodrigu <yrodrigu@student.42barcelo>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:26:45 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/04 15:48:27 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"

int g_signal = true;

bool	is_listening_socket(int fd, const std::vector<Socket *>& sockets) {

	for (size_t j = 0; j < sockets.size(); ++j) {
		if (fd == sockets[j]->getsocket_fd()) {
			return (true);
		}
	}
	return (false);
}

int	socket_creation(std::vector<Socket *> &sockets, std::vector<t_server> &servers) {
	
	for (size_t i = 0; i < servers.size(); i++) {
	
		Socket	*socket = new Socket();

		socket->set_port(servers[i].port);
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
	return (0);
}

void	close_pollfd(std::vector<struct pollfd> &poll_fds, size_t &i) {
	
	close(poll_fds[i].fd);
	poll_fds.erase(poll_fds.begin() + i);
	i--;
}

void	add_sockets_poll(std::vector<Socket *> &sockets, std::vector<struct pollfd> &poll_fds) {
	
	for (size_t i = 0; i < sockets.size(); i++) {
		
			struct	pollfd socket_fd;
			socket_fd.fd = sockets[i]->getsocket_fd();
			socket_fd.events = POLLIN;
			socket_fd.revents = 0;
			poll_fds.push_back(socket_fd);
		}
}

void	connect_to_clients(std::vector<struct pollfd> &poll_fds, std::vector<Socket *> &sockets,
		std::map<int, std::string> &client_requests) {
	
	for (size_t i = 0; i < poll_fds.size(); i++) {
		
		if (poll_fds[i].revents & POLLIN) {
			
			if (is_listening_socket(poll_fds[i].fd, sockets)) {
				
				int client_fd = sockets[i]->accepting();
				std::cout << "\e[0;92mNew CLient fd is created: " << client_fd << "\e[0m" << std::endl;
				struct pollfd new_polls;
				new_polls.fd = client_fd;
				new_polls.events = POLLIN;
				new_polls.revents = 0;
				poll_fds.push_back(new_polls);
			}
			else {
				std::cout << "\e[0;92mclient able to send data " << poll_fds[i].fd << "\e[0m"  << std::endl;
				char	buffer[4096];
				int bytes = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);

				if (bytes <= 0) {
					if (bytes < 0)
						std::cerr << "Error: recv() funtion\n";
					if (bytes == 0)
						std::cout << "Client closed connectin gracefully.\n";
					close_pollfd(poll_fds, i);
					continue ;
				}
				
				client_requests[poll_fds[i].fd].append(buffer, bytes);
				std::string	&request_str = client_requests[poll_fds[i].fd];
				size_t end_pos = request_str.find("\r\n\r\n");
						
				if (end_pos != std::string::npos) {
				
					std::cout << request_str;
					std::cout << "---------------------------\n";
					
					int sent_bytes = send(poll_fds[i].fd, get_http(), HTTP_LEN, 0);
					if (sent_bytes > 0)
						close_pollfd(poll_fds, i);
					else
						std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
					request_str = "";
					request_str.clear();
				}
			}	
		}
	}
}

int	poll_events_ready(std::vector<struct pollfd> &poll_fds) {

		int events;
		
		events = poll(poll_fds.data(), poll_fds.size(), -1);
		if (events == -1) {
			
			if (errno == EINTR && g_signal == false) {
				std::cerr << "\nSignal INTERRUPTION CALLED\n";
				return (1);
			}
			else if (errno != EINTR) {
				std::cerr << "Poll critical error: " << strerror(errno) << std::endl;
				return (1);
			}
			else
				return (2);
		}
		return (0);
}
