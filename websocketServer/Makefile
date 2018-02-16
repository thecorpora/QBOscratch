.PHONY:clean

all:websocket_server

#example-server.o: example-server.c
#	gcc -c -o example-server.o example-server.c

websocket_server:websocket_server.c
	gcc websocket_server.c -o websocket_server -lwebsockets -lpthread

clean:
	-rm websocket_server
