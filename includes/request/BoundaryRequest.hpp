#pragma once 

#include <Request.hpp>

class BoundaryRequest : public Request {
	private :
		// the body will conteain the header elements 
		// and the entityPost
		std::vector<std::map<std::string, std::string> > body;
		std::string                         boundary;

	public :
		BoundaryRequest();
		BoundaryRequest(REQUEST_TYPE type);
		void                                                parseBody( std::string body);
		void                                                setBoundary(std::string boundary);
		void                                                setBody(std::vector<std::map<std::string, std::string> >);
		std::vector<std::map<std::string, std::string> >&	getBody();
};
