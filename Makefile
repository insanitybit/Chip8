## file Makefile

# The -O2 is fairly important. The rest are really just there because this will be networked at some point.
CXXOBJECTS= %.o
CXX= g++ 
CXXSOURCES= main.cpp CPU.cpp
CXXFLAGS= -std=c++11 -lncurses


Chip8 : main.o CPU.o
	g++  main.o CPU.o -lncurses -o Chip8


main.o : main.cpp 
CPU.o : CPU.cpp
 
clean:
	rm *.o Chip8

## eof Makefile