COMPILER=clang++ -g -std=c++11 -Wall
ROOT=$(shell pwd)
SRC=src
BIN=bin

SERVER_EXEC=server
CLIENT_EXEC=client
PORT=9000

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SRC)/server_main.cpp $(BIN)/server.o $(BIN)/message.o $(BIN)/user_list.o $(BIN)/util.o
	$(COMPILER) -o $(SERVER_EXEC) $(SRC)/server_main.cpp $(BIN)/server.o $(BIN)/message.o $(BIN)/util.o $(BIN)/user_list.o -lpthread

$(CLIENT_EXEC): $(SRC)/client_main.cpp $(BIN)/client.o $(BIN)/message.o $(BIN)/util.o
	$(COMPILER) -o $(CLIENT_EXEC) $(SRC)/client_main.cpp $(BIN)/client.o $(BIN)/message.o $(BIN)/util.o -lpthread

$(BIN)/server.o: $(SRC)/server.cpp $(SRC)/server.h
	$(COMPILER) -o $(BIN)/server.o -c $(SRC)/server.cpp

$(BIN)/client.o: $(SRC)/client.cpp $(SRC)/client.h
	$(COMPILER) -o $(BIN)/client.o -c $(SRC)/client.cpp

$(BIN)/message.o: $(SRC)/message.cpp $(SRC)/message.h
	$(COMPILER) -o $(BIN)/message.o -c $(SRC)/message.cpp

$(BIN)/user_list.o: $(SRC)/user_list.cpp $(SRC)/user_list.h
	$(COMPILER) -o $(BIN)/user_list.o -c $(SRC)/user_list.cpp

$(BIN)/util.o: $(SRC)/util.cpp $(SRC)/util.h
	$(COMPILER) -o $(BIN)/util.o -c $(SRC)/util.cpp

clean:
	rm -rf $(BIN)/*.o
	-rm server
	-rm client
