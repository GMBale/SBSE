tsp : bin/main.o
	g++ -o symbreg bin/main.o

bin/main.o : src/main.cpp
	g++ -c -o bin/main.o src/main.cpp

clean :
	rm bin/*.o
	rm symbreg
