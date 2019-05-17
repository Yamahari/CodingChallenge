GetInITCondingChallenge: main.o
	g++ -std=c++17 -o GetInITCondingChallenge main.o 

main.o: main.cpp
	g++ -std=c++17 -c main.cpp

clean:
	rm *.o GetInITCondingChallenge
