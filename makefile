all: Worker.cpp Config.cpp main.cpp Worker.h Config.h
	g++ main.cpp Worker.cpp Config.cpp -o TinyTP -I./ -lpthread
