#ifndef SCHEDULER_H
#define SCHEDULER_H 1

void schedule(void (*func) (void), uint32_t time);
void cancel(void (*func) (void));
void init_scheduler(void);
void scheduler_iteration(void);


#endif
