#include <BoundaryRequest.hpp>

Data::Data() {}
Data::~Data() {}


void    Data::setHead(std::map<std::string, std::string> head) {
    this->head = head;
}

void    Data::setContent(std::string content) {
    this->content = content;
}

const std::map<std::string, std::string>	&Data::getHead() const {
    return (this->head);
}

const std::string &Data::getContent() const {
    return (this->content);
}

BoundaryRequest::BoundaryRequest()   {}

void    BoundaryRequest::parseBoundary(){}

const std::vector<Data>&	BoundaryRequest::getData() const{
    return (this->dataOfBoundaries);
}

void    BoundaryRequest::setDataByValues(std::map<std::string, std::string> head, std::string content){
    Data d;

    d.setHead(head);
	d.setContent(content);
	this->dataOfBoundaries.push_back(d);
}

Data& BoundaryRequest::getDataByIndex(size_t index){
	return this->dataOfBoundaries[index];
}

void    BoundaryRequest::setContentByIndex(std::string content, size_t index){
	this->getDataByIndex(index).setContent(content);
}


// std::ostream& operator<<(std::ostream& os, const Data& d)
// {
//     std::map<std::string, std::string> m = d.getHead();
//     for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++) {
// 		os << it->first << " " << it->second << std::endl;
// 	}
//     os << d.getContent();
//     return os;
// }