build : buffer.c requests.c helpers.c client.c parson-master/parson.c
	gcc buffer.c requests.c helpers.c client.c parson-master/parson.c -o client

run : client
	./client

clean : client
	rm client
