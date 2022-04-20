#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>

// Declare STB_Image library

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"  
#define STB_IMAGE_WRITE_IMPLEMENTATION  
#include "stb_image_write.h"  

// Create structure for arguments in pthread_create 

struct pthread_args {
	int pthread_number;
	int channels;
	size_t imgs_size;   
	char* img_in; 
	char* img_out; 
	int pthreads; 
};  

void* proccess_image(void *args) {

	// Destruct args

	int pthread_number = ((struct pthread_args*) args)->pthread_number; 
	int channels = ((struct pthread_args*) args)->channels;
	size_t imgs_size = ((struct pthread_args*) args)->imgs_size;  
	char *img_in = ((struct pthread_args*) args)->img_in; 
	char *img_out = ((struct pthread_args*) args)->img_out; 
	int pthreads = ((struct pthread_args*) args)->pthreads; 

	int pad = imgs_size/pthreads;  

	// Filter implementation

	for(unsigned char *ii = img_in + (pad*pthread_number), *io = img_out + (pad*pthread_number); ii != img_in + (pad*(pthread_number+1)); ii += channels, io += channels) {
		*io = (uint8_t) 2 * *ii;
		*(io + 1) = (uint8_t) 2 * *(ii + 1);
		*(io + 2) = (uint8_t) 2 * *(ii + 2);
    }

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

	// Declare size of images and use malloc to reserve memory of ouput image

	size_t imgs_size = width * height * channels; 
	unsigned char* img_out = malloc(imgs_size); 
	if(img_out == NULL) {
       printf("Unable to allocate memory for the gray image.\n");
       exit(1);
    }

	// Instance struct for pthread_create 

	struct pthread_args *args = (struct pthread_args *)malloc(sizeof(struct pthread_args));
	args->img_in = img_in; 
	args->img_out = img_out;   
	args->channels = channels;
	args->imgs_size = imgs_size;  
	args->pthreads = number_pthreads; 

	// Threads Logic

    int i_pthreads[number_pthreads]; 
	int error_pthread;
	pthread_t pthreads[number_pthreads];

	for(int i = 0; i < number_pthreads; i++) {

		i_pthreads[i] = i; 
		args->pthread_number = i; 
		error_pthread = pthread_create(&(pthreads[i]), NULL, proccess_image, (void*)args);
		if (error_pthread != 0) {
			printf("Error in thread creation \n");
			return -1;
		}

	}

	for(int j = 0; j < number_pthreads; j++) {

		pthread_join(pthreads[j], NULL);
	
    }

	// Write jgp image out 

	stbi_write_jpg(img_path_out, width, height, channels, img_out, 100);

	// Free from STB Library

	stbi_image_free(img_in); 
	stbi_image_free(img_out); 

  	return 0;

}