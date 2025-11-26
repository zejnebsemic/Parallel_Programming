CC = mpicc

default: PartialSum

PartialSum: PartialSum.o timer.o
	$(CC) PartialSum.o timer.o -o PartialSum

%.o: %.c
	$(CC) -c $<

run:
	mpirun -n 4 ./PartialSum

clean:
	rm -f *.o PartialSum
