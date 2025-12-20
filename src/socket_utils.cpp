/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:26:45 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/12/20 13:38:35 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

bool	is_listening_socket(int fd, const std::vector<Socket *>& sockets) {

	for (size_t j = 0; j < sockets.size(); ++j) {
		if (fd == sockets[j]->getsocket_fd()) {
			return (true);
		}
	}
	return (false);
}

int	socket_creation(std::vector<Socket *> &sockets, std::vector<t_server> &servers) {
		
	system("clear");
	print_banner();
	std::cout << "\033[96m\n";
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
		std::cout << "# " << "Socket in PORT: " << socket->getport()
			<< " | fd: " << socket->getsocket_fd() << " #" << std::endl;
			sockets.push_back(socket);
	}
	std::cout << "\033[0m\n\n";
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

int	poll_events_ready(std::vector<struct pollfd> &poll_fds) {

		int events;
		
		events = poll(poll_fds.data(), poll_fds.size(), 1000);
		if (events == -1) {
			
			if (errno == EINTR && g_signal == false) {
				system("clear");
				std::cerr << "\n\033[092mSignal Interruption called:\n" 
					<< "\n...Thanks for using our server.\033[0m\n\n";
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


