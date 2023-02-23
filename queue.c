#include <stdlib.h>
#include "queue.h"
    
struct Queue* createQueue(unsigned capacity){
   struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
   queue->capacity = capacity; 
   queue->front = queue->size = 0;
   queue->rear = capacity - 1;
   queue->cpu = (CPU_stats*)malloc(
      queue->capacity * sizeof(CPU_stats));
   return queue;
}

int isFull(struct Queue* queue)
{
   return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue)
{
   return (queue->size == 0);
}

void enqueue(struct Queue* queue, CPU_stats item)
{
   if (isFull(queue))
      return;
   queue->rear = (queue->rear + 1) % queue->capacity;
   queue->cpu[queue->rear] = item;
   queue->size = queue->size + 1;
}
 
struct CPU_stats dequeue(struct Queue* queue)
{
   if (isEmpty(queue)){
      CPU_stats dummy = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
         {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
      return dummy;
   }
   CPU_stats item = queue->cpu[queue->front];
   queue->front = (queue->front + 1) % queue->capacity;
   queue->size = queue->size - 1;
   return item;
}
