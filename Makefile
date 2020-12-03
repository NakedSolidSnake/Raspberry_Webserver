all:
	gcc -g -c -I./src src/webserver.c -o src/webserver.o
	gcc src/webserver.o -o webserver -lpthread