#pragma once

#include <Request.hpp>

class SimpleRequest : public Request {
	public :
		SimpleRequest(REQUEST_TYPE requestType);
		void    parseBody(std::string body);
};
