#pragma once 

#include <Request.hpp>

# define NONE				0
# define BOUNDARY			1
# define CHUNKED			2
# define BOUNDARYCHUNKED	3

typedef struct s_data {
	std::map<std::string, std::string>	head;
	std::string content;
} t_data;

class PostRequest : public Request {
	private :
		std::string	body;
        int			parsingType;
		std::string	content;
		std::vector<s_data> data;
	public :
		PostRequest();
		PostRequest(int parsingType);
		const std::string&			getBody() const;
		void						setBody(std::string body);
        void                		setParsingType(int& parsingType);
        const int&					getParsingType() const;
		void						parseBody(std::string body);
		const std::vector<t_data>&	getData() const;

        void                		setDataByValues(std::map<std::string, std::string> head, std::string content);
		t_data&						getDataByIndex(size_t index);
		void						setContent(std::string content, size_t index);

};