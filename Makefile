
build:
	gcc ./image-effect-pthread.c -o image-effect-openmp -fopenmp -lm
run: build