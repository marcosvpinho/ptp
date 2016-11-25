CC=gcc
CFLAGS=-g
OBJS=protocoloTESTE.o utilsTESTE.o filaTESTE.o 

testetap: $(OBJS)
	$(CC) -o protocoloTESTE $(OBJS) -lutil

clean:
	rm -f *.o protocoloTESTE

