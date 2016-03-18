/*g++ -o recv recv.c && ./recv  */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include <unistd.h>
#include <signal.h>
/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
char *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile.txt";

void init(int& shmid, int& msqid, char*& sharedMemPtr)
{
	
   key_t key;
   if ((key = ftok("keyfile.txt", 'Z')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644 | IPC_CREAT)) == -1) {
      perror("shmget");
      exit(1);
   }

   /* attach to the shared memory */
   sharedMemPtr = (char*)shmat(shmid, (void *)0, 0);
   if (sharedMemPtr == (char *)(-1)) {
      perror("shmat");
      exit(1);
   }

   // Attach to the message queue 
   if ((msqid = msgget(key, 0644)) == -1) {  
      perror("msgget");
      exit(1);
   }
}


/**
 * The main loop
 */
void mainLoop()
{
   /* The size of the mesage */
   int msgSize = 1;

   /* Open the file for writing */
   FILE* fp = fopen(recvFileName, "w+");

   /* Error checks */
   if(!fp)	{		
      perror("fopen");			
      exit(-1);	
   }

   // A buffer to store message we will send to the sender. */
   message revMsg;   
   //msgSize is for teminate purpose
   msgSize=1;

   while(msgSize!= 0) {	 

      /*recv message*/
      if (msgrcv(msqid, &revMsg, sizeof(int), 1, 0) == -1) {
         perror("msgrcv");
         exit(1);
      }     
      printf("receive size:%d\n",revMsg.size);   

      if(msgSize!=0){ //prevent deadlock
         msgSize=revMsg.size;
         // Save the shared memory to file */
         if(fwrite(sharedMemPtr, sizeof(char),  revMsg.size ,fp) < 0)	{
            perror("fwrite");
            exit(1);
         }
         /* We are done reading*/
         revMsg.mtype = RECV_DONE_TYPE;  
         if (msgsnd(msqid, &revMsg, sizeof(int), 0) == -1){  
            perror("msgsnd");
            exit(1);
         }  
      }
   }
   /* Close the file */
   fclose(fp);
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
   // Deallocate the message queue */
   if (msgctl(msqid, IPC_RMID, NULL) == -1) {//remove a message queue 
      perror("msgctl");
      exit(1);
   }

   //  Detach from shared memory */
   if (shmdt(sharedMemPtr) == -1) {
      perror("shmdt");
      exit(1);
   }
   // detach from the segment: */
   if(shmctl(shmid, IPC_RMID, NULL)==-1){
      perror("shmctl");
      exit(1);
   }

   sharedMemPtr=NULL;
}

/*
 * Handles the exit signal
 */

void signalHandlerFunc(int signo){
   printf("I am in signal handle\n");  
	/* let Ctrl-C deallocate memory and message queue */
   cleanUp(shmid, msqid, sharedMemPtr);
   exit(0);
}

int main(void)
{
	// Overide the default signal handler for the SIGINT signal with signalHandlerFunc
	signal(SIGINT, signalHandlerFunc); 
				
	/* Initialize */
	init(shmid, msqid, sharedMemPtr);
	
	/* Go to the main loop */
	mainLoop();

   printf("i am returned to main\n"); 
   for(;;);

   return 0;
}
