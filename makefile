default: smallc.tab.o main.o tree.o semantic.o ircode.o machinecode.o
	gcc -o scc smallc.tab.o main.o tree.o semantic.o ircode.o machinecode.o

smallc.tab.o: smallc.c smallc.tab.c
	gcc -g -c -lfl smallc.tab.c

smallc.c: smallc.l 
	flex -o smallc.c smallc.l

smallc.tab.c: smallc.y
	bison -dv smallc.y

main.o: main.c
	gcc -g -c main.c

tree.o: tree.c
	gcc -g -c tree.c

semantic.o: semantic.c
	gcc -g -c semantic.c

ircode.o: ircode.c
	gcc -g -c ircode.c

machinecode.o: machinecode.c
	gcc -g -c machinecode.c
clean:
	rm -f smallc.c smallc.tab.h smallc.tab.c smallc.output *.o
