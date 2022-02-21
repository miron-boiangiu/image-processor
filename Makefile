build:
	gcc image_processor.c -o bmp -lm -g -Wall

run:
	./bmp

clean:
	rm -f bmp
