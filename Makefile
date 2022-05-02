all: main exec

main:
	mpiCC -O3 mochila.c

exec:
	mpirun -np 2 ./a.out  < entrada/500