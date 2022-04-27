#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/time.h>

// Declare STB_Image library

#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"  
#define STB_IMAGE_WRITE_IMPLEMENTATION  
#include "stb_image_write.h"  

#define CHANNEL_NUM 1

int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

// Create structure for arguments in pthread_create 

struct pthread_args {
	int pthread_number;
	int height; 
	int width;   
	uint8_t *img_in; 
	uint8_t *img_out; 
	int pthreads; 
};  

void* proccess_image(void *args) {

	// Destruct args

	int pthread_number = ((struct pthread_args*) args)->pthread_number;
	int width = ((struct pthread_args*) args)->width; 
	int height = ((struct pthread_args*) args)->height; 
	uint8_t *img_in = ((struct pthread_args*) args)->img_in; 
	uint8_t *img_out = ((struct pthread_args*) args)->img_out; 
	int pthreads = ((struct pthread_args*) args)->pthreads; 

	// Calculate division of image per thread 

    int amount_work_x = (int)((width)/pthreads);

	// Filter implementation 

	for(int x = (pthread_number * amount_work_x); x < (pthread_number * amount_work_x) + amount_work_x; x++) {
    	for(int y = 1; y < height - 2; y++) {
    		int pixel_x = ((sobel_x[0][0]*img_in[x*height-1+y-1])+ (sobel_x[0][1]* img_in[x*height+y-1]) + (sobel_x[0][2] * img_in[x*height+1+y-1]))+
    			      ((sobel_x[1][0]*img_in[x*height-1+y])+ (sobel_x[1][1]* img_in[x*height+y]) + (sobel_x[1][2] * img_in[x*height+1+y]))+
    			      ((sobel_x[2][0]*img_in[x*height-1+y+1])+ (sobel_x[2][1]* img_in[x*height+y+1]) + (sobel_x[2][2] * img_in[x*height+1+y+1]));
    			      
    	    int pixel_y = ((sobel_y[0][0]*img_in[x*height-1+y-1])+ (sobel_y[0][1]* img_in[x*height+y-1]) + (sobel_y[0][2] * img_in[x*height+1+y-1]))+
    			      ((sobel_y[1][0]*img_in[x*height-1+y])+ (sobel_y[1][1]* img_in[x*height+y]) + (sobel_y[1][2] * img_in[x*height+1+y]))+
    			      ((sobel_y[2][0]*img_in[x*height-1+y+1])+ (sobel_y[2][1]* img_in[x*height+y+1]) + (sobel_y[2][2] * img_in[x*height+1+y+1]));
    		int val = ceil(sqrt((pixel_x*pixel_x) + (pixel_y*pixel_y)));
    		img_out[x*height+y] = val;	
    	}
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
    struct timeval tval_before, tval_after, tval_result;
    FILE * fPtr;
    
    fPtr = fopen("results.txt", "a");

	// Load input image with pointer and general info

    int width, height, channels;
    uint8_t *img_in = stbi_load(img_path_in, &width, &height, &channels, 1);
    if(img_in == NULL) {
        printf("Error loading the image\n");
        return -1;
    }

	// Declare size of images and use malloc to reserve memory of ouput image

	size_t imgs_size = width * height; 
	uint8_t *img_out = malloc(imgs_size); 
	if(img_out == NULL) {
       printf("Unable to allocate memory for the gray image.\n");
       exit(1);
    }

	// Instance struct for pthread_create 

	struct pthread_args args_array[number_pthreads];

	// Threads Logic

    int i_pthreads[number_pthreads]; 
	int error_pthread;
	pthread_t pthreads[number_pthreads];

  	gettimeofday(&tval_before, NULL);
	for(int i = 0; i < number_pthreads; i++) {
		i_pthreads[i] = i; 
		args_array[i].img_in = img_in; 
		args_array[i].img_out = img_out;  
		args_array[i].width = width; 
		args_array[i].height = height;  
		args_array[i].pthreads = number_pthreads;
		args_array[i].pthread_number = i_pthreads[i];
		error_pthread = pthread_create(&(pthreads[i]), NULL, proccess_image, (void*)&args_array[i]);
		if (error_pthread != 0) {
			printf("Error in thread creation \n");
			return -1;
		}
	}

	for(int j = 0; j < number_pthreads; j++) {
		pthread_join(pthreads[j], NULL);
    }
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Time elapsed: %ld.%06ld seconds\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    fprintf(fPtr, "%-12ld%-13d%ld.%06ld seconds\n", imgs_size, number_pthreads, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

	// Write jgp image out 

	stbi_write_png(img_path_out, width, height, CHANNEL_NUM, img_out, width*CHANNEL_NUM);

	// Free from STB Library

	stbi_image_free(img_in); 
	stbi_image_free(img_out);
  fclose(fPtr);

  	return 0;

}
