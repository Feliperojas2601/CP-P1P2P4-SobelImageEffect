
build:
	gcc ./image-effect-openmp.c -o image-effect-openmp -fopenmp -lm
run: build
