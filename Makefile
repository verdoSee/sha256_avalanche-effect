all: compile link

compile:
	g++ -c main.cpp -std=c++20

link:
	g++ main.o -o main -lcrypto
