compile: clean ../open62541/obj/open62541.o obj/myClient_actuator.o
	mkdir bin
	gcc ../open62541/obj/open62541.o obj/myClient_actuator.o -o bin/myClient_actuator

../open62541/obj/open62541.o: ../open62541/open62541.c
	gcc -c -std=c99 ../open62541/open62541.c -o ../open62541/obj/open62541.o

obj/myClient_actuator.o: src/myClient_actuator.c
	mkdir -p obj
	gcc -c src/myClient_actuator.c -I ../open62541 -o obj/myClient_actuator.o

clean:
	rm -rf obj bin

run: compile
	./bin/myClient_actuator