/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nikitadorofeychik <nikitadorofeychik@st    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:55:44 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/11/18 11:22:26 by nikitadorof      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "ResponseBuilder.hpp"
#include "HttpUtils.hpp"

std::string handle_client_request(std::string request_str, Config &config)
{
	RequestParser	parser;
	ResponseBuilder	rb;

	parser.feedData(request_str);
	if (!parser.isComplete())
		rb.setStatus(405); //responder con la response error

	const HttpRequest& par = parser.getRequest();
	
	//esto se elimina
	size_t num = get_port_www(config, par);
	std::cout << request_str;
	const std::vector<t_server> & servers = config.getServers();
	const t_server &server = servers[num];

	//valid_method
	if (isvalidmethod(par, config))
		rb.setStatus(405);

	// GCI
	if (isGCI(par, config))
		return executegci(par, config); // ToDO
	
	//

	//Simplificar		
	if (par.getMethod() == "GET")
		rb.handle_get(par, );
	else if (par.getMethod() == "POST")
		rb.handle_post();
	else if (par.getMethod() == "DELETE")
		rb.handle_delete();
	else
		rb.setStatus(501); // not implemented
	HttpResponse response_fn = rb.build();
	return (response_fn.toString());
}


int	process_request(std::vector<struct pollfd> &poll_fds,
		std::map<int, std::string> &client_requests, size_t &i, Config &config) 
	{
	

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
		std::string response_str = handle_client_request(request_str, config);
		
		int sent_bytes = send(poll_fds[i].fd, response_str.c_str(), response_str.size(), 0); //hasta aqui
		if  (sent_bytes > 0)
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
