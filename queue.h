struct CPU_stats{
   unsigned long user[17];
   unsigned long nice[17];
   unsigned long system[17];
   unsigned long idle[17];
   unsigned long iowait[17];
   unsigned long irq[17];
   unsigned long softirq[17];
   unsigned long steal[17];
   unsigned long guest[17];
   unsigned long guest_nice[17];
};
typedef struct CPU_stats CPU_stats;

struct Queue{
   unsigned front, rear, size;
   unsigned capacity;
   CPU_stats *cpu;
};

struct Queue* createQueue(unsigned capacity);

int isFull(struct Queue* queue);

int isEmpty(struct Queue* queue);

void enqueue(struct Queue* queue, CPU_stats item);
 
struct CPU_stats dequeue(struct Queue* queue);
