#include <Server.hpp>
#include <Utils.hpp>

Response::Response() {}

Response::~Response() {}

const std::string&	Response::getHeader() {
	return (this->header);
}

const std::string&	Response::getBody() {
	return (this->body);
}

const std::string& 	Response::getStatusCode(int status) {
	return (this->statusCode[status]);
}

const std::string&	Response::getHttpVersion() {
	return (this->httpVersion);
}

const std::string&	Response::getContentType() {
	return (this->contentType);
}

std::string	Response::getBodylength() {
	return (to_string(this->body.length()));
}

void	Response::setHeader(int status) {
	header = getHttpVersion();
	header += getStatusCode(status) + "\r\n";
	header += "Content-type: " + getContentType() + "\r\n";
	header += " Content-length: " + getBodylength();
	header += " \r\n\r\n";
}

void	Response::setBody(const std::string& body) {
	this->body = body;
}

void	Response::setHttpVersion(const std::string& httpVersion) {
	this->httpVersion = httpVersion;
}

void	Response::setResponse() {
	this->response = getHeader() + getBody();
}

const std::string&	Response::getResponse() {
	setResponse();
	return (response);
}

void	Response::setStatusCode() {
	static int i = 0;

	if (!i) {
		statusCode[200] = " 200 OK";
		statusCode[301] = " 301 Moved Permanently";
		statusCode[403] = " 403 Forbidden";
		statusCode[404] = " 404 Not Found";
		statusCode[405] = " 405 Method Not Allowed";
		statusCode[501] = " 501 Not Implemented";
		i = 1;
	}
}

void	Response::setContentType(const std::string& extention) {
	static std::map<std::string, std::string> contentTypeMap;
	static int i = 0;

	if (!i) {
		contentTypeMap[".aac"]		= "audio/aac";
		contentTypeMap[".abw"]		= "application/x-abiword";
		contentTypeMap[".arc"]		= "application/x-freearc";
		contentTypeMap[".avif"]		= "image/avif";
		contentTypeMap[".avi"]		= "video/x-msvideo";
		contentTypeMap[".azw"]		= "application/vnd.amazon.ebook";
		contentTypeMap[".bin"]		= "application/octet-stream";
		contentTypeMap[".bmp"]		= "image/bmp";
		contentTypeMap[".bz"]		= "application/x-bzip";
		contentTypeMap[".bz2"]		= "application/x-bzip2";
		contentTypeMap[".cda"]		= "application/x-cdf";
		contentTypeMap[".csh"]		= "application/x-csh";
		contentTypeMap[".css"]		= "text/css";
		contentTypeMap[".csv"]		= "text/csv";
		contentTypeMap[".doc"]		= "application/msword";
		contentTypeMap[".docx"]		= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		contentTypeMap[".eot"]		= "application/vnd.ms-fontobject";
		contentTypeMap[".epub"]		= "application/epub+zip";
		contentTypeMap[".gz"]		= "application/gzip";
		contentTypeMap[".gif"]		= "image/gif";
		contentTypeMap[".htm"]		= "text/html";
		contentTypeMap[".html"]		= "text/html";
		contentTypeMap[".ico"]		= "image/vnd.microsoft.icon";
		contentTypeMap[".ics"]		= "text/calendar";
		contentTypeMap[".jar"]		= "application/java-archive";
		contentTypeMap[".jpeg"]		= "image/jpeg";
		contentTypeMap[".jpg"]		= "image/jpeg";
		contentTypeMap[".js"]		= "text/javascript";
		contentTypeMap[".json"]		= "application/json";
		contentTypeMap[".jsonld"]	= "application/ld+json";
		contentTypeMap[".midi"]		= "audio/midi";
		contentTypeMap[".mid"]		= "audio/x-midi";
		contentTypeMap[".mjs"]		= "text/javascript";
		contentTypeMap[".mp3"]		= "audio/mpeg";
		contentTypeMap[".mp4"]		= "video/mp4";
		contentTypeMap[".mpeg"]		= "video/mpeg";
		contentTypeMap[".mpkg"]		= "application/vnd.apple.installer+xml";
		contentTypeMap[".odp"]		= "application/vnd.oasis.opendocument.presentation";
		contentTypeMap[".ods"]		= "application/vnd.oasis.opendocument.spreadsheet";
		contentTypeMap[".odt"]		= "application/vnd.oasis.opendocument.text";
		contentTypeMap[".oga"]		= "audio/ogg";
		contentTypeMap[".ogv"]		= "video/ogg";
		contentTypeMap[".ogx"]		= "application/ogg";
		contentTypeMap[".opus"]		= "audio/opus";
		contentTypeMap[".otf"]		= "font/otf";
		contentTypeMap[".png"]		= "image/png";
		contentTypeMap[".pdf"]		= "application/pdf";
		contentTypeMap[".php"]		= "application/x-httpd-php";
		contentTypeMap[".ppt"]		= "application/vnd.ms-powerpoint";
		contentTypeMap[".pptx"]		= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		contentTypeMap[".rar"]		= "application/vnd.rar";
		contentTypeMap[".rtf"]		= "application/rtf";
		contentTypeMap[".sh"]		= "application/x-sh";
		contentTypeMap[".svg"] 		= "image/svg+xml";
		contentTypeMap[".tar"] 		= "application/x-tar";
		contentTypeMap[".tif"] 		= "image/tiff";
		contentTypeMap[".tiff"]		= "image/tiff";
		contentTypeMap[".ts"]		= "video/mp2t";
		contentTypeMap[".ttf"]		= "font/ttf";
		contentTypeMap[".txt"]		= "text/plain";
		contentTypeMap[".vsd"]		= "application/vnd.visio";
		contentTypeMap[".wav"]		= "audio/wav";
		contentTypeMap[".weba"]		= "audio/webm";
		contentTypeMap[".webm"]		= "video/webm";
		contentTypeMap[".webp"]		= "image/webp";
		contentTypeMap[".woff"]		= "font/woff";
		contentTypeMap[".woff2"]	= "font/woff2";
		contentTypeMap[".xhtml"]	= "application/xhtml+xml";
		contentTypeMap[".xls"]		= "application/vnd.ms-excel";
		contentTypeMap[".xlsx"]		= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		contentTypeMap[".xml"]		= "application/xml";
		contentTypeMap[".xul"]		= "application/vnd.mozilla.xul+xml";
		contentTypeMap[".zip"]		= "application/zip";
		contentTypeMap[".3gp"]		= "video/3gpp";
		contentTypeMap[".3g2"]		= "video/3gpp2";
		contentTypeMap[".7z"]		= "application/x-7z-compressed";
		i = 1;
	}
	std::string tmp = contentTypeMap[extention];
	if (tmp.empty()) {
		tmp = "text/plain";
	}
	this->contentType = tmp;
}
