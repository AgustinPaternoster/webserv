/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:55:44 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/11 15:34:38 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "HttpUtils.hpp"

int	process_request(std::vector<struct pollfd> &poll_fds,
		std::map<int, std::string> &client_requests, size_t &i, Config &config) {
	

	char	buffer[4096];
	int bytes = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);
	if (bytes <= 0) {
		if (bytes < 0)
			std::cerr << "Error: recv() funtion\n";
		if (bytes == 0)
			std::cout << "Client closed connectin gracefully.\n";
		close_pollfd(poll_fds, i);
		return (1);
	}
	
	client_requests[poll_fds[i].fd].append(buffer, bytes);
	std::string	&request_str = client_requests[poll_fds[i].fd];
	size_t end_pos = request_str.find("\r\n\r\n");
	if (end_pos != std::string::npos) 
	{
		RequestParser	parser;
		parser.feedData(request_str);
		if (!parser.isComplete())
			std::cerr << "Error: Failed to parse request" << std::endl;
		const HttpRequest& par = parser.getRequest();

		ResponseBuilder	rb;
		size_t num = get_port_www(config, par);
		const std::vector<t_server> & servers = config.getServers();
		const t_server &server = servers[num];
				
		if (par.getMethod() == "GET")
		{
			if (par.getUri() == "/")
			{
				std::string full_path = server.root;
				if (!full_path.empty() && full_path.back() != '/')
					full_path += '/';
				full_path += server.index;
				rb.setBodyFile(full_path);
			}
			else if (par.getUri() == "/api/users")
				rb.setContent("application/json")
				.setBody("[{\"id\":1,\"name\":\"Ana\"},{\"id\":2,\"name\":\"Carlos\"}]");
			else
			{
				std::string full_path = server.root;
				if (!full_path.empty() && full_path.back() != '/')
					full_path += '/';
				std::string path_error = get_error_page(server.error_page, "404");
				full_path += path_error;
				rb.setStatus(404)
				.setBodyFile(full_path);
			}
		}
		else if (par.getMethod() == "POST") { //ex bash: curl -X POST http://127.0.0.1:8080/upload/test.txt -d "Hola desde C++ webserv"
			rb.setStatus(201)
			.setContent("application/json")
			.setBody("{\"message\":\"User created\"}");
		}
		else if (par.getMethod() == "DELETE") {
			rb.setStatus(200)
			.setContent("application/json")
			.setBody("{\"message\":\"User deleted\"}");
		}
		else {
			rb.setStatus(405)
			.setBody("Method Not Allowed")
			.setContent("text/plain");
		}
		HttpResponse response_fn = rb.build();
		std::string response_str = response_fn.toString();
		int sent_bytes = send(poll_fds[i].fd, response_str.c_str(), response_str.size(), 0); //hasta aqui
		if (sent_bytes > 0)
			close_pollfd(poll_fds, i);
		else
			std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
		request_str.clear();
	}
	return (0);
}

void	connect_to_clients(std::vector<struct pollfd> &poll_fds, std::vector<Socket *> &sockets,
		std::map<int, std::string> &client_requests, Config &config) {
	
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
				int process_status = process_request(poll_fds, client_requests, i, config);
				if (process_status)
					continue ;
			}
		}
	}
}
