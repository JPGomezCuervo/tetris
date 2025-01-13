tetris: ./main.c
	gcc -Wall -ggdb main.c -o tetris.out -lraylib

clean:
	rm -f ./tetris.out
