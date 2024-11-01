CFLAGS = -std=c99 -g -Wall #flags para compilacao
objects = arquivo.o controle.o  gerais.o listaConteudos.o main.o
LDLIBS = -lm -g #libs de ligacao
CC = gcc

all: vina++

vina++: $(objects)
	$(CC) $(objects) -g $(LDLIBS) -o vina++

main.o: main.c arquivo.h controle.h gerais.h listaConteudos.h
	$(CC) -c main.c -g $(CFLAGS)
	
gerais.o: gerais.c gerais.h
	$(CC) -c gerais.c $(CFLAGS)

arquivo.o: arquivo.c gerais.h arquivo.h
	$(CC) -c arquivo.c -g $(CFLAGS)

controle.o: controle.c gerais.h controle.h
	$(CC) -c controle.c $(CFLAGS)

listaConteudos.o: listaConteudos.c gerais.h listaConteudos.h
	$(CC) -c listaConteudos.c $(CFLAGS)

clean:$(objects)
	rm -f $(objects) *~

purge: clean
	-rm -f vina++