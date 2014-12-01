COMPILER=g++ -g -std=c++11 -Wall
ROOT=$(shell pwd)
SRC=src
BIN=bin

all: server client

server: $(SRC)/server_main.cpp server.o message.o util.o
	$(COMPILER) -o server $(SRC)/server_main.cpp server.o message.o util.o -lpthread

client: $(SRC)/client_main.cpp client.o message.o util.o
	$(COMPILER) -o client $(SRC)/client_main.cpp client.o message.o util.o -lpthread

server.o: $(SRC)/server.cpp $(SRC)/server.h
	$(COMPILER) -c $(SRC)/server.cpp

client.o: $(SRC)/client.cpp $(SRC)/client.h
	$(COMPILER) -c $(SRC)/client.cpp

message.o: $(SRC)/message.cpp $(SRC)/message.h
	$(COMPILER) -c $(SRC)/message.cpp

util.o: $(SRC)/util.cpp $(SRC)/util.h
	$(COMPILER) -c $(SRC)/util.cpp

clean:
	rm -rf *.o
	rm server
	rm client
