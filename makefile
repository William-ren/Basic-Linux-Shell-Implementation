ashell:ashell.o
	gcc -o ashell ashell.o
ahsell.o:ashell.c
	gcc -c ashell.c	
clean:
	rm ashell.o
