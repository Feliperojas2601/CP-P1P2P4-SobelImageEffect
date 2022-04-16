#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"  
#define STB_IMAGE_WRITE_IMPLEMENTATION  
#include "stb_image_write.h"  

void* proccess_image(int number_thread) {

	return NULL;

}


int main(int argc, char* argv[]){

	if (argc != 4) {
		printf("Parameters Error \n");
		return -1; 
	}

    char* img_in = argv[1]; 
    char* img_out = argv[2]; 
    int number_pthreads = atoi(argv[3]); 

    int width, height, channels;
    unsigned char *img = stbi_load("dog.jpeg", &width, &height, &channels, 0);
    if(img == NULL) {
        printf("Error in loading the image\n");
        return -1;
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    int i_pthreads[number_pthreads]; 
	int error_pthread;
	pthread_t pthreads[number_pthreads];

	for(int i = 0; i < number_pthreads; i++) {

		i_pthreads[i] = i; 
		error_pthread = pthread_create(&(pthreads[i]), NULL, proccess_image, (void*)&i_pthreads[i]);
		if (error_pthread != 0) {
			printf("Error in thread creation \n");
			return -1;
		}

	}

	for(int j = 0; j < number_pthreads; j++) {

		pthread_join(pthreads[j], NULL);
	
    }

  	return 0;

}