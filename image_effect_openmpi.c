#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CHANNEL_NUM 1

int main(int argc, char *argv[] ) {
    // Variable declarations in each process 

    uint8_t* image, edge_image;
    uint8_t* section_image;  

    int width, height, bpp;
    int amount_work;  

    size_t imgs_size;
    FILE * fPtr;   
    //struct timeval tval_before, tval_after, tval_result;

    MPI_Status mpi_status; 
    int master = 0; 
    int rank, size; 
    double start, end;

    // Init MPI 

    MPI_Init(&argc,&argv); 

    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    // Read image, check parameters, charge image data and send to process a part of the image in master process

    if(rank == master){
        // Init count of time

        start = MPI_Wtime();

        // Check number parameters

        if (argc != 3) {
            printf("Incorrect input arguments. Usage: <in image> <out image>\n");
            MPI_Finalize();
            return 0;
        }

        // Read image and calculate amount of work, broadcast them with width and height

        image = stbi_load(argv[1], &width, &height, &bpp, 1);
        printf("Loaded image with height %d and width %d \n", height, width);
        MPI_Bcast(&image, 1, MPI_UINT8_T, master, MPI_COMM_WORLD);
        amount_work = (width/size) + 1;
        MPI_Bcast(&amount_work, 1, MPI_INT, master, MPI_COMM_WORLD);
        MPI_Bcast(&width, 1, MPI_INT, master, MPI_COMM_WORLD);
        MPI_Bcast(&height, 1, MPI_INT, master, MPI_COMM_WORLD);
        
        // Reserve memory for edge_image

        imgs_size = width * height; 
        edge_image = malloc(imgs_size); 
        fPtr = fopen("results-openmpi.txt", "a");

        //gettimeofday(&tval_before, NULL); 
    }

    // Establish barrier after broadcast 

    MPI_Barrier(MPI_COMM_WORLD);

    // ALlocate space for section image

    imgs_size = width * height; 
    section_image = malloc(imgs_size);

    // Declare sobel kernels 

    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Apply sobel filter to section images

    for(int x = (rank * amount_work); x < (rank * amount_work) + amount_work; x++) {
    	for(int y = 1; y < height - 2; y++) {
    		int pixel_x = ((sobel_x[0][0]*image[x*height-1+y-1])+ (sobel_x[0][1]* image[x*height+y-1]) + (sobel_x[0][2] * image[x*height+1+y-1]))+
    			      ((sobel_x[1][0]*image[x*height-1+y])+ (sobel_x[1][1]* image[x*height+y]) + (sobel_x[1][2] * image[x*height+1+y]))+
    			      ((sobel_x[2][0]*image[x*height-1+y+1])+ (sobel_x[2][1]* image[x*height+y+1]) + (sobel_x[2][2] * image[x*height+1+y+1]));
    			      
    	    int pixel_y = ((sobel_y[0][0]*image[x*height-1+y-1])+ (sobel_y[0][1]* image[x*height+y-1]) + (sobel_y[0][2] * image[x*height+1+y-1]))+
    			      ((sobel_y[1][0]*image[x*height-1+y])+ (sobel_y[1][1]* image[x*height+y]) + (sobel_y[1][2] * image[x*height+1+y]))+
    			      ((sobel_y[2][0]*image[x*height-1+y+1])+ (sobel_y[2][1]* image[x*height+y+1]) + (sobel_y[2][2] * image[x*height+1+y+1]));

    		int val = ceil(sqrt((pixel_x*pixel_x) + (pixel_y*pixel_y)));
    		section_image[x*height+y] = val;	
    	}
	}

    // Establish barrier after sobel 

    MPI_Barrier(MPI_COMM_WORLD);

    // Send section images, join them and finish time count in master process 

    MPI_Gather(&section_image,1, MPI_UINT8_T, &edge_image,1, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    // CHANGE GATHER TO FOR OF SENDS AND ADD FOR OF RECVS WHO PASTE IMG 

    if (rank == master){
        //gettimeofday(&tval_after, NULL);
        //timersub(&tval_after, &tval_before, &tval_result);
        //printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
        //fprintf(fPtr, "%-12ld%-13d%ld.%06ld seconds\n", imgs_size, number_processes, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

        end = MPI_Wtime();

        printf("Sobel edge filter was sucessfully applied to %s \n", argv[1]);

        stbi_write_png(argv[2], width, height, CHANNEL_NUM, edge_image, width*CHANNEL_NUM);
    }
    

    MPI_Finalize();

    stbi_image_free(image); 
    fclose(fPtr);
    free(edge_image);
    free(section_image);
    return 0;
}

