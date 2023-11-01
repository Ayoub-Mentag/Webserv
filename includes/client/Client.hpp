#pragma once
#include <Request.hpp>
#include <DelGetRequest.hpp>
#include <BoundaryRequest.hpp>

class Client {
    private :
        Request*		request;
        const int		fd;
        bool			statusHeadRead;

	public :
		Client(int fd);
		Request	*getRequest();
		void	setRequest(Request *request);
		int		getFd() const;
		bool	getStatusHeadRead();
		void	setStatusHeadRead(bool statusHeadRead);
		void	initRequest();
		bool	headEnds(std::string bufferLine);
};