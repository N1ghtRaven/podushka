CFLAGS +=-s -Os

all: 
	mkdir -p exe
	$(CC) -o exe/manual_decoder ../../obj/decoder.o ../../obj/util.o src/manual_decoder.c $(CFLAGS)

clean:
	rm -rf *.o