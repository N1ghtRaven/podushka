PRJ_NAME=pdu
CFLAGS +=-s -Os

all: $(PRJ_NAME)

test: 
	$(CC) -o $(PRJ_NAME) src/$(PRJ_NAME).c $(LDFLAGS) $(CFLAGS) -DUNIT_TEST -lcriterion
	./$(PRJ_NAME)

$(PRJ_NAME): $(PRJ_NAME).o
	$(CC) -o $(PRJ_NAME) $(PRJ_NAME).o $(LDFLAGS) $(CFLAGS)

$(PRJ_NAME).o:
	$(CC) -c -o $(PRJ_NAME).o src/$(PRJ_NAME).c $(LDFLAGS) $(CFLAGS)

clean:
	rm -rf *.o $(PRJ_NAME)