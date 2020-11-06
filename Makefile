PRJ_NAME=pdu
CFLAGS +=-s -Os

all: $(PRJ_NAME)

test: decoder_test encoder_test

decoder_test: decoder.o util.o
	mkdir -p exe
	$(CC) -o exe/decoder_test obj/decoder.o obj/util.o test/decoder_test.c -lcriterion
	./exe/decoder_test

decoder.o:
	mkdir -p obj
	$(CC) -c src/decoder.c -o obj/decoder.o


encoder_test: encoder.o util.o
	mkdir -p exe
	$(CC) -o exe/encoder_test obj/encoder.o obj/util.o test/encoder_test.c -lcriterion
	./exe/encoder_test

encoder.o:
	mkdir -p obj
	$(CC) -c src/encoder.c -o obj/encoder.o


util.o:
	$(CC) -c src/util.c -o obj/util.o

clean:
	rm -rf *.o $(PRJ_NAME)