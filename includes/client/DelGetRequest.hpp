#pragma once

#include <Request.hpp>

class DelGetRequest : public Request {
	public :
		DelGetRequest();
		DelGetRequest(std::map<std::string, std::string>);
};
