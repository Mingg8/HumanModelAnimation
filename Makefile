LIBS = -lGL -lGLU -lglut

all : main.o camera.o node.o joint.o tree.o
	g++ -std=c++14 main.o camera.o node.o joint.o tree.o $(LIBS) -o main

main.o : src/main.cpp include/camera.h
	g++ -std=c++14 -c -g src/main.cpp $(LIBS) -o main.o

camera.o : include/camera.h
	g++ -std=c++14 -c -g src/camera.cpp $(LIBS) -o camera.o

node.o : include/node.h
	g++ -std=c++14 -I /usr/local/include/ -c -g src/node.cpp $(LIBS) -o node.o

joint.o : include/joint.h
	g++ -std=c++14 -I /usr/local/include/ -c -g src/joint.cpp $(LIBS) -o joint.o

tree.o : include/tree.h
	g++ -std=c++14 -I /usr/local/include/ -c -g src/tree.cpp $(LIBS) -o tree.o
clean :
	rm *.o main


