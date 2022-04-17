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

	// Receive Exe parameters, receive strings with pointers

	if (argc != 4) {
		printf("Parameters Error \n");
		return -1; 
	}

    char* img_path_in = argv[1]; 
    char* img_path_out = argv[2]; 
    int number_pthreads = atoi(argv[3]); 

	// Load input image with pointer and general info

    int width, height, channels;
    unsigned char *img_in = stbi_load(img_path_in, &width, &height, &channels, 0);
    if(img_in == NULL) {
        printf("Error loading the image\n");
        return -1;
    }
	printf("%d", channels);

	// Declare size of images and use malloc to reserve memory of ouput image

	size_t imgs_size = width * height * channels; 
	unsigned char* img_out = malloc(imgs_size); 
	if(img_out == NULL) {
       printf("Unable to allocate memory for the gray image.\n");
       exit(1);
    }

	// Threads Logic ...

	// Filter 

	for(unsigned char *ii = img_in, *io = img_out; ii != img_in + imgs_size; ii += channels, io += channels) {
        //*io = (uint8_t)(2*(*ii + *(ii + 1) + *(ii + 2)));
		*io = (uint8_t) 2 * *ii;
		*(io + 1) = (uint8_t) 2 * *(ii + 1);
		*(io + 2) = (uint8_t) 2 * *(ii + 2);
    }

	// Write jgp image out 

	stbi_write_jpg(img_path_out, width, height, channels, img_out, 100);

	// Free from STB Library

	stbi_image_free(img_in); 
	stbi_image_free(img_out); 

	// Threads Logic

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