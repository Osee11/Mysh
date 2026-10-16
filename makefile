main:shell.c main.c
	gcc shell.c main.c -o main

run: main
	./main