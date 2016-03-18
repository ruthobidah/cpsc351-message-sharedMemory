/*g++ -o sender sender.c && ./sender  sender.txt */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "msg.h"
#include <unistd.h>
#include <signal.h>

// The size of the shared memory chunk 
#define SHARED_MEMORY_CHUNK_SIZE 1000

// The ids for the shared memory segment and the message queue 
int shmid, msqid;
 
// The pointer to the shared memory 
char* sharedMemPtr;


void init(int& shmid, int& msqid, char*& sharedMemPtr)
{     
   key_t key;
   if ((key = ftok("keyfile.txt", 'Z')) == -1) {
   perror("ftok");
   exit(1);}

   //Get the id of the shared memory segment.
   if ((shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0644 | IPC_CREAT)) == -1) {
      perror("shmget");
      exit(1);
   }

   // attach to the shared memory 
   sharedMemPtr = (char*)shmat(shmid, (void *)0, 0);
   if (sharedMemPtr == (char *)(-1)) {
      perror("shmat");
      exit(1);
   }
   // Attach to the message queue 
   if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }
}

void send(const char* fileName)
{
   // Open the file argv[1] for reading 
   FILE* fp = fopen(fileName, "r"); 

   // Was the file open? 
   if(!fp)	{
      perror("fopen");
      exit(1);
   }

   // A buffer to store message we will send to the receiver. 
   message sendMsg;   
  
   while(!feof(fp) ){
       //critical section
      if((sendMsg.size=fread(sharedMemPtr, 1, SHARED_MEMORY_CHUNK_SIZE, fp))<0){
	      perror("fread");
	      exit(1);
      }  
      printf("send size:%d\n",sendMsg.size);  
      //set type = 1 to wake up receiver.
      sendMsg.mtype = SENDER_DATA_TYPE;  
      //send message
      if (msgsnd(msqid, &sendMsg, sizeof(int), 0) == -1){ // +1 for '\0' 
         perror("msgsnd");
         exit(1);
      }
      if (  msgrcv(msqid, &sendMsg, sizeof(int), 2, 0)  == -1) {
         perror("msgrcv");
         exit(1);
      }
}

   //it's done
   sendMsg.size = 0;  
   sendMsg.mtype = SENDER_DATA_TYPE;  
   //send message
   if (msgsnd(msqid, &sendMsg, sizeof(int), 0) == -1) {// +1 for '\0' 
      perror("msgsnd");
      exit(1);
   }   
   fclose(fp);
}


int main(int argc, char *argv[])
{
   // Connect to shared memory and the message queue 
   init(shmid, msqid, sharedMemPtr);

   // Send the file 
   send(argv[1]);

   // no need to Cleanup, let receiver do it
   // if clean here, sender will destroy the shared memory.
   //cleanUp(shmid, msqid, sharedMemPtr);

   return 0;
}
