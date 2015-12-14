#ifndef QOS_UTIL_H
#define QOS_UTIL_H
#include "aosUtil/Tracer.h"
#define QOS_GLOBAL_DEBUG
#define QOS_MODULE_DEBUG
#define QOS_TRAFFIC_DEBUG
//#define QOS_RULE_DEBUG

#ifdef QOS_MODULE_DEBUG
#define QOS_MODULE_PRINTK(format, x...) aos_trace(format, ##x);
#else
#define QOS_MODULE_PRINTK(format, x...) {\
								 }
#endif

#ifdef QOS_TRAFFIC_DEBUG
#define QOS_TRAFFIC_PRINTK(format, x...) aos_trace(format, ##x);

#else
#define QOS_TRAFFIC_PRINTK(format, x...) {\
								 }
#endif
#ifdef QOS_RULE_DEBUG
#define QOS_RULE_PRINTK(format, x...) aos_trace(format, ##x);
#else
#define QOS_RULE_PRINTK(format, x...) {\
	                                    }
#endif
#ifdef QOS_RULE_DEBUG
#define QOS_RULE_MALLOC(p,size,flag,stat) {\
											p = kmalloc(size,flag);\
											if (p != NULL) {\
												stat += size;\
											}\
	                                      }
#else
#define QOS_RULE_MALLOC(p,size,flag,stat) {\
											p = kmalloc(size,flag);\
										  }
#endif
#ifdef QOS_RULE_DEBUG
#define QOS_RULE_FREE(p,size,stat) {\
										kfree(p);\
										stat -= size;\
									}
#else
#define QOS_RULE_FREE(p,size,stat) {\
										kfree(p);\
									}
#endif
#define QOS_FREE_IF_POINTER(p) {\
							   if (p != NULL) \
							   		kfree(p);\
								p = NULL;    \
	                    	   }

#ifndef QOS_GET_JIFFIES

#ifdef __KERNEL__
#define QOS_GET_JIFFIES() jiffies 
#else
#define QOS_GET_JIFFIES() qos_get_time()
#include <sys/time.h>
#include <time.h>
#include "aosUtil/Types.h"

static inline u32 qos_get_time()
//static inline unsigned long qos_get_time()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
//	aos_trace("sec:%u,usec:%u",tv.tv_sec,tv.tv_usec);
	return tv.tv_sec*1000+tv.tv_usec/1000;
//	return tv.tv_sec*10000+tv.tv_usec/100;
}
#endif // #ifdef __KERNEL__

#endif // #ifndef QOS_GET_JIFFIES

#endif // #ifndef QOS_UTIL_H
