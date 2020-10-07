/*
 *	Template: prop-cons.c by Andrae Muys
 *
 *	Title	: Wifier
 *
 *	Short	: A low energy WIFI scanner solution using pthreads
 *
 *	Long 	: Scan every t seconds for Wifi networks and add them
 * 		        into a file with its timestamps.
 *
 *	Author	: Rafael Brouzos.
 *
 *	Date	: 1/10/2017 (update)
 *
 */

 //Raf Brouzos

//Include libraries
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define QUEUESIZE 20000		//size of the queue
#define NameSpace 100		//maximun length of every SSID string

//global variables
struct timeval tv;	//time struct to get timestamp
float t; 			//the timestep of each scan (argument)
FILE *wifile;		//file with list of the the wifi networks Output
//timestamp keepers
long start;

//pthreads function pointers
void *producer (void *args);
void *consumer (void *args);

//queue struct
typedef struct {
	//queue data variables
	char buff[QUEUESIZE][NameSpace];	//SSID names
	long stamps[QUEUESIZE];				//timestamps
	//queue helping variables
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;


//define functions
queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, char *in, long stamp);
void queueDel (queue *q, char **out, long *stamp);
long scanWIFI(queue *q);
int addWIFI(char *d, long timeval);

/* Main function of the program. Takes the arguments and creates the
 * threads of the producer and consumer model. */
int main (int argc, char **argv){

	//check for arguments
	if (argc != 2) {
		printf("Please just pass 1 argument \n");
		exit(1);
	}
	//the argument is the scanning timestep in seconds
	t = atof(argv[1]);
	//scan timestep in microseconds
	t = t * 1000000;
	//limit scan timestep to 2sec
	if(t<2000000){
		printf("given scan timestep too small... \n	automatically set to the minimun value of 1,6 seconds\n");
		t = 2000000;
	}

	//clean the file to write new wifi list inside
	wifile = fopen("wifile.txt", "w");
	if(wifile == NULL){
		printf("Error while creating the wifile.txt!");
		exit(1);
	}
	fclose(wifile);

	//open the file to write wifi list inside
	wifile = fopen("wifile.txt", "r+");
	if(wifile == NULL){
		printf("Error while openning the wifile.txt!");
		exit(1);
	}

	//write title at the first line
	fprintf(wifile,"WIFI list with timestamps below\n");
	fflush(wifile);	//update the file before fclose

	//define pthread functions and the queue
	queue *fifo;
	pthread_t pro, con;
	//make the queue
	fifo = queueInit ();
	if (fifo ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}

	//call the threads and wait them finish
	pthread_create (&pro, NULL, producer, fifo);
	pthread_create (&con, NULL, consumer, fifo);
	pthread_join (pro, NULL);
	pthread_join (con, NULL);
	//delete the queue the free the memory
	queueDelete (fifo);
	//close the file
	fclose(wifile);
	return 0;
}


/* Producer thread. Calls for a scan of the WIFI networks (to add them
 * in the queue). Then, calculates the sleeping time until next scan to
 * respect the timestep (t) and sleeps. */
void *producer (void *q){
	queue *fifo;
	//timestamp handlers
	long stamp = 0, sleepTime, temp[2]={0,0}, start2, feedback, last=0, runTime=0, fullSleepTime=0;

	fifo = (queue *)q;
	//get start time (just before the 1st scan)
	gettimeofday(&tv,NULL);
	start = 1000000 * tv.tv_sec + tv.tv_usec;
	start2 = 1000000 * tv.tv_sec + tv.tv_usec;

	//initialize sleeping time to the standart timestep
	sleepTime = t;

	//forever running loop
	while (1){

		//fix timestep of the scans
		sleepTime = sleepTime + runTime;

		//get time bofore the scan
		gettimeofday(&tv,NULL);
		temp[0] = 1000000 * tv.tv_sec + tv.tv_usec;

		//take the scan and its timestamp
		last = scanWIFI(q);

		//get time after the scan
		gettimeofday(&tv,NULL);
		temp[1] = (1000000 * tv.tv_sec + tv.tv_usec);
		//time needed for one scan
		runTime = temp[1] - temp[0];

		//sub scan's time from the timestep to get the sleepTime
		sleepTime = sleepTime - runTime;
		//calculate feedback to update sleepTime in this iteration
		feedback = last - start2;
		//update sleepTime
		sleepTime = sleepTime - feedback;

		//start2 shows next desired timestamp
		start2 = last + t;
		//limit sleepTime to be positive
		if(sleepTime < 0){sleepTime  = 0;}

		//uncomment following line to print the real time passed since first scan (from the start timestamp)
//		printf("realtime = %ld \n",last-start);

		//sleep to wait next scan
		usleep (sleepTime);
		//total idle time of the program (needed for energy analysis)
		fullSleepTime = fullSleepTime + sleepTime;

	}
	//uncomment following line to print full sleeping time while executing
//	printf("full sleep Time = %ld",fullSleepTime);
	return (NULL);
}


/* Consumer thread. If the queue is not empty take a SSID and its
 * timestamp from the queue and adds it in the file, dealing with
 * thread synchronization. */
void *consumer (void *q){
	queue *fifo;
	int i, flag;
	char *d, temp[NameSpace];
	long stamp;

	fifo = (queue *)q;
	//forever running loop
	while (1) {
		//manage parallel threads and empty queue
		pthread_mutex_lock (fifo->mut);
		while (fifo->empty) {
			//uncomment following line to print empty queue messages
//			printf ("consumer: queue EMPTY.\n");
			pthread_cond_wait (fifo->notEmpty, fifo->mut);
		}
		//export a wifi from the queue
		queueDel (fifo, &d, &stamp);
		//manage parallel threads
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notFull);

		//add the wifi to the file (if it is new)
		i = addWIFI(d, stamp);

		//uncomment following line to print the SSIDs that the function add in the file
//		printf ("consumer: recieved %s\n", d);
	}
	return (NULL);
}


/* Initializes the queue */
queue *queueInit (void){
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);

	return (q);
}


/* Deletes the queue, destroying and letting free everything. */
void queueDelete (queue *q){
	pthread_mutex_destroy (q->mut);
	free (q->mut);
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

/* Adds a new value in the queue, dealing with overflow. */
void queueAdd (queue *q, char *in, long stamp){
	//copy the SSID to import it to the SSID queue
	strcpy(q->buff[q->tail], in);

	//copy the timestamp to import it to the timestamp queue
	q->stamps[q->tail] = stamp;

	//increase tail and check its value
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;

	q->empty = 0;

	return;
}


/* Pushes a value from the queue to the out varable. */
void queueDel (queue *q, char **out, long *stamp){
	//copy the SSID to export it from the SSID queue
	*out = &(q->buff[q->head][0]);

	//copy the timestamp to export it from the timestamp queue
	*stamp = q->stamps[q->head];

	//increase head and check its value
	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

/* Scans for Wifi networks using scanWIFI.sh script and adds their
 * SSIDs and their timestamps in the queue, dealing with
 * thread synchronization. */
long scanWIFI(queue *q){
	long stamp, addTime;	//timestamp keepers
	char path[NameSpace];	//string with ssid list
	FILE *fp;				//pipe for search script

	// Open the command for reading.
	fp = popen("/bin/sh scanWIFI.sh", "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}

	//get timestamp of the scan
	gettimeofday(&tv,NULL);
	//write timestamp in usecond
	stamp = 1000000 * tv.tv_sec + tv.tv_usec;

	//read the output a line at a time and put it in the queue
	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		pthread_mutex_lock (q->mut);
		while (q->full) {
			printf ("producer: queue FULL.\n");
			pthread_cond_wait (q->notFull, q->mut);
		}

		//add the wifi with its stamp in the queue
		queueAdd (q, path, stamp);

		pthread_mutex_unlock (q->mut);
		pthread_cond_signal (q->notEmpty);

		//get timestamp that the SSID is added in the queue
		gettimeofday(&tv,NULL);
		//get time from scan to the add in the queue of the SSID
		addTime = 1000000 * tv.tv_sec + tv.tv_usec - stamp;

		//uncomment following line to print time from a scan to the add in the queue in every SSID
//		printf("addTime = %ld \n",addTime);

	}

	// close pipe
	pclose(fp);
	//return scan's timestamp
	return stamp;
}

/* Take an SSID and its timestamp and adds it in the wifile.txt. If the
 * SSID already exists writes just its new timestamp in the list. */
int addWIFI(char *d, long timeval){
	//helping string keepers for the files
	char reader[NameSpace];
	char reader2[NameSpace];
	FILE *buffer;	//buffer helping file
	int i=0;		//counter of the lines
	long position;	//offset of the pointer's position

	//open a buffer local helping file
	buffer = fopen("buffer.txt", "w+");
	if(buffer == NULL){
		printf("Error while openning the buffer.txt!");
		exit(1);
	}

	//go to the start of the file
	rewind(wifile);
	while(wifile!=NULL && fgets(reader, sizeof(reader), wifile)!=NULL){
		i++;	//counter of lines

		if(strcmp(reader, d)==0){	//match on the SSID name
			//find the position in the SSID's match
			position = ftell(wifile);
			//go to the line after the matching SSID
			fseek(wifile, position, SEEK_SET);
			//copy the rest of the file to a temp file
			while(wifile!=NULL && fgets(reader2, sizeof(reader2), wifile)!=NULL){
				fprintf(buffer,"%s",reader2);
				fflush(buffer);
			}
			//go again to the matching position to add the new timestamp
			fseek(wifile, position, SEEK_SET);
			//add the new tmestamp below the matched SSID
			fprintf(wifile,"	%ld\n",timeval);
			fflush(wifile);	//update the file
			//go to the start of the temp file
			rewind(buffer);
			//restore the file below the additional timestamp
			while(buffer!=NULL && fgets(reader2, sizeof(reader2), buffer)!=NULL){
				fprintf(wifile,"%s",reader2);
			}
			//close the file
			fclose(buffer);
			return i;

		}
	}
	//the current SSID is new
	//just add SSID and timestamp in the end of the file
	fprintf(wifile,"%s	%ld \n",d, timeval);	//write the WIFI in the file
	fflush(wifile);								//update the file before fclose
	//close the file
	fclose(buffer);
	return 0;

}
