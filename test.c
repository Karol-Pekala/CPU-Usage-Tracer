#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "CPUtracker.h"

int main()
{
   unsigned char time[5] = {1,1,1,1,1};
   pthread_t reader_id, analyzer_id, printer_id, watchdog_id, logger_id;
   unsigned thread;
   
   for(unsigned i = 0; i<4; ++i){
      if(i == 3)
         ++i;
      time[i] = 3;
      
      setup(time);  
      struct sigaction action;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = term;
      sigaction(SIGTERM, &action, NULL);
  
      pthread_create(&reader_id, NULL, Reader, (void *)reader_id);
      pthread_create(&analyzer_id, NULL, Analyzer, (void *)analyzer_id);
      pthread_create(&printer_id, NULL, Printer, (void *)printer_id);
      pthread_create(&watchdog_id, NULL, Watchdog, (void *)watchdog_id);
      pthread_create(&logger_id, NULL, Logger, (void *)logger_id);
   
      pthread_join(reader_id, NULL);
      pthread_join(analyzer_id, NULL);
      pthread_join(printer_id, NULL);
      pthread_join(watchdog_id, NULL);
      pthread_join(logger_id, NULL);


      time[i] = 1;
      thread = get_broken_thread();
      printf("i: %u th: %u", i, thread);
      end();
      assert(i == thread);
   }   
   return 0;
}
