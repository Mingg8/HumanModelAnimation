LIBS = -lGL -lGLU -lglut

all : main.o camera.o node.o
	g++ -std=c++11 main.o camera.o $(LIBS) -o main

main.o : src/main.cpp include/camera.h include/node.h
	g++ -std=c++11 -c -g src/main.cpp $(LIBS) -o out/main.o

camera.o : include/camera.h
	g++ -std=c++11 -c -g src/camera.cpp $(LIBS) -o out/camera.o

node.o : include/node.h
	g++ -std=c++11 -c -g src/node.cpp $(LIBS) -o out/camera.o

clean :
	rm *.o main


