#include <string.h>
#include "string-queue.h"

void setupQueue(string_Queue* queue){
   queue->front = queue->size = 0;
   queue->rear = 15;
}

int isFullString(string_Queue* queue){
   return (queue->size == 16);
}

int isEmptyString(string_Queue* queue){
   return (queue->size == 0);
}

void enqueueString(string_Queue* queue, char *log){
   if (isFullString(queue))
      return;
   queue->rear = (queue->rear + 1) % 16;
   queue->logs[queue->rear] = log;
   queue->size = queue->size + 1;
}
 
char* dequeueString(string_Queue* queue){
   if (isEmptyString(queue)){
      char *dummy = "\0";
      return dummy;
   }
   char *log = queue->logs[queue->front];
   queue->front = (queue->front + 1) % 16;
   queue->size = queue->size - 1;
   return log;
}
