all: enigma

enigma: main.cpp enigma.h errors.h
	g++ -Wall -g main.cpp -o enigma
