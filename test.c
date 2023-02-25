#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "CPUtracker.h"

int main()
{
   setup();
   
   struct sigaction action;
   memset(&action, 0, sizeof(struct sigaction));
   action.sa_handler = term;
   sigaction(SIGTERM, &action, NULL);
   
   pthread_t reader_id, logger_id;
   
   pthread_create(&reader_id, NULL, Reader, (void *)reader_id);
   pthread_create(&logger_id, NULL, Logger, (void *)logger_id);
   
   pthread_join(reader_id, NULL);
   pthread_join(logger_id, NULL);

   end();
   return 0;
}
