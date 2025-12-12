#include "CgiTasks.hpp"

CgiTask::CgiTask(void){}

CgiTask::~CgiTask(void){}

void CgiTask::AddNewCgiTask(int pipe, t_cgi_job task)
{
    std::pair<int , t_cgi_job> new_par(pipe , task);
    _cgiJobs.insert(new_par);
    
}