#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>



struct CPU_stats{
   unsigned long user[5];
   unsigned long nice[5];
   unsigned long system[5];
   unsigned long idle[5];
   unsigned long iowait[5];
   unsigned long irq[5];
   unsigned long softirq[5];
   unsigned long steal[5];
   unsigned long guest[5];
   unsigned long guest_nice[5];
};

struct Queue{
   int front, rear, size;
   unsigned capacity;
   struct CPU_stats *cpu;
};

struct Queue* createQueue(unsigned capacity){
   struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
   queue->capacity = capacity;
   queue->front = queue->size = 0;
   queue->rear = capacity - 1;
   queue->cpu = (struct CPU_stats*)malloc(
      queue->capacity * sizeof(struct CPU_stats));
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

void enqueue(struct Queue* queue, struct CPU_stats item)
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
      struct CPU_stats dummy;
      return dummy;
   }
   struct CPU_stats item = queue->cpu[queue->front];
   queue->front = (queue->front + 1) % queue->capacity;
   queue->size = queue->size - 1;
   return item;
}
 

// Global declarations
struct Queue *queue;

void *Reader(){
   FILE *proc_stat;
   char cpu_num[255];
   int count; // cpu_counter
   
   proc_stat= fopen("/proc/stat", "r");
   if(proc_stat != NULL){
      for(int i = 0; i<1; ++i){
         if(isFull(queue))
            continue;
         struct CPU_stats cpu;     
         for(count = 0; count<5; ++count){
      	    fscanf(proc_stat,"%s ", cpu_num);
	    fscanf(proc_stat,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",
	       &(cpu.user[count]),&(cpu.nice[count]),&(cpu.system[count]),&(cpu.idle[count]),
	       &(cpu.iowait[count]),&(cpu.irq[count]),&(cpu.softirq[count]),
	       &(cpu.steal[count]),&(cpu.guest[count]),&(cpu.guest_nice[count]));
         }
         enqueue(queue, cpu);
      }
   }
   fclose(proc_stat);
}

void *Analyzer(){
   for(int i = 0; i<1; ++i){
      if(isEmpty(queue)) 
         continue;
      struct CPU_stats cpu = dequeue(queue);
      int count;
      for(count = 0; count<5; ++count){
   	 printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld \n",
	    cpu.user[count],cpu.nice[count],cpu.system[count],cpu.idle[count],
	    cpu.iowait[count],cpu.irq[count],cpu.softirq[count],
	    cpu.steal[count],cpu.guest[count],cpu.guest_nice[count]);
      }
   }
}

int main()
{
   queue = createQueue(20);
   
   pthread_t reader_id;
   pthread_t analyzer_id;
   Reader();
   Analyzer();
   
   /*pthread_create(&reader_id, NULL, Reader, NULL);
   pthread_create(&analyzer_id, NULL, Analyzer, NULL);

   pthread_exit(NULL);*/
   return 0;
}
