server : 
	cc server.c -o server
	./server

client : 
	cc client.c -o client
	./client

fclean : 
	rm -f client server