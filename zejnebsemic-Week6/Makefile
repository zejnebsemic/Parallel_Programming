CC = gcc
CFLAGS = -g -O3 -fstrict-aliasing -ftree-vectorize -fopenmp-simd \
 -march=native -mtune=native \
 -fopt-info-vec-optimized -fopt-info-vec-missed -fno-trapping-math -fno-math-errno

CFLAGS_OPT3 = $(CFLAGS)

OBJ = main.o timer.o

OPT1 = timestep_opt1.o
OPT2 = timestep_opt2.o
OPT3 = timestep_opt3.o
OPT3TP = timestep_opt3_two_pass.o

all: opt1 opt2 opt3 opt3tp

opt1: $(OBJ) $(OPT1)
	$(CC) $(CFLAGS) -o stream_opt1 $^ -lm

opt2: $(OBJ) $(OPT2)
	$(CC) $(CFLAGS) -o stream_opt2 $^ -lm

opt3: $(OBJ) $(OPT3)
	$(CC) $(CFLAGS_OPT3) -o stream_opt3 $^ -lm

opt3tp: $(OBJ) $(OPT3TP)
	$(CC) $(CFLAGS_OPT3) -o stream_opt3_two_pass $^ -lm

clean:
	rm -f *.o stream_opt1 stream_opt2 stream_opt3 stream_opt3_two_pass
