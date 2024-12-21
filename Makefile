all: build run

build:
	mkdir -p ./bin
	gcc ./src/main.c -o ./bin/main

run: build
	./bin/main
