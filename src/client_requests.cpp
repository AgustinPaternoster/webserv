/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apaterno <apaterno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:55:44 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/12/17 15:57:32 by apaterno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "HTTPResponse.hpp"
#include "HttpUtils.hpp"
#include "Cgi.hpp"
#include "CgiTasks.hpp"


std::string manageMethodError(int status, HttpResponse &response, t_server &server)
{
	std::string respon;
	if(status == 404)
		respon = response.generateError(status,server, "File not found" );
	if(status == 405 )
		respon = response.generateError(status,server, "Method Not Allowed" );
	if(status == 500)
		respon = response.generateError(status,server, "Internal Server Error");
	if(status == 501)
		respon = response.generateError(status,server, "Method not implemented");
	return(respon);
}

void handle_cgi_read(std::vector<struct pollfd> &poll_fds, CgiTask &cgiJobs, size_t &i)
{

	HttpResponse response;
	int current_fd = poll_fds[i].fd;
	t_cgi_job &cgi_task = cgiJobs.getCgiTask(current_fd);
	char buffer[4096];
	ssize_t bytes_read;

	bytes_read = read(current_fd, buffer, sizeof(buffer));

	if (bytes_read > 0)
	{
		cgi_task.cgi_output_buffer.append(buffer, bytes_read);
		if (!cgi_task.header_parsed)
		{
			size_t header_end_pos = cgi_task.cgi_output_buffer.find("\r\n\r\n");
			if (header_end_pos != std::string::npos)
				cgi_task.header_parsed = true;
		}
		
	}
	else if (bytes_read == 0)
	{
		int status;
		waitpid(cgi_task.pid, &status, WNOHANG);
		close(current_fd);
		if ((WIFEXITED(status) && WEXITSTATUS(status) != 0))
        {
            std::cerr << "[CGI ERROR] El proceso hijo falló o envió salida inválida." << std::endl;
            std::string respos = response.generateError(500, cgi_task.server,"Internal Server Error");
			int sent_bytes = send(cgi_task.client_fd , respos.c_str(), respos.size(), 0);
			if  (sent_bytes < 0)
				std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
        }
		else
			cgiJobs.sendResponse(cgi_task);
		int client_fd =  cgi_task.client_fd;
		close(client_fd);
		cgiJobs.removeCgiTask(current_fd);
		poll_fds[i].fd = -1;
		for (size_t j = 0; j < poll_fds.size(); j++) {
            if (poll_fds[j].fd == client_fd) {
                poll_fds[j].fd = -1;
                break;
            }
        }
		return;
	}
	else if (bytes_read == -1)
	{	
		if (errno != EAGAIN && errno != EWOULDBLOCK)
		{
			std::cerr << "[CGI ERROR] Error crítico de lectura en pipe FD " << current_fd 
					<< ": " << strerror(errno) << std::endl;
			std::string respos = response.generateError(500, cgi_task.server,"Internal Server Error");
			int sent_bytes = send(cgi_task.client_fd , respos.c_str(), respos.size(), 0);
			if  (sent_bytes < 0)
				std::cerr << "ERROR IN SEND: " << strerror(errno) << std::endl;
			int status;
			waitpid(cgi_task.pid, &status, WNOHANG);
			close(current_fd);
			int client_fd = cgi_task.client_fd;
			cgiJobs.removeCgiTask(current_fd);
			for (size_t j = 0; j < poll_fds.size(); j++) {
				if (poll_fds[j].fd == client_fd) {
					poll_fds[j].fd = -1;
					break;
				}
			}
		}
	}
}

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
	std::string res_response;
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
		size_t content_len =getContentLength(request_str);
		size_t total_body = request_str.length() - body_start;
		if (total_body < content_len)
		return 0;
		try
		{
			HttpRequest par = HttpRequest::fromString(request_str);
			HttpResponse response;
			t_server server = config.locationRouter(getServerPort(poll_fds[i].fd), par.getUri());
			if(!server.locations[0].cgi_extension.first.empty())
			{
				
				Cgi httpcgi(par,poll_fds, i, server );
				int status =httpcgi.parseRequestToEnv();
				if(status != 200)
				{
					res_response = response.generateError(status, server, "File not found");
				}
				else
				{
					status = httpcgi.CgiHandler(config.CgiJobs);
					if(status == 200)
					{
						client_requests.erase(poll_fds[i].fd);
						return(0);
					}
					res_response = manageMethodError(status,response,server);
				}
				client_requests.erase(poll_fds[i].fd);
			}
			else
				res_response = response.execute_response(par,server );


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
			
		CgiTask &cgijobs = config.CgiJobs;

		for (size_t i = 0; i < poll_fds.size(); i++) {
		
		if (poll_fds[i].revents & (POLLIN | POLLHUP )) {
			
			if (is_listening_socket(poll_fds[i].fd, sockets)) {
				
				int client_fd = sockets[i]->accepting();
				std::cout << "\e[0;92mNew CLient fd is created: " << client_fd << "\e[0m" << std::endl;
				struct pollfd new_polls;
				new_polls.fd = client_fd;
				new_polls.events = POLLIN;
				new_polls.revents = 0;
				poll_fds.push_back(new_polls);
			}
			else if(cgijobs.isCgiReadFd(poll_fds[i].fd)) {
				handle_cgi_read(poll_fds, cgijobs, i);
			}
			else {
				std::cout << "\e[0;92mclient able to send data " << poll_fds[i].fd << "\e[0m"  << std::endl;
				int process_status = process_request(poll_fds, client_requests, i, config);
				if (process_status)
					continue ;
				}
		
		}
	}
	clean_poll_fd(poll_fds);
}
