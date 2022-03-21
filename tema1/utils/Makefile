header_files = list.o hashmap.o
test_file = test.c
all:build run clean

build: list.o hashmap.o
	$(CC) $(header_files) $(test_file) -o test 
list.o:
	$(CC) -c list.c -o list.o
hashmap.o:
	$(CC) -c hashmap.c -o hashmap.o
run: 
	./test
clean:
	rm hashmap.o
	rm list.o
	rm test

