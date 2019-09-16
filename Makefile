# LIBS = -lGL -lGLU -lglut
LIBS = -framework OpenGL -framework GLUT
all : main.o camera.o
	g++ -std=c++11 main.o camera.o $(LIBS) -o main

main.o : main.cpp camera.h
	g++ -std=c++11 -c -g main.cpp $(LIBS) -o main.o

camera.o : camera.h
	g++ -std=c++11 -c -g camera.cpp $(LIBS) -o camera.o

clean :
	rm *.o main


