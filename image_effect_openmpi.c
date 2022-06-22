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

    unsigned char *image, *edge_image, *section_image, *section_sobel_image; 

    int width, height, bpp;
    int amount_work; 
    int params[3]; 
    size_t imgs_size;
    FILE * fPtr;   
    struct timeval tval_before, tval_after, tval_result;

    // MPI Variables 

    int master = 0; 
    int rank, size, temporal_size; 
    double start, end, elapsed;

    // Init MPI 

    MPI_Init(&argc,&argv); 

    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 

    // In master: read the image, check parameters, charge the image data and send to each process a part of the image

    if (rank == master) {

        // Check number of parameters

        if (argc != 3) {
            printf("Incorrect input arguments. Usage: mpirun -np number_process image_effect <in image> <out image>\n");
            MPI_Finalize();
            return -1;
        }

        // Read image and calculate the amount of work per process and the image size 

        image = stbi_load(argv[1], &width, &height, &bpp, 1);
        printf("Loaded image with height %d and width %d \n", height, width);
        amount_work = (width/size) + 1;
        imgs_size = width * height; 

        // Init count of time 

        start = MPI_Wtime();
        gettimeofday(&tval_before, NULL);

        // Establish params: amount of work, width and height

        params[0] = amount_work; 
        params[1] = width; 
        params[2] = height;

        // Send image according to images_size 

        //temporal_size = height * amount_work; 
        temporal_size = imgs_size;

        for(int i = 1; i < size; i++) {
            //section_image = (unsigned char*)malloc(temporal_size * sizeof(unsigned char*));
            section_image = (unsigned char*)malloc(imgs_size * sizeof(unsigned char*));
			//memcpy(section_image, image + (i * temporal_size * sizeof(unsigned char*)), temporal_size * sizeof(unsigned char*));
            memcpy(section_image, image, imgs_size);
            MPI_Send(&temporal_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD); 
			//MPI_Send(section_image, temporal_size * sizeof(unsigned char*), MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
            MPI_Send(section_image, imgs_size, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
			free(section_image);  
        }  

        // Memcpy for section image in master

        section_image = (unsigned char*)malloc(imgs_size * sizeof(unsigned char*));
        memcpy(section_image, image, imgs_size);

        // Open result text file

        fPtr = fopen("results-openmpi.txt", "a");      

        // Free memory of image 

        stbi_image_free(image); ;

    } else {

        // Receive temporal size

        MPI_Recv(&temporal_size, 1, MPI_INT, master, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Allocate memory for section image

        section_image = (unsigned char*)malloc(temporal_size * sizeof(unsigned char*));

        // Receive section image

        MPI_Recv(section_image, temporal_size, MPI_UNSIGNED_CHAR, master, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }

    // Establish barrier after receive sections of image in each process 

    MPI_Barrier(MPI_COMM_WORLD);

    // Broadcast params: amount of work, width and height. Establish barrier after it.

    MPI_Bcast(params, 3, MPI_INT, master, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // Calculate imgs_size and establish params 

    amount_work = params[0]; 
    width = params[1]; 
    height = params[2]; 
    imgs_size = width * height;

    // Allocate space for section sobel iamge 

    section_sobel_image = (unsigned char*)malloc(temporal_size * sizeof(unsigned char*));

    // Declare sobel kernels 

    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Apply sobel filter to section images and put in secton sobel images

    for(int x = (rank * amount_work); x < (rank * amount_work) + amount_work; x++) {
    	for(int y = 0; y < height; y++) {
    		int pixel_x = ((sobel_x[0][0]*section_image[x*height-1+y-1])+ (sobel_x[0][1]* section_image[x*height+y-1]) + (sobel_x[0][2] * section_image[x*height+1+y-1]))+
    			      ((sobel_x[1][0]*section_image[x*height-1+y])+ (sobel_x[1][1]* section_image[x*height+y]) + (sobel_x[1][2] * section_image[x*height+1+y]))+
    			      ((sobel_x[2][0]*section_image[x*height-1+y+1])+ (sobel_x[2][1]* section_image[x*height+y+1]) + (sobel_x[2][2] * section_image[x*height+1+y+1]));
    			      
    	    int pixel_y = ((sobel_y[0][0]*section_image[x*height-1+y-1])+ (sobel_y[0][1]* section_image[x*height+y-1]) + (sobel_y[0][2] * section_image[x*height+1+y-1]))+
    			      ((sobel_y[1][0]*section_image[x*height-1+y])+ (sobel_y[1][1]* section_image[x*height+y]) + (sobel_y[1][2] * section_image[x*height+1+y]))+
    			      ((sobel_y[2][0]*section_image[x*height-1+y+1])+ (sobel_y[2][1]* section_image[x*height+y+1]) + (sobel_y[2][2] * section_image[x*height+1+y+1]));

    		int val = ceil(sqrt((pixel_x*pixel_x) + (pixel_y*pixel_y)));
    		section_sobel_image[x*height+y] = val;	
    	}
	}

    // Establish barrier after sobel 

    MPI_Barrier(MPI_COMM_WORLD);

    // Send section sobel images, join them and finish time count in master process 

    if (rank != master) {

        // Send section images 

        MPI_Send(section_sobel_image, imgs_size, MPI_UNSIGNED_CHAR, master, 0, MPI_COMM_WORLD);

    } else {

        // Reserve memory for edge_image

        edge_image = (unsigned char*)malloc(imgs_size * sizeof(unsigned char*));

        // Concat section image of master to edge image according to imgs-size / amount  

        memcpy(edge_image, section_sobel_image, (imgs_size/size) + width);

        // Concat received sections images  

        for(int j = 1; j < size; j++) {
            free(section_sobel_image); 
            section_sobel_image = (unsigned char*)malloc(imgs_size * sizeof(unsigned char*));
            MPI_Recv(section_sobel_image, imgs_size, MPI_UNSIGNED_CHAR, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (j != size - 1) {
                memcpy(edge_image + (j * (imgs_size/size) + width), section_sobel_image + (j * (imgs_size/size) + width), (imgs_size/size));
            } else {
                memcpy(edge_image + (j * (imgs_size/size) + width), section_sobel_image + (j * (imgs_size/size) + 2*width), (imgs_size/size));
            }
        }

        // End count of sobel filter time 

        end = MPI_Wtime();

        // Print time and write png 

        elapsed = end - start;

        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        printf("Time elapsed Get Time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
        fprintf(fPtr, "%-12ld%-13d%ld.%06ld seconds\n", imgs_size, size, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

        printf("Elapsed time MPI: %f seconds\n", elapsed); 

        printf("Sobel edge filter was sucessfully applied to %s \n", argv[1]);

        stbi_write_png(argv[2], width, height, CHANNEL_NUM, edge_image, width*CHANNEL_NUM); 

        // Close result text file 

        fclose(fPtr);

        // Free edge image 

        free(edge_image);

    }

    // Free section and section sobel images 

    free(section_image);
    free(section_sobel_image);

    // Finalize MPI

    MPI_Finalize();

    return 0;
}

