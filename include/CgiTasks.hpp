#ifndef __CGITASK_H__
#define __CGITASK_H__
#include "Common.hpp"

class CgiTask
{
    public:
        CgiTask(void);
        CgiTask(const CgiTask& other);
        ~CgiTask(void);

        CgiTask& operator=(const CgiTask& other);
        void AddNewCgiTask(int pipe, t_cgi_job task);
        bool isCgiReadFd(int fd);
        void removeCgiTask(int fd);
        t_cgi_job& getCgiTask(int current_fd);
        void sendResponse(t_cgi_job& task);
    private:
        std::map<int , t_cgi_job> _cgiJobs; 
        //first int  is for the reading pipe when
        //client send the message.
        //fd client inside t_cgi_job
};

#endif