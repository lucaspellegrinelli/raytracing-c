all:
	gcc main.c common_math.c raytracing.c -o main -lm -O3 -Wall

clean:
	rm -rf *.o
	rm main