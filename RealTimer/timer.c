/*
 *	Title	: Real Timer
 *
 *	Short	: A low energy timer solution
 *
 *	Long 	: Wake up every x seconds for obtaining a timestamp
 * 		        for a total duration of N timestamps
 *
 *	Author	: Rafael Brouzos.
 *
 *	Date	: 25/5/2017 (update)
 *
 */

 //Include libraries
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

//global variables
struct timeval tv;	//time struct to get timestamp
long start, *a;		//starting time and timestamp matrix

//functions
void sampleTime(int j);


/*  Main function of the program. Calls the time sampler and sleeps
 * 	until the new sample is ready to be taken. */
int main(int argc, char **argv) {

	int j;			//helps as counter
	int N;			//number of samples
	float x,y;		//time space between samples in microseconds

	//check for arguments
	if (argc != 3) {
		printf("Please just pass 2 arguments \n");
		exit(1);
	}

	//1st argument is the number of timestaps
	N = atoi(argv[1]);
	a = (long *) malloc(N * sizeof(long));

	//2nd argument is the sampling timestep
	x = atof(argv[2]);
	y = x * 1000000;
	x = y;	//something like a temp

    //get start time in usec
    gettimeofday(&tv,NULL);
    start = 1000000 * tv.tv_sec + tv.tv_usec;

	//sampling loop
    for (j=0;j<N;j++) {
		//get sample
		sampleTime(j);

		//5th,10th,15th... samples fix the time error (feedback)
		if(j%5 == 0){ y = y - (a[j]-j*x); }
		else{ y = x; }

		//wait before next sample
		usleep(y);
    }

    //before exiting print results
    for(j=0;j<N;j++){
		printf("timestamp	%d =	%ld\n",j,a[j]);
	}

    return 100;
}


/*	Takes a time sample and stores it into a */
void sampleTime(int j) {
	//get timestamp
	gettimeofday(&tv,NULL);
	//write timestamp in usecond into a
	a[j] = 1000000 * tv.tv_sec + tv.tv_usec - start;
}
