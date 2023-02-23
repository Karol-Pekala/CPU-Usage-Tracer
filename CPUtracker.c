#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "queue.h"

// Global declarations
static struct Queue *queue;	//Queue for data transfer from Reader to Analyzer.
static CPU_stats prev;	//CPU stats from previous iteration, for calculating usage.
static double usage[10][5];	//Queue for data transfer from Analyzer to Printer.
static unsigned int usage_front;	
static unsigned int usage_rear;
static volatile sig_atomic_t terminate;

// SIGTERM action
static void term(){
    terminate = 1;
}

static void *Reader(){
   FILE *proc_stat;
   char cpu_num[255];
   int count; 			// cpu_counter
   
   while(!terminate){
      if(isFull(queue)){
         printf("\nqueue is full\n");
         sleep(1);
         continue;
      }
      proc_stat= fopen("/proc/stat", "r");
      if(proc_stat != NULL){
         CPU_stats cpu = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
            {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};   
         for(count = 0; count<5; ++count){
      	    fscanf(proc_stat,"%s ", cpu_num);
	    fscanf(proc_stat,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",
	       &(cpu.user[count]),&(cpu.nice[count]),&(cpu.system[count]),&(cpu.idle[count]),
	       &(cpu.iowait[count]),&(cpu.irq[count]),&(cpu.softirq[count]),
	       &(cpu.steal[count]),&(cpu.guest[count]),&(cpu.guest_nice[count]));
         }
	 enqueue(queue, cpu);         
      }
      fclose(proc_stat);
      sleep(1);
   }
   printf("\nReader quiting.\n");   
   return NULL;
}

static void *Analyzer(){
   sleep(1);
   prev = dequeue(queue);
   while(!terminate){
      sleep(1);
      if(isEmpty(queue)) {
         printf("\nqueue is empty\n");
         continue;
      }
      if(usage[(usage_rear + 1) % 10][0] > 0){
         printf("\narray is full\n");
         continue;
      }
      CPU_stats cpu = dequeue(queue);
      int count;
      usage_rear = (usage_rear + 1) % 10;
      for(count = 0; count<5; ++count){
	 unsigned long PrevIdle = prev.idle[count] + prev.iowait[count];
	 unsigned long Idle = cpu.idle[count] + cpu.iowait[count];
	 unsigned long PrevNonIdle = prev.user[count] + prev.nice[count] + prev.system[count] + 
	   prev.irq[count] + prev.softirq[count] + prev.steal[count];
	 unsigned long NonIdle = cpu.user[count] + cpu.nice[count] + cpu.system[count] + 
	   cpu.irq[count] + cpu.softirq[count] + cpu.steal[count];
	 unsigned long  PrevTotal = PrevIdle + PrevNonIdle;
	 unsigned long  Total = Idle + NonIdle;

	 usage[usage_rear][count] = (double)(Total - PrevTotal - Idle + PrevIdle)/
	    (double)(Total - PrevTotal)*100;
      }
      prev = cpu;
   }
   printf("\nAnalyzer quiting.\n");   
   return NULL; 
}

static void *Printer(){
   int count;
   sleep(2);
   while(!terminate){
      sleep(1);
      if(usage[usage_front][0] <= 0){
         printf("\narray is empty\n");
         continue;
      }
      for(count = 0; count<5; ++count){
         printf("cpu %d usage: %f %%\n", count, usage[usage_front][count]);
         usage[usage_front][count] = 0;
      }
      printf("\n");
      usage_front = (usage_front + 1) % 10;
   }
   printf("\nPrinter quiting.\n");   
   return NULL;
}

int main()
{
   queue = createQueue(10);
   for(int i = 0; i<10; ++i){
      for(int j = 0; j<5; ++j){
         usage[i][j] = 0;
      }
   }
   usage_front = 0;
   usage_rear = 9;
   
   terminate = 0;
   struct sigaction action;
   memset(&action, 0, sizeof(struct sigaction));
   action.sa_handler = term;
   sigaction(SIGTERM, &action, NULL);
   
   pthread_t reader_id, analyzer_id, printer_id;
   
   pthread_create(&reader_id, NULL, Reader, (void *)reader_id);
   pthread_create(&analyzer_id, NULL, Analyzer, (void *)analyzer_id);
   pthread_create(&printer_id, NULL, Printer, (void *)printer_id);
   pthread_join(reader_id, NULL);
   pthread_join(analyzer_id, NULL);
   pthread_join(printer_id, NULL);

   //pthread_exit(NULL);
   free(queue);
   return 0;
}
