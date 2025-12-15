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
    
    /// FALTA AGREGAR HEADERS ////
    const char* data_to_send = task.cgi_output_buffer.c_str();
    size_t total_size = task.cgi_output_buffer.size();

    ssize_t bytes_sent = send(task.client_fd, data_to_send, total_size, 0);
    if (bytes_sent > 0) {
            std::cout << "[CGI] Respuesta enviada a FD " << task.client_fd << std::endl;
        } else {
            // manejo de escritura parcial
            std::cerr << "[CGI ERROR] Fallo al enviar/Escritura parcial no manejada en FD " 
                      << task.client_fd << std::endl;
        }
    close(task.client_fd);

    // NOTA: La limpieza del client_fd de poll_fds y client_requests DEBE
        // hacerse externamente por la clase que gestiona el bucle poll()
}
