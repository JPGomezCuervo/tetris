tetris: ./main.c
	gcc -Wall -ggdb main.c -o tetris.out -lraylib -lm

clean:
	rm -f ./tetris.out
