bas: main.c
	gcc main.c -o bas -lncurses

main.o: main.c vector.h
	gcc -c main.c -o main.o

vector.o: vector.c vector.h
	gcc -c vector.c -o vector.o

clean:
	rm -f *.o bas
