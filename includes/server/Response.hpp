#pragma once


class Response {
	private:
		std::string					contentType; // mime type
		std::string					httpVersion;
		std::map<int, std::string>	statusCode;
		std::string					header;
		std::string					body;
		std::string					response;
	public:
		Response();
		~Response();

		const std::string&			getHeader();
		const std::string&			getBody();
		const std::string& 			getStatusCode(int status);
		const std::string&			getHttpVersion();
		const std::string&			getContentType();
		std::string					getBodylength();
		const std::string&			getResponse();

		void						setHeader(std::string& _header);
		void						setHeader(int status);
		void						setBody(const std::string& body);
		void			 			setStatusCode();
		void						setHttpVersion(const std::string&);
		void						setContentType(const std::string& extention);
		void						setResponse(std::string& resp);
		void			 			setResponse();
};