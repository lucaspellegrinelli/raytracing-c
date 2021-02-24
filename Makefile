all:
	gcc main.c src/common_math.c src/raytracing.c -I include/ -o main -lm -O3 -Wall

lint:
	clang-format -i *.c
	clang-format -i src/*.c
	clang-format -i include/*.h

clean:
	rm -rf *.o
	rm main