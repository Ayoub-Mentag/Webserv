#include <SimpleRequest.hpp>


SimpleRequest::SimpleRequest(REQUEST_TYPE type) : Request(type) {}

void	SimpleRequest::parseBody(std::string body) {(void)body;}

