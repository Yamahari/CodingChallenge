CodingChallenge: main.o
	g++ -std=c++17 -Wall -O2 -o CodingChallenge main.o 

main.o: main.cpp
	g++ -std=c++17 -Wall -O2 -c main.cpp

clean:
	rm *.o CodingChallenge
