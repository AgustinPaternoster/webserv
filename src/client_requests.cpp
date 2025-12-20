/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_requests.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: camurill <camurill@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:55:44 by yrodrigu          #+#    #+#             */
/*   Updated: 2025/12/20 13:04:15 by yrodrigu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Response.hpp"
#include "HTTPRequestParser.hpp"
#include "HTTPResponse.hpp"
#include "HttpUtils.hpp"
#include "Cgi.hpp"
#include "CgiTasks.hpp"

void cleanupTask(std::vector<struct pollfd> &poll_fds, CgiTask &cgiJobs, int current_fd, int client_fd)
{
    if (current_fd != -1)
        close(current_fd);
    if (client_fd != -1)
        close(client_fd);

    for (size_t j = 0; j < poll_fds.size(); j++)
    {
        if (poll_fds[j].fd == client_fd || poll_fds[j].fd == current_fd)
        {
            poll_fds[j].fd = -1;
            poll_fds[j].revents = 0;
        }
    }
    cgiJobs.removeCgiTask(current_fd);

    std::cout << "[CGI Cleanup] Tarea finalizada. FDs " << current_fd 
              << " y " << client_fd << " cerrados." << std::endl;
}


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
	if(status == 403)
		respon = response.generateError(status, server, "403 Forbidden: Permission denied");
	return(respon);
}


void handle_cgi_read(std::vector<struct pollfd> &poll_fds, CgiTask &cgiJobs, size_t &i)
{
	HttpResponse response;
    int current_fd = poll_fds[i].fd;
    short revents = poll_fds[i].revents;
    t_cgi_job &cgi_task = cgiJobs.getCgiTask(current_fd);
    int client_fd = cgi_task.client_fd;

    if (revents & POLLIN)
    {
        char buffer[4096];
        ssize_t bytes_read = read(current_fd, buffer, sizeof(buffer));

        if (bytes_read > 0)
        {
            cgi_task.cgi_output_buffer.append(buffer, bytes_read);
            if (!cgi_task.header_parsed && cgi_task.cgi_output_buffer.find("\r\n\r\n") != std::string::npos)
                cgi_task.header_parsed = true;
            return; 
        }
        else if (bytes_read == 0)
        {
            int status;
            waitpid(cgi_task.pid, &status, 0);
            
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			{
				std::cerr << "⚠️ El script terminó con error code: " << WEXITSTATUS(status) << std::endl;
        		std::string respos = response.generateError(500, cgi_task.server, "CGI Execution Error");
        		send(client_fd, respos.c_str(), respos.size(), 0);
			}
            else if(!cgi_task.header_parsed)
			{
				std::cerr << "⚠️ El script terminó sin enviar cabeceras válidas." << std::endl;
        		std::string respos = response.generateError(500, cgi_task.server, "Invalid CGI Response");
        		send(client_fd, respos.c_str(), respos.size(), 0);
            }
			else
				cgiJobs.sendResponse(cgi_task);


            cleanupTask(poll_fds, cgiJobs,  current_fd, client_fd);
            return;
        }
        else 
        {
            std::string respos = response.generateError(500, cgi_task.server, "Internal Server Error");
            send(client_fd, respos.c_str(), respos.size(), 0);
            cleanupTask(poll_fds, cgiJobs,  current_fd, client_fd);
            return;
        }
    }
	if (revents & (POLLERR | POLLHUP | POLLNVAL))
	{
		int status;
		waitpid(cgi_task.pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) 
        {
            std::cerr << "[CGI] Crash detectado (Exit " << WEXITSTATUS(status) << "). Enviando 500." << std::endl;
            std::string respos = response.generateError(500, cgi_task.server, "CGI Script Crash");
            send(client_fd, respos.c_str(), respos.size(), 0);
        }
		else if (WIFSIGNALED(status))
        {
            std::cerr << "[CGI] Proceso muerto por señal. Enviando 500." << std::endl;
            std::string respos = response.generateError(500, cgi_task.server, "CGI Terminated by Signal");
            send(client_fd, respos.c_str(), respos.size(), 0);
        }
		else if (!cgi_task.header_parsed)
        {
            std::cerr << "[CGI] Respuesta incompleta. Enviando 500." << std::endl;
            std::string respos = response.generateError(500, cgi_task.server, "Invalid CGI Headers");
            send(client_fd, respos.c_str(), respos.size(), 0);
        }
		else 
        {
            std::cout << "[CGI] Éxito. Enviando respuesta acumulada." << std::endl;
            cgiJobs.sendResponse(cgi_task);
        }
		cleanupTask(poll_fds, cgiJobs,  current_fd, client_fd);
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
			std::cout << "Client closed connection gracefully.\n";
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
			if (par.getFlag() == -1)
				res_response =response.generateError(400, server, "Bad Request");
			else if (par.getFlag() == -2)
				res_response =response.generateError(501, server, "Method not implemented");
			else if(!server.locations.empty() && !server.locations[0].cgi_extension.first.empty())
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
		
		if (poll_fds[i].revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL )) {
			
			if (is_listening_socket(poll_fds[i].fd, sockets)) {
				
				int client_fd = sockets[i]->accepting();
				std::cout << "\e[0;92mNew fd created: client fd = " << client_fd << "\033[0m" <<std::endl;
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
				std::cout << "\e[0;92mClient fd : " << poll_fds[i].fd 
					<< "\033[0m" << " sending data..." << std::endl;	
				int process_status = process_request(poll_fds, client_requests, i, config);
				if (process_status)
					continue ;
				}
		
		}
	}
	clean_poll_fd(poll_fds);
}
