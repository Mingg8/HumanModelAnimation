LIBS = -lGL -lGLU -lglut

all : main.o camera.o node.o joint.o
	g++ -std=c++14 main.o camera.o node.o joint.o $(LIBS) -o main

main.o : src/main.cpp include/camera.h
	g++ -std=c++14 -c -g src/main.cpp $(LIBS) -o main.o

camera.o : include/camera.h
	g++ -std=c++14 -c -g src/camera.cpp $(LIBS) -o camera.o

node.o : include/node.h
	g++ -std=c++14 -I /usr/local/include/ -c -g src/node.cpp $(LIBS) -o node.o
joint.o : include/joint.h
	g++ -std=c++14 -I /usr/local/include/ -c -g src/joint.cpp $(LIBS) -o joint.o
clean :
	rm *.o main


