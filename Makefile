CFLAGS := -Wall -ggdb -lssl -lcrypto

eyeball:main.c formatting.c connections.c ping.c config.c
	gcc $^ ${CFLAGS} -o a.out
