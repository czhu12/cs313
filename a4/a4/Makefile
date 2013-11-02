CFLAGS = -g

all: cache-ref cache-test timegraph
cache-ref: cache-test.o cache-ref.o
	gcc -g -o cache-ref cache-test.o cache-ref.o
cache-test: cache-test.o cache.o
cache-test.o: cache-test.c cache.h
cache.o: cache.c cache.h
timegraph: graph.o timegraph.o
timegraph.o: timegraph.c graph.h
	gcc -c -g -O3 -o timegraph.o timegraph.c
graph.o: graph.c graph.h
	gcc -c -g -O3 -o graph.o graph.c
clean: 
	rm -f cache-test cache-test.o cache.o cache-ref timegraph timegraph.o graph.o

view:
	vim cache.c cache-test.c -o
