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
	public :

    	int         serverIndex;
    	int         locationIndex;
		virtual ~Request();
		Request();
		Request(std::map<std::string, std::string> head);
	
		void									setHead( std::map<std::string, std::string>  head);
		std::map<std::string, std::string>&		getHead();
		void									setTypeOfRequest(REQUEST_TYPE typeOfRequest);

		// ADDING TO MAP
		void									addToHead(std::string key, std::string value);
		std::string&							getValueByKey(std::string key);


		// TODO: Add a function to add to head map
};