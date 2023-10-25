#pragma once 

#include <Request.hpp>

# define NONE		0
# define SIMPLE		1
# define BOUNDARY	2
# define CHUNKED	3

class PostRequest : public Request {
	private :
		std::string	body;
        int			parsingType;
		std::string	content;

	public :
		PostRequest();
		PostRequest(int parsingType);
		const std::string&	getBody() const;
		void				setBody(std::string body);
        void                setParsingType(int& parsingType);
        const int&			getParsingType() const;
		void				parseBody(std::string body);
};