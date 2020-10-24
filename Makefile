PRJ_NAME=pdu
CFLAGS +=-s -Os
all: $(PRJ_NAME)

test:
	$(CC) -o $(PRJ_NAME) src/encoder.c src/decoder.c src/tests.c -DUNIT_TEST -lcriterion
	./$(PRJ_NAME)

profile:
	$(CC) -pg -Wall -o $(PRJ_NAME) src/$(PRJ_NAME).c
	./$(PRJ_NAME) > /dev/null
	gprof ./$(PRJ_NAME)

$(PRJ_NAME): $(PRJ_NAME).o
	$(CC) -o $(PRJ_NAME) $(PRJ_NAME).o $(CFLAGS)
	strip --strip-all ./$(PRJ_NAME)

$(PRJ_NAME).o:
	$(CC) -o $(PRJ_NAME).o src/encoder.c src/decoder.c src/main.c

mvp:
	$(CC) -o $(PRJ_NAME) src/main.c

clean:
	rm -rf *.o $(PRJ_NAME)