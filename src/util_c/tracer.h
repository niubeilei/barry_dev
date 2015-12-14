////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tracer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_Tracer_h
#define Omn_AosUtil_Tracer_h

#include "util_c/modules.h"
#include "KernelSimu/jiffies.h"

struct aosKernelApiParms;

typedef enum AosLogLevel
{
	eAosLogLevel_Start = 0,

	eAosLogLevel_NoLog,
	eAosLogLevel_Minimum,
	eAosLogLevel_Production,
	eAosLogLevel_Engineering,
	eAosLogLevel_Debug,

	eAosLogLevel_End
} aos_log_level_e;

static inline int aos_log_level_check(aos_log_level_e level)
{
	return (level > eAosLogLevel_Start &&
			level < eAosLogLevel_End)?1:0;
}

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_log_init(void);
extern int aos_tracer_set_filter(
			aos_module_id_e module,
			const aos_log_level_e level);

extern char *aos_log_get_msg(char *fmt, ...);
extern int aos_create_log_entry(const char *file, 
					const int line, 
					int level, 
					int module, 
					char *msg);
extern int AosTracer_setFilterCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int AosTracer_showCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

#ifdef CONFIG_AOS_INMEMORY_LOG
extern int AosTracer_addInMemEntry(const char *entry);
extern int AosTracer_printLogCli(char *data,
		    unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int AosTracer_setStatusCli(char *data,
		    unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int AosTracer_resetCli(char *data,
		    unsigned int *length,
			struct aosKernelApiParms *parms,
			char *errmsg,
			const int errlen);
extern int AosTracer_setWrapCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen);
extern int AosTracer_retrieveCli(char *data,
		      		   unsigned int *length,
			       	   struct aosKernelApiParms *parms,
				       char *errmsg,
					   const int errlen);
#endif

extern void aos_trace_hex_f(
			const char *file,
			int line,
			const char *title, 
			const char *data, 
			const unsigned int data_len);

#ifdef __cplusplus
}
#endif

// 
// Kernel portion
//

#ifdef AOS_DEBUG
	#define aos_debug(format,x...)	printk(format,##x)
#else
	#define aos_debug(format,x...)	
#endif

#ifndef aos_min_log
#define aos_min_log(module, format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Minimum, \
		module, aos_log_get_msg(format, ##x))
#endif

#ifndef aos_prod_log
#define aos_prod_log(module, format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Production, \
		module, aos_log_get_msg(format, ##x))
#endif

#ifndef aos_eng_log
#define aos_eng_log(module, format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Engineering, \
		module, aos_log_get_msg(format, ##x))
#endif

#ifndef aos_debug_log
#define aos_debug_log(module, format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Debug, \
		module, aos_log_get_msg(format, ##x) )
#endif

/*
#ifndef aos_debug
#define aos_debug(format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Debug, \
		eAosMD_Platform, aos_log_get_msg(format, ##x) )
#endif
*/

#ifndef aos_trace
#define aos_trace(format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Debug, \
		eAosMD_Tracer, aos_log_get_msg(format, ##x) )
#endif

#ifndef aos_ssldebug
#define aos_ssldebug(format, x...) \
	aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Debug, \
		eAosMD_SSL, aos_log_get_msg(format, ##x) )
#endif



//
// Chen Ding, 04/30/2006
//
#ifndef aos_t_trace
#define aos_t_trace(timestamp, diff, format, x...) \
    if (jiffies - (timestamp) >= (diff)) \
    { \
        aos_create_log_entry(__FILE__, __LINE__, eAosLogLevel_Minimum, \
        		eAosMD_Tracer, aos_log_get_msg(format, ##x) ); \
		(timestamp) = jiffies; \
	}
#endif



#ifndef aos_trace_hex
#define aos_trace_hex(title, data, len) aos_trace_hex_f(__FILE__, __LINE__, title, data, len)
#endif

#endif

