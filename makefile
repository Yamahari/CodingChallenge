GetInITCodingChallenge: main.o
	g++ -std=c++17 -o GetInITCodingChallenge main.o 

main.o: main.cpp
	g++ -std=c++17 -c main.cpp

clean:
	rm *.o GetInITCodingChallenge
