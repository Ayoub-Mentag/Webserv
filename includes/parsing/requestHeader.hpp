#pragma once 

# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <signal.h>
# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <sstream>



class Request {
	protected :
		std::map<std::string, std::string>	head;
	public :
		virtual ~Request();
		Request();
		virtual void                        parseBody(std::string body) = 0;
		void                                setHead( std::map<std::string, std::string>  head);
		std::map<std::string, std::string>	getHead() const;

};

class BoundaryRequest : public Request {
	private :
		// the body will conteain the header elements 
		// and the entityPost
		std::vector<std::map<std::string, std::string> > body;
		std::string                         boundary;

	public :
		BoundaryRequest();
		void                                                parseBody(std::string body);
		void                                                setBoundary(std::string boundary);
		void                                                setBody(std::vector<std::map<std::string, std::string> >);
};

class SimpleRequest : public Request {
	private :
		std::string entityPost;
	public :
		std::string											getEntityPost() const;
		void												setEntityPost(std::string entityPost);
		SimpleRequest();
		void												parseBody(std::string body);
};

class ChunkedRequest : public Request {
	private :
		std::string entityPost;
	public :
		ChunkedRequest();
		void												parseBody(std::string body);
		std::string											getEntityPost() const;
		void												setEntityPost();
};

void	checkPath(std::string path);
