clear:
	@echo "Image_size  Num_threads  Time" > results.txt
	@echo "Image_size  Num_threads  Time" > results-openmp.txt

build:
	gcc -fopenmp ./image-effect-openmp.c -o image-effect-openmp -lm

effect-720:
	./image-effect dog720p.jpg dog720p_output.jpg 1
	./image-effect dog720p.jpg dog720p_output.jpg 2
	./image-effect dog720p.jpg dog720p_output.jpg 4
	./image-effect dog720p.jpg dog720p_output.jpg 8
	./image-effect dog720p.jpg dog720p_output.jpg 16

effect-1080:
	./image-effect dog1080p.jpg dog1080p_output.jpg 1
	./image-effect dog1080p.jpg dog1080p_output.jpg 2
	./image-effect dog1080p.jpg dog1080p_output.jpg 4
	./image-effect dog1080p.jpg dog1080p_output.jpg 8
	./image-effect dog1080p.jpg dog1080p_output.jpg 16

effect-4k:
	./image-effect dog4k.jpg dog4k_output.jpg 1
	./image-effect dog4k.jpg dog4k_output.jpg 2
	./image-effect dog4k.jpg dog4k_output.jpg 4
	./image-effect dog4k.jpg dog4k_output.jpg 8
	./image-effect dog4k.jpg dog4k_output.jpg 16

effect-720-openmp:
	./image-effect-openmp dog720p.jpg dog720p-openmp.jpg 1
	./image-effect-openmp dog720p.jpg dog720p-openmp.jpg 2
	./image-effect-openmp dog720p.jpg dog720p-openmp.jpg 4
	./image-effect-openmp dog720p.jpg dog720p-openmp.jpg 8
	./image-effect-openmp dog720p.jpg dog720p-openmp.jpg 16

effect-1080-openmp:
	./image-effect-openmp dog1080p.jpg dog1080p-openmp.jpg 1
	./image-effect-openmp dog1080p.jpg dog1080p-openmp.jpg 2
	./image-effect-openmp dog1080p.jpg dog1080p-openmp.jpg 4
	./image-effect-openmp dog1080p.jpg dog1080p-openmp.jpg 8
	./image-effect-openmp dog1080p.jpg dog1080p-openmp.jpg 16

effect-4k-openmp:
	./image-effect-openmp dog4k.jpg dog4k-openmp.jpg 1
	./image-effect-openmp dog4k.jpg dog4k-openmp.jpg 2
	./image-effect-openmp dog4k.jpg dog4k-openmp.jpg 4
	./image-effect-openmp dog4k.jpg dog4k-openmp.jpg 8
	./image-effect-openmp dog4k.jpg dog4k-openmp.jpg 16

run: effect-720 effect-1080 effect-4k

run-openmp: clear effect-720-openmp effect-1080-openmp effect-4k-openmp