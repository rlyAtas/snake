CC=gcc
FLAGS=-Wall -Werror -Wextra

all: build

build:
	$(CC) $(FLAGS) snake.c -o ../build/Snake

clean:
	rm -f ../build/Snake

rebuild: clean build

zip:
	rm -f snake.zip
	zip snake.zip *
