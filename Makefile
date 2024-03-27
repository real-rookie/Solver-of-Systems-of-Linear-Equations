CC = gcc
C_FLAGS = -c -g -Wall -fopenmp
L_FLAGS = -lm -fopenmp

main: main.o Lab3IO.o
	$(CC) -o $@ $^ $(L_FLAGS)
main.o: main.c
	$(CC) $(C_FLAGS) $<
serial: serial.o Lab3IO.o
	$(CC) -o $@ $^ $(L_FLAGS)
Lab3IO.o: Lab3IO.c
	$(CC) $(C_FLAGS) $<
datagen: datagen.o Lab3IO.o
	$(CC) -o $@ $^ 
datagen.o: datagen.c
	$(CC) $(C_FLAGS) $<
serial.o: serial.c
	$(CC) $(C_FLAGS) $<

run:
	./$(main)
memcheck:
	valgrind --tool=memcheck --leak-check=full ./main
clean:
	rm -f main datagen *.o
