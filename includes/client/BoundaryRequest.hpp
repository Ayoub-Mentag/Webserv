#pragma once 
#include <Utils.hpp>
#include <PostRequest.hpp>

class Data {
    public :
        Data();
        ~Data();
        void								setHead(std::map<std::string, std::string> head);
        void								setContent(std::string content);
        const std::map<std::string, std::string>	&getHead() const;
        const std::string							&getContent() const;
    private :
        std::map<std::string, std::string>  head;
        std::string     content;
};

// std::ostream& operator<<(std::ostream& os, const Data& d);

class BoundaryRequest : public PostRequest {
    private :
        std::vector<Data> dataOfBoundaries;
	public :
		BoundaryRequest();
		BoundaryRequest(std::map<std::string, std::string> head);
        void    parseBoundary();

        void                		setDataByValues(std::map<std::string, std::string> head, std::string content);
		void						setContentByIndex(std::string content, size_t index);

    public :
        const std::vector<Data>&	getData() const;
		Data&						getDataByIndex(size_t index);
};