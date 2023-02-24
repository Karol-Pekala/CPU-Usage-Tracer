#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "queue.h"
#include "CPUtracker.h"

int main()
{
   setup();
   
   struct sigaction action;
   memset(&action, 0, sizeof(struct sigaction));
   action.sa_handler = term;
   sigaction(SIGTERM, &action, NULL);
   
   pthread_t reader_id, analyzer_id, printer_id, watchdog_id;
   
   pthread_create(&reader_id, NULL, Reader, (void *)reader_id);
   pthread_create(&analyzer_id, NULL, Analyzer, (void *)analyzer_id);
   pthread_create(&printer_id, NULL, Printer, (void *)printer_id);
   pthread_create(&watchdog_id, NULL, Watchdog, (void *)watchdog_id);
   
   pthread_join(reader_id, NULL);
   pthread_join(analyzer_id, NULL);
   pthread_join(printer_id, NULL);
   pthread_join(watchdog_id, NULL);

   end();
   return 0;
}
