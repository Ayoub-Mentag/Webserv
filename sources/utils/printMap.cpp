// #include <Utils.hpp>
// #include <Parsing.hpp>
// #include <ChunkedRequest.hpp>
// #include <BoundaryRequest.hpp>
// #include <FormRequest.hpp>

// void	printMap(std::map<std::string, std::string>	m) {
// 	for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end(); it++) {
// 		std::cout << "Key " << it->first << " Value " << it->second << std::endl;
// 	}
// }

// void	printRequest(Request* request) {
// 	REQUEST_TYPE type = request->getTypeOfRequest();
// 	if (type == NONE) {
// 		std::cout << "Unknown type of request " << std::endl;
// 		return ;
// 	}
// 	std::cout << "*************Head\n";
// 	printMap(request->getHead());

// 	std::cout << "\n*************Body\n";
// 	if (type == POST_CHUNKED) {
// 		ChunkedRequest *c = dynamic_cast<ChunkedRequest *>(request);
// 		std::cout << c->getBody() << std::endl;
// 	} else if (type == POST_BOUNDARY)
// 	{
// 		BoundaryRequest *b = dynamic_cast<BoundaryRequest *>(request);
// 		for (size_t i = 0; i < b->getBody().size(); i++) {
// 			printMap(b->getBody()[i]);
// 		}
// 	}
// 	else if (type == POST_SIMPLE) {
// 		FormRequest *s = dynamic_cast<FormRequest *>(request);
// 		std::cout << s->getBody() << std::endl;
// 	}
// }