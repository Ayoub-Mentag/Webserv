#include <SimpleRequest.hpp>


SimpleRequest::SimpleRequest(REQUEST_TYPE requestType) : Request(requestType) {}

void	SimpleRequest::parseBody(std::string body) {(void)body;}

