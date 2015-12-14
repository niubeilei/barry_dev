////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCorePcp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_core_corePcp_h
#define aos_aos_core_corePcp_h

#include "aos/KernelEnum.h"
struct aosRule;
struct sk_buff;

struct aosKernelApiParms;
extern struct aosRule *aosNetInputPcp_getRule(int ruleId, int lockflag);
extern void aosNetInputPcp_remove(struct aosRule *rule, int lockflag);
extern int 	aosNetInputPcp_proc(struct sk_buff *skb);
extern void aosNetInputPcp_lock(struct aosRule ***pcpdata);
extern void aosNetInputPcp_unlock(void);
extern int	aosNetInputPcp_delRule(int ruleId);

extern int aosCorePcp_statusCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosCorePcp_showPcpCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);


extern int 	aosNetworkInputPcpStatus;

#endif
