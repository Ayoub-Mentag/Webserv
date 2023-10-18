#pragma once 

#include <Request.hpp>

class SimpleRequest : public Request {
	public :
		SimpleRequest(REQUEST_TYPE type);
		void    parseBody(std::string body);
};
