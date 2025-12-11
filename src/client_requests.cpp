/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:55:44 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/12/09 17:10:36 by camurill         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "HTTPResponse.hpp"
#include "HttpUtils.hpp"

size_t getContentLength(const std::string &req) 
{
    size_t pos = req.find("Content-Length:");
    if (pos == std::string::npos) return 0;
    
    size_t start = pos + 15; 
    size_t end = req.find("\r\n", start);
    if (end == std::string::npos) return 0;
    
    std::string len_str = req.substr(start, end - start);
    return std::atoi(len_str.c_str());
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
		client_requests.erase(poll_fds[i].fd);
		return (1);
	}
	
	client_requests[poll_fds[i].fd].append(buffer, bytes);
	std::string	&request_str = client_requests[poll_fds[i].fd];
	size_t end_pos = request_str.find("\r\n\r\n");
	
	if (end_pos != std::string::npos) 
	{
		size_t body_start = end_pos + 4;
		size_t content_len = getContentLength(request_str);
		size_t total_body = request_str.length() - body_start;
		if (total_body < content_len)
			return 0;
		try
		{
			HttpRequest par = HttpRequest::fromString(request_str);
			HttpResponse response;
			t_server server = config.locationRouter(getServerPort(poll_fds[i].fd), par.getUri());
			/// comprobar si la request coincide exactamente con locations
			// eje /methods vs /methosds/
			// en caso contrario error 301

			std::string res_response = response.execute_response(par,server );

			int sent_bytes = send(poll_fds[i].fd, res_response.c_str(), res_response.size(), 0);
			if  (sent_bytes < 0)
				std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
			request_str.clear();

		}
		catch (const std::exception& e)
		{
			std::cerr << "Request parsing failed: " << e.what() << std::endl;
			close_pollfd(poll_fds, i);
		}
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
