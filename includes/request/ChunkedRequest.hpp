#pragma once 

#include <Request.hpp>

class ChunkedRequest : public Request {
	private :
		std::string body;
	public :
		ChunkedRequest();
		ChunkedRequest(REQUEST_TYPE type);
		void												parseBody(std::string body);
		std::string											getBody() const;
		void												setBody(std::string body);
};
