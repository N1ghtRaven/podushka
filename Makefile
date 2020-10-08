PRJ_NAME=pdu
CFLAGS +=-s -Os -flto -fno-asynchronous-unwind-tables
all: $(PRJ_NAME)

test: 
	$(CC) -o $(PRJ_NAME) src/$(PRJ_NAME).c $(CFLAGS) -DUNIT_TEST -lcriterion
	./$(PRJ_NAME)

profile:
	$(CC) -pg -Wall -o $(PRJ_NAME) src/$(PRJ_NAME).c
	./$(PRJ_NAME) > /dev/null
	gprof ./$(PRJ_NAME)

$(PRJ_NAME): $(PRJ_NAME).o
	$(CC) -o $(PRJ_NAME) $(PRJ_NAME).o $(CFLAGS)
	strip --strip-all ./$(PRJ_NAME)

$(PRJ_NAME).o:
	$(CC) -c -o $(PRJ_NAME).o src/$(PRJ_NAME).c $(CFLAGS)

clean:
	rm -rf *.o $(PRJ_NAME)