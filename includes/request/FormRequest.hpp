#pragma once 

#include <Request.hpp>

class FormRequest : public Request {
	private :
		std::string body;
	public :
		std::string			getBody() const;
		void				setBody(std::string body);
		FormRequest();
		FormRequest(REQUEST_TYPE type);
		void				parseBody(std::string body);
};
