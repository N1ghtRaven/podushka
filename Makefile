CFLAGS +=-s -Os

all: decoder_test encoder_test

decoder_test: decoder.o util.o
	mkdir -p exe
	$(CC) -o exe/decoder_test obj/decoder.o obj/util.o test/decoder_test.c -lcriterion
	./exe/decoder_test

decoder.o:
	mkdir -p obj
	$(CC) -c src/decoder.c -o obj/decoder.o $(CFLAGS)


encoder_test: encoder.o util.o
	mkdir -p exe
	$(CC) -o exe/encoder_test obj/encoder.o obj/util.o test/encoder_test.c -lcriterion
	./exe/encoder_test

encoder.o:
	mkdir -p obj
	$(CC) -c src/encoder.c -o obj/encoder.o $(CFLAGS)


util.o:
	$(CC) -c src/util.c -o obj/util.o

clean:
	rm -rf *.o $(PRJ_NAME) decoder_test encoder_test