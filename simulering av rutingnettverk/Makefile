CCFLAGS= -Wall -Wextra -Wpedantic -std=c99 -g

all: routing_server node

routing_server: routing_server.c
	gcc print_lib.c routing_server.c $(CCFLAGS) -o routing_server

node: node.c
	gcc print_lib.c node.c $(CCFLAGS) -o node


run: all
	bash run_1.sh 5000

.PHONY: clean

clean:
	rm -f routing_server
	rm -f node
	rm -f *.0
