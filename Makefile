CFLAGS := -Wall -ggdb -lssl -lcrypto

a.out:main.c formatting.c connections.c ping.c config.c
	gcc $^ ${CFLAGS} -o a.out
