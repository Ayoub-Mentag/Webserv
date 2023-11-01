#pragma once 

#include <Request.hpp>

class PostRequest : public Request {
	private :
		std::string	body;
	public :
		PostRequest();
		PostRequest(std::map<std::string, std::string> head, std::string body);
		const std::string&			getBody() const;
		void						setBody(std::string body);
};