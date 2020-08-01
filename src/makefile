target := ECPlugin
SRC_DIR := .
LIB_DIR := ./lib
OUT_DIR := ../bin


ECPlugin : main.o
	g++ ./*.o -o $(OUT_DIR)/ECPlugin
main.o : Args.o ECModel.o Executer.o XMLParser.o FindNewDir.o
	g++ -c $(SRC_DIR)/main.cpp
Args.o : ECModel.o
	g++ -c $(SRC_DIR)/Args.hpp $(SRC_DIR)/Args.cpp
ECModel.o :
	g++ -c $(SRC_DIR)/ECModel.cpp $(SRC_DIR)/ECModel.hpp
Executer.o :
	g++ -c $(SRC_DIR)/Executer.cpp $(SRC_DIR)/Executer.hpp
XMLParser.o : TinyXML.o ECModel.o
	g++ -c $(SRC_DIR)/XMLParser.cpp $(SRC_DIR)/XMLParser.hpp
TinyXML.o : 
	g++ -c $(LIB_DIR)/tinyxml2.h $(LIB_DIR)/tinyxml2.cpp
FindNewDir.o : Executer.o
	g++ -c $(SRC_DIR)/FindNewDir.hpp

all : Args.o ECModel.o Executer.o XMLPaser.o FindNewDir.o
	g++ -c $(SRC_DIR)/main.cpp

install : main.o
	g++ ./*.o -o $(OUT_DIR)/ECPlugin
	cp $(OUT_DIR)/ECPlugin /usr/bin
clean:
	rm -f ./*.o
	rm -f $(SRC_DIR)/*.gch
	rm -f $(LIB_DIR)/*.gch
	rm -f $(OUT_DIR)/*