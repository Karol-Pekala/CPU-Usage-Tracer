#include <stdio.h>

struct CPU_stats{
   unsigned long user;
   unsigned long nice;
   unsigned long system;
   unsigned long idle;
   unsigned long iowait;
   unsigned long irq;
   unsigned long softirq;
   unsigned long steal;
   unsigned long guest;
   unsigned long guest_nice;
};

void main()
{
   FILE *proc_stat;
   
   struct CPU_stats cpu[5];
   char cpu_num[255];
   int count = 0; // cpu_counter
   
   proc_stat= fopen("/proc/stat", "r");
   if(proc_stat != NULL){
   	do {
   	   fscanf(proc_stat,"%s ", cpu_num);
	   fscanf(proc_stat,"%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld ",
	   &(cpu[count].user),&(cpu[count].nice),&(cpu[count].system),&(cpu[count].idle),
	   &(cpu[count].iowait),&(cpu[count].irq),&(cpu[count].softirq),
	   &(cpu[count].steal),&(cpu[count].guest),&(cpu[count].guest_nice));
	   printf("%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld \n",
	   cpu[count].user,cpu[count].nice,cpu[count].system,cpu[count].idle,
	   cpu[count].iowait,cpu[count].irq,cpu[count].softirq,
	   cpu[count].steal,cpu[count].guest,cpu[count].guest_nice);
           ++count;
        } while(count<5); 
   }
   fclose(proc_stat);
   return;
}
