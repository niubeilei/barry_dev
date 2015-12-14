////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMacCtlr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosMacCtlr_h
#define aos_core_aosMacCtlr_h


struct sk_buff;
struct aosRule;

extern int aosMacCtlr_proc(struct sk_buff *skb);
extern int aosMacCtlrFlag;
extern int aosMacCtlr_init(void);

// 
// Rules
//
extern int aosMacCtlr_macFilterRule(struct aosRule *self, struct sk_buff *skb);
extern int aosMacCtlr_macNoAosRule(struct aosRule *self, struct sk_buff *skb);
extern int aosMacCtlr_procMacRule(struct aosRule *self, struct sk_buff *skb);

extern int aosMacCtlr_addCli(char *data, unsigned int *length);
extern int aosMacCtlr_delCli(char *data, unsigned int *length);
extern int aosMacCtlr_statusCli(char *data, unsigned int *length);
extern int aosMacCtlr_showCli(char *data, unsigned int *length);

#endif

