tsp : bin/main.o
	g++ -o tsp bin/main.o

bin/main.o : src/main.cpp
	g++ -c -o bin/main.o src/main.cpp

clean :
	rm bin/*.o
	rm tsp
