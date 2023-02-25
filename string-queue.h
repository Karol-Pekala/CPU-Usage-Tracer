struct string_Queue{
   unsigned front, rear, size;
   char *logs[16];
};
typedef struct string_Queue string_Queue;

void setupQueue(string_Queue* queue);

int isFullString(string_Queue* queue);

int isEmptyString(string_Queue* queue);

void enqueueString(string_Queue* queue, char *log);
 
char* dequeueString(string_Queue* queue);
