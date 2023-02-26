#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "queue.h"
#include "string-queue.h"
#include "CPUtracker.h"

//Global declarations
static struct Queue *queue;	// Queue for data transfer from Reader to Analyzer.
static CPU_stats prev;		// CPU stats from previous iteration.
static double usage[10][5];	// Queue for data transfer from Analyzer to Printer.
static unsigned char usage_front;	
static unsigned char usage_rear;
static string_Queue log_queue;	// Queue for data transfer to Logger.
static char is_working[5];	// Info for Watchdog about threads.
static unsigned char sleep_time[5];// Time to sleep between iteration of threads.
static volatile sig_atomic_t terminate;
static unsigned char cores;  	// Number of processor's cores.
static unsigned broken_thread;  // Which thread caused Watchdog to stop the program:
				// -for testing purpose.

// SIGTERM action
void term(){
    terminate = 1;
}

void *Reader(){
   FILE *proc_stat;
   char cpu_num[255];
   int count; 			// cpu_counter
   while(!terminate){
      is_working[0] = 2;
      sleep(sleep_time[0]);
      if(isFull(queue)){
         enqueueString(&log_queue, "Reader: data queue is full.");
         continue;
      }
      proc_stat= fopen("/proc/stat", "r");
      if(proc_stat != NULL){
         CPU_stats cpu = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
            {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};   
         for(count = 0; count<cores; ++count){
      	    fscanf(proc_stat,"%s ", cpu_num);
	    fscanf(proc_stat,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",&(cpu.user[count]),
	       &(cpu.nice[count]),&(cpu.system[count]),&(cpu.idle[count]),
	       &(cpu.iowait[count]),&(cpu.irq[count]),&(cpu.softirq[count]),
	       &(cpu.steal[count]),&(cpu.guest[count]),&(cpu.guest_nice[count]));
         }
	 enqueue(queue, cpu);         
      }
      fclose(proc_stat);
   }
   enqueueString(&log_queue, "Reader quiting.");   
   return NULL;
}

void *Analyzer(){
   prev = dequeue(queue);
   while(!terminate){
      is_working[1] = 2;
      sleep(sleep_time[1]);
      if(isEmpty(queue)) {
         enqueueString(&log_queue, "Analyzer: data queue is empty.");
         continue;
      }
      if(usage[(usage_rear + 1) % 10][0] > 0){
         enqueueString(&log_queue, "Analyzer: usage queue is full.");
         continue;
      }
      CPU_stats cpu = dequeue(queue);
      int count;
      usage_rear = (usage_rear + 1) % 10;
      for(count = 0; count<cores; ++count){
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
   enqueueString(&log_queue, "Analyzer quiting.");   
   return NULL; 
}

void *Printer(){
   int count;
   while(!terminate){
      is_working[2] = 2;
      sleep(sleep_time[2]);
      printf("\n");
      if(usage[usage_front][0] <= 0){
         enqueueString(&log_queue, "Printer: usage queue is empty.");
         continue;
      }
      for(count = 0; count<cores; ++count){
         printf("cpu %d usage: %f %%\n", count, usage[usage_front][count]);
         usage[usage_front][count] = 0;
      }
      usage_front = (usage_front + 1) % 10;
   }
   enqueueString(&log_queue, "Printer quiting.");   
   return NULL;
}

void *Watchdog(){
   unsigned i;
   while(!terminate){
      sleep(sleep_time[3]);
      for(i = 0; i<4; ++i){
         if(i == 3)
            ++i;
         if(is_working[i] == 0){
            terminate = 1;
            broken_thread = i;
            enqueueString(&log_queue, "Watchdog: following thread is not working:");  
            char c[2]; 
            sprintf(c, "%u", i);
            enqueueString(&log_queue, c);
         }
         --is_working[i];
      }
   }
   enqueueString(&log_queue, "Watchdog quitting.");   
   return NULL;
}

void *Logger(){
   FILE *log = fopen("log", "w");
   time_t start = time(NULL);
   while(!terminate){
      is_working[4] = 2;
      char *message = dequeueString(&log_queue);
      if(message[0] != '\0'){
         fprintf(log, "%lld: %s\n", (long long)(time(NULL)-start), message);
      }
      sleep(sleep_time[4]);
   }
   sleep(sleep_time[4]);
   is_working[4] = 2;
   for(int i = 0; i<4; ++i){
      char *message = dequeueString(&log_queue);
      if(message[0] != '\0'){
         fprintf(log, "%lld: %s\n", (long long)(time(NULL)-start), message);
      }
   }
   fprintf(log, "%lld: Logger quitting.\n", (long long)(time(NULL)-start));
   fclose(log); 
   return NULL;
}

void get_CPU_number(void){
   FILE *proc_stat;
   char cpu_num[255];
   cores = 0;
   CPU_stats cpu = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},
      {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};  
   proc_stat= fopen("/proc/stat", "r");
   if(proc_stat != NULL){
      for(int count = 0; count<17; ++count){
      	 fscanf(proc_stat,"%s ", cpu_num);
	 fscanf(proc_stat,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",&(cpu.user[count]),
	    &(cpu.nice[count]),&(cpu.system[count]),&(cpu.idle[count]),
	    &(cpu.iowait[count]),&(cpu.irq[count]),&(cpu.softirq[count]),
	    &(cpu.steal[count]),&(cpu.guest[count]),&(cpu.guest_nice[count]));
	 if(cpu_num[0] == 'c' && cpu_num[1] == 'p' && cpu_num[2] == 'u') 
	    ++cores;
	 else break;
      }     
   }
   printf("CPU_cores = %u", cores-1);
   fclose(proc_stat);
}

unsigned get_broken_thread(void){
   return broken_thread;
}

void setup(unsigned char time[5]){
   queue = createQueue(10);
   setupQueue(&log_queue);
   for(int i = 0; i<10; ++i){
      for(int j = 0; j<5; ++j){
         usage[i][j] = 0;
      }
   }
   usage_front = 0;
   usage_rear = 9;
   terminate = 0;   
   for(int i = 0; i<5; ++i){
      is_working[i] = 2;
      sleep_time[i] = time[i];
   }
   get_CPU_number();
   broken_thread = 0;
   enqueueString(&log_queue, "Setup done.");
}

void end(void){
   free(queue->cpu);
   free(queue);
   printf("\nMemory freed.\n");
}
