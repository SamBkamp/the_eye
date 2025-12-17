CFLAGS := -Wall -ggdb -lssl -lcrypto

a.out:main.c formatting.c connections.c ping.c
	gcc $^ ${CFLAGS} -o a.out
