struct CPU_stats{
   unsigned long user[5];
   unsigned long nice[5];
   unsigned long system[5];
   unsigned long idle[5];
   unsigned long iowait[5];
   unsigned long irq[5];;
   unsigned long softirq[5];
   unsigned long steal[5];
   unsigned long guest[5];
   unsigned long guest_nice[5];
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
