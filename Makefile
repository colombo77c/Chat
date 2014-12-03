COMPILER=clang++ -g -std=c++11 -Wall
ROOT=$(shell pwd)
SRC=src
BIN=bin

SERVER_EXEC=server
CLIENT_EXEC=client
PORT=9000

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SRC)/server_main.cpp server.o message.o user_list.o util.o
	$(COMPILER) -o $(SERVER_EXEC) $(SRC)/server_main.cpp server.o message.o util.o user_list.o -lpthread

$(CLIENT_EXEC): $(SRC)/client_main.cpp client.o message.o util.o
	$(COMPILER) -o $(CLIENT_EXEC) $(SRC)/client_main.cpp client.o message.o util.o -lpthread

server.o: $(SRC)/server.cpp $(SRC)/server.h
	$(COMPILER) -c $(SRC)/server.cpp

client.o: $(SRC)/client.cpp $(SRC)/client.h
	$(COMPILER) -c $(SRC)/client.cpp

message.o: $(SRC)/message.cpp $(SRC)/message.h
	$(COMPILER) -c $(SRC)/message.cpp

user_list.o: $(SRC)/user_list.cpp $(SRC)/user_list.h
	$(COMPILER) -c $(SRC)/user_list.cpp

util.o: $(SRC)/util.cpp $(SRC)/util.h
	$(COMPILER) -c $(SRC)/util.cpp

clean:
	rm -rf *.o
	-rm server
	-rm client
