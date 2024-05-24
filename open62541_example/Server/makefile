compile: clean ../open62541/obj/open62541.o obj/myServer.o
	mkdir bin
	gcc ../open62541/obj/open62541.o obj/myServer.o -o bin/myServer

../open62541/obj/open62541.o: ../open62541/open62541.c
	gcc -c -std=c99 ../open62541/open62541.c -o ../open62541/obj/open62541.o

obj/myServer.o: src/myServer.c
	mkdir obj
	gcc -c src/myServer.c -I ../open62541 -o obj/myServer.o

clean:
	rm -rf obj bin

run: compile
	./bin/myServer
