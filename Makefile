run:
	gcc -c -g c-automatic-sql.c -o main.o
	gcc -c -g c.c -o functions.o
	gcc main.o functions.o -lpq -o main.exe
	./main.exe

clean:
	rm -rf *.o *.exe

all: run clean