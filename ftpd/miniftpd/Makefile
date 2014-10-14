#显示指定伪目标否则容易错
.PHONY:clean
CC=gcc
CFLAGS=-Wall -g
BIN=miniftpd
OBJECTS=main.o sysutil.o session.o privparent.o ftpproto.o str.o tunable.o parseconf.o privsock.o hash.o
LIBS=-lcrypt
$(BIN):$(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(BIN) $(OBJECTS)
