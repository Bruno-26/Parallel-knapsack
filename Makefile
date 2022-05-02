all: main exec

main:
	gcc -O3 -fopenmp knapSack.c

exec:
	export OMP_NUM_THREADS=1
	./a.out < entrada/10000

