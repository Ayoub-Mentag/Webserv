#pragma once
# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>

enum REQUEST_TYPE{
	GET,
	DELETE,
	POST_SIMPLE,
	POST_BOUNDARY,
	POST_CHUNKED,
	POST_JSON,
	NONE
};


class Request {
	protected :
		std::map<std::string, std::string>	head;
		REQUEST_TYPE						typeOfRequest;
	public :
		// std::string method;
    	// std::string path;
    	// std::string httpVersion;
    	// std::string serverName;
    	int         serverIndex;
    	int         locationIndex;
    	// int         port;
    	// std::stringContentType;
		virtual ~Request();
		Request();
		Request(REQUEST_TYPE type);
		virtual void						parseBody(std::string body);
		void								setHead( std::map<std::string, std::string>  head);
		std::map<std::string, std::string>&	getHead();
		void								setTypeOfRequest(REQUEST_TYPE typeOfRequest);
		REQUEST_TYPE						getTypeOfRequest() const;
};

class BoundaryRequest : public Request {
	private :
		// the body will conteain the header elements 
		// and the entityPost
		std::vector<std::map<std::string, std::string> > body;
		std::string                         boundary;

	public :
		BoundaryRequest();
		BoundaryRequest(REQUEST_TYPE type);
		void                                                parseBody(std::string body);
		void                                                setBoundary(std::string boundary);
		void                                                setBody(std::vector<std::map<std::string, std::string> >);
		std::vector<std::map<std::string, std::string> >	getBody() const;
};

class SimpleRequest : public Request {
	private :
		std::string entityPost;
	public :
		std::string			getEntityPost() const;
		void				setEntityPost(std::string entityPost);
		SimpleRequest();
		SimpleRequest(REQUEST_TYPE type);
		void				parseBody(std::string body);
};

class ChunkedRequest : public Request {
	private :
		std::string entityPost;
	public :
		ChunkedRequest();
		ChunkedRequest(REQUEST_TYPE type);
		void												parseBody(std::string body);
		std::string											getEntityPost() const;
		void												setEntityPost();
};


