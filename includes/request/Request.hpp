#pragma once

# include <map>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>

enum REQUEST_TYPE{
	NONE,
	GET,
	DELETE,
	POST_SIMPLE,
	POST_BOUNDARY,
	POST_CHUNKED,
	POST_JSON
};

class Request {
	protected :
		std::map<std::string, std::string>	head;
		REQUEST_TYPE						typeOfRequest;
	public :

    	int         serverIndex;
    	int         locationIndex;
		virtual ~Request();
		Request();
		Request(REQUEST_TYPE type);
	
		void									setHead( std::map<std::string, std::string>  head);
		std::map<std::string, std::string>&		getHead();
		void									setTypeOfRequest(REQUEST_TYPE typeOfRequest);
		REQUEST_TYPE							getTypeOfRequest() const;

		// ADDING TO MAP
		void									addToHead(std::string key, std::string value);
		std::string&							getValueByKey(std::string key);


		// TODO: Add a function to add to head map
};
