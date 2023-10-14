#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <stack>





int main() {
	// std::string buffer = (
	// 	"POST /upload-endpoint HTTP/1.1\r\nHost: localhost:8080\r\nContent-Type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n\r\n"

	// 	"----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
	// 	"Content-Disposition: form-data; name=\"file\"; filename=\"example.txt\"\r\n\r\n"
	// 	"Content-Type: text/plain\r\n"
	// 	"File content goes here\r\n"
	// 	"----WebKitFormBoundary7MA4YWxkTrZu0gW\r\n"
	// );

	std::string buffer = "test                    test\n";
	checkRequest(buffer);
    std::cout << buffer;
	return 0;
}