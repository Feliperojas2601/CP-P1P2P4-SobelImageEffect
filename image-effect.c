#include<stdio.h>
#include<pthread.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>

void* proccess_image(int number_thread) {

	return NULL;

}


int main(int argc, char* argv[]){

    char* img_in = argv[1]; 
    char* img_out = argv[2]; 
    int number_pthreads = atoi(argv[3]); 

    printf("%s", img_in);
    printf("%s", img_out);
    printf("%d", number_pthreads);

    int i_pthreads[number_pthreads]; 
	int error_pthread;
	pthread_t pthreads[number_pthreads];

	for(int i = 0; i < number_pthreads; i++) {

		i_pthreads[i] = i; 
		error_pthread = pthread_create(&(pthreads[i]), NULL, proccess_image, (void*)&i_pthreads[i]);
		if (error_pthread != 0) {
			printf("Error creando thread \n");
			return -1;
		}

	}

	for(int j = 0; j < number_pthreads; j++) {

		pthread_join(pthreads[j], NULL);
	
    }

  	return 0;

}