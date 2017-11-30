FLAGS=-std=c++14 -O3 -mavx  -msse -msse2 -msse3 -mfpmath=sse

all: cypher_test

cypher_test: main.o kuznechik.o
	g++ main.o kuznechik.o -o cypher_test

main.o: main.cpp
	g++ $(FLAGS) main.cpp -c

kuznechik.o: kuznechik.cpp
	g++ $(FLAGS) kuznechik.cpp -c

clean:
	rm -rf *.o cypher_test