#ifndef KernelSimu_wait_h
#define KernelSimu_wait_h

#ifdef __KERNEL__
#include <linux/wait.h>
#else
// Userland part

/*
struct __wait_queue_head {
	spinlock_t lock;
	struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;
*/
typedef int wait_queue_head_t;
#define wait_event_interruptible(a,b) 
#define init_waitqueue_head(a)
#define wake_up_interruptible(a)

#endif
#endif
