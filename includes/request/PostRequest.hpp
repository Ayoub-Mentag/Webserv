#pragma once 

#include <Request.hpp>

class PostRequest : public Request {
	private :
		std::string	body;
	public :
		PostRequest();
		PostRequest(int parsingType);
		const std::string&			getBody() const;
		void						setBody(std::string body);
};