all: clear build effect-720 effect-1080 effect-4k

clear:
	@echo "Image_size  Num_threads  Time" > results.txt

build:
	gcc ./image-effect.c -o image-effect -pthread -lm

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

run: effect-720 effect-1080 effect-4k