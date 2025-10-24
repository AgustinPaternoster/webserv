/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yrodrigu <yrodrigu@student.42barcelo>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 15:20:27 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/10/24 15:32:34 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Socket.hpp"

Socket::Socket(): socket_fd(-1) {
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	client_addr_size = sizeof(client_addr);
}

Socket::Socket(const Socket &obj) { (void)obj; };

Socket &Socket::operator=(const Socket &obj) {
	
	(void)obj;	
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

	int status = getaddrinfo(NULL, "8080", &hints, &server_info);
	return (status);
}

int	Socket::create_socket() {

	this->socket_fd = socket(server_info->ai_family,
							server_info->ai_socktype,
							server_info->ai_protocol);
	if (socket_fd != -1) {

//		int yes = 1;
		fcntl(socket_fd, F_SETFL, O_NONBLOCK);
//		setsockopt(soc);
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
/*
void    signal_handler(int signum) {

    (void)signum;
    g_signal = false;
}
*/
int	Socket::webserver_init() {

    Socket  socket;

    int status = socket.set_addrinfo();
    if (status != 0) {
        std::cout << gai_strerror(status) << std::endl;
        return (1);
    }

    if (socket.create_socket() == -1 || socket.binding() == -1 || socket.listening() == -1)
    {
        socket.print_error();
        return (1);
    }

    std::cout << "Server listening... on PORT: 8080 " <<  std::endl;

//    std::signal(SIGINT, signal_handler);

    while (1) {

        int client_fd = socket.accepting();
        if (client_fd == -1) {

            if (errno == EAGAIN)
                continue ;
        }
        int send_status = send(client_fd, get_http(), HTTP_LEN, 0);

        /*char  buff[1024];

        int recv_status = recv(client_fd, buff, strlen(buff), 0);
        (void)recv_status;
        std::cout << buff << std::endl;
*/
        if (send_status == -1) {
            socket.print_error();
        }
        else {
            std::cout << "Message sent succesfully to the client...\n";
        }
        close(client_fd);
    }
    return (0);
}
