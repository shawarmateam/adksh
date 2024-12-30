all: build

build:
	mkdir -p ./bin
	gcc ./src/main.c -o ./bin/main

run: build
	./bin/main

install: build
	cp ./bin/main /adkbin/adksh
