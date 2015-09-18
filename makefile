all:
	gcc -c --std=c99 -o  linkedlist.o linkedlist.c
	gcc -c --std=c99 -O0 -W -o main.o main.c
	gcc -o projectTwo --std=c99 -O0 -W -pthread -w main.o linkedlist.o

clean:
	rm projectTwo

cleanall:
	rm linkedlist.o
	rm main.o
	rm projectTwo
