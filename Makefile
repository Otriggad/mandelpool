#################### 
GROUP_NUMBER := 20
####################

# compiler
CC = gcc
# flags
CFLAGS = -Wall -Wextra -Wshadow -Wcast-qual -pedantic -ggdb -std=c99 -Ofast
# libs
LIBS = -lpthread -lrt -lm

.PHONY: doc clean

all:	main

main:  mandelbrot.o threadpool.o
	g++ src/sfml.cpp bin/mandelbrot.o bin/fifo.o bin/threadpool.o bin/colorpalette.o -o bin/mandelpool -ggdb -lsfml-system -lsfml-window -lsfml-graphics -lpthread

start:
	bin/mandelpool

prototype: mandelbrot.o threadpool.o 
	$(CC) $(CFLAGS) src/prototype.c bin/colorpalette.o bin/fifo.o bin/threadpool.o bin/mandelbrot.o -o bin/prototype $(LIBS)

mandelbrot.o: colorpalette.o
	$(CC) $(CFLAGS) -c -o bin/mandelbrot.o src/mandelbrot.c

colorpalette.o:
	$(CC) $(CFLAGS) -c -o bin/colorpalette.o src/colorpalette.c

threadpool.o: fifo.o
	$(CC) $(CFLAGS) -c -o bin/threadpool.o src/threadpool.c

fifo.o:
	$(CC) $(CFLAGS) -c -o bin/fifo.o src/fifo.c

# threadpool + mandelbrot test
timenopool: threadpool.o mandelbrot_nopool.o
	$(CC) -std=gnu99 src/time_nopool.c src/colorpalette.c bin/fifo.o bin/threadpool.o src/mandelbrot_nopool.o -o bin/timenopool $(LIBS)

timepool: threadpool.o mandelbrot.o colorpalette.o
	$(CC) -std=gnu99 src/time_pool.c src/colorpalette.c bin/fifo.o bin/threadpool.o bin/mandelbrot.o -o bin/timepool $(LIBS)

mandelbrot_nopool.o: colorpalette.o
	$(CC) $(CFLAGS) -c -o src/mandelbrot_nopool.o src/mandelbrot_nopool.c

# threadpool spin-test
performance: threadpool.o
	$(CC) -Wall -Wextra -Wshadow -Wcast-qual -pedantic -ggdb -std=gnu99 -O0 src/poolvsthread.c bin/threadpool.o bin/fifo.o -o bin/performance $(LIBS)

performance1: threadpool.o
	$(CC) -Wall -Wextra -Wshadow -Wcast-qual -pedantic -ggdb -std=gnu99 -O0 src/poolvsthread1run.c bin/threadpool.o bin/fifo.o -o bin/performance1 $(LIBS)

# archive
archive: clean
	tar -zcvf ../mandelpool.tar.gz ../mandelpool/ --exclude=.git/* --exclude=.git

# documentation
doc:
	doxygen ./Doxyfile

# check for race conditions
valgrind_race:
	valgrind --tool=helgrind bin/prototype

# check for leaks
valgrind_leak:
	valgrind --leak-check=full bin/prototype

# Test with minunit
test: testfifo testthreadpool

testfifo: clean
	$(CC) tests/test_fifo.c src/fifo.c -lrt -lm -o bin/test_fifo
	./bin/test_fifo

testthreadpool: clean fifo.o threadpool.o
	$(CC) tests/test_threadpool.c bin/fifo.o bin/threadpool.o -std=c99 -lrt -lm -o bin/test_threadpool $(LIBS)
	./bin/test_threadpool

# utils
clean:
	rm -f src/*.o
	rm -f bin/*
	rm -f mandelpool.tar.gz
	rm -rf doc/html/*

# Code formatting
astyle:
	astyle -A3 src/*.c src/*.h tests/*.c tests/*.h
