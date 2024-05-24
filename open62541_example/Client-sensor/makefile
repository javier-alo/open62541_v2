compile: clean ../open62541/obj/open62541.o obj/myClient_sensor.o
	mkdir bin
	gcc ../open62541/obj/open62541.o obj/myClient_sensor.o -o bin/myClient_sensor

../open62541/obj/open62541.o: ../open62541/open62541.c
	gcc -c -std=c99 ../open62541/open62541.c -o ../open62541/obj/open62541.o

obj/myClient_sensor.o: src/myClient_sensor.c
	mkdir -p obj
	gcc -c src/myClient_sensor.c -I ../open62541 -o obj/myClient_sensor.o

clean:
	rm -rf obj bin

run: compile
	./bin/myClient_sensor