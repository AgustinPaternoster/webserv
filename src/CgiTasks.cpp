#include "CgiTasks.hpp"

CgiTask::CgiTask(void){}

CgiTask::~CgiTask(void){}

void CgiTask::AddNewCgiTask(int pipe, t_cgi_job task)
{
    std::pair<int , t_cgi_job> new_par(pipe , task);
    _cgiJobs.insert(new_par);
    
}

bool CgiTask::isCgiReadFd(int fd)
{
    return (_cgiJobs.count(fd) > 0);
}

void CgiTask::removeCgiTask(int fd)
{
    _cgiJobs.erase(fd);
}

t_cgi_job& CgiTask::getCgiTask(int current_fd)
{
    return(_cgiJobs[current_fd]);
}

void CgiTask::sendResponse(t_cgi_job& task)
{
    std::stringstream ss;
    std::string full_cgi_output = task.cgi_output_buffer;
    std::string final_response;

    size_t separator_pos = full_cgi_output.find("\r\n\r\n");
    if (separator_pos == std::string::npos) {
          std::cerr << "[CGI ERROR] Respuesta CGI sin separador de cuerpo.\n";
        return;
    }
    std::string cgi_headers = full_cgi_output.substr(0, separator_pos);
    std::string cgi_body = full_cgi_output.substr(separator_pos + 4);
    size_t body_size = cgi_body.size();
    ss << "HTTP/1.1 200 OK\r\n";
    ss << cgi_headers << "\r\n";
    ss << "Content-Length: " << body_size << "\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    final_response = ss.str() + cgi_body;
    const char* data_to_send = final_response.c_str();
    size_t total_size = final_response.size();
    ssize_t bytes_sent = send(task.client_fd, data_to_send, total_size, 0);
    if (bytes_sent > 0) {
        std::cout << "[CGI] Respuesta HTTP completa enviada a FD " << task.client_fd << " (" << bytes_sent << " bytes)" << std::endl;
    } else {
        std::cerr << "[CGI ERROR] Fallo al enviar/Escritura parcial no manejada en FD " 
                  << task.client_fd << std::endl;
    } 
}

std::vector<int> CgiTask::getAllReadFds(void)
{
    std::vector<int> fds;
    for (std::map<int, t_cgi_job>::iterator it = _cgiJobs.begin(); it != _cgiJobs.end(); ++it) {
        fds.push_back(it->first); 
    }
    return fds;
}
