////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DenyPage.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AppProxy_Deny_PAGE_h
#define AppProxy_Deny_PAGE_h


#include "aos/aosReturnCode.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Alarm.h"

enum
{
	eAosMaxDenyPages = 256,
	eAosDenyPageMaxLength = 5000,
	eAosDenyPageMaxName = 16,
	eAosDenyPageMaxPathLen = 256,
};



struct aos_deny_page
{
	
	char page_name[eAosDenyPageMaxName];
	char page_path[eAosDenyPageMaxPathLen];
	char *page_contents;	
};


enum eAosDenyPageReturnCode
{
	eAosRc_DenyPageReinit = eAosRc_DenyPageModuleStart+1,
	eAosRc_DenyPageMaxExceed,
	eAosRc_NoDenyPageName,
	eAosRc_NoDenyPageContent,
	eAosRc_DenyPageNameExceed,
	eAosRc_DenyPageLengthExceed,
	eAosRc_DenyPagePoolMem,
	aAosRc_DenyPageNullPointer,
	eAosRc_DenyPageNotFound,
	eAosRc_DenyPageTcpVsNameExceed,
	eAosRc_DenyPageTcpVsNotFound,

};

enum eAosDenyPageAlarmCode
{
	eAosAlarm_DenyPageMaxExceed = eAosAlarm_DenyPageStart + 1,
	eAosAlarm_NoDenyPageName,
	eAosAlarm_NoDenyPageContent,
	eAosAlarm_DenyPageNameExceed,
	eAosAlarm_DenyPageLengthExceed,
	eAosAlarm_DenyPagePoolMem,
	aAosAlarm_DenyPageNullPointer,
	eAosAlarm_DenyPageNotFound,
	eAosAlarm_DenyPageTcpVsNameExceed,
	eAosAlarm_DenyPageTcpVsNotFound,

};

struct aosKernelApiParms;
extern int aos_deny_page_init(void);
extern int aos_deny_page_add(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen);
extern int aos_deny_page_remove(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen);
extern int aos_deny_page_show(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen);
extern int aos_deny_page_assign(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen);
extern int aos_deny_page_deassign(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen);
extern int aos_deny_page_save_config(char *data, 
			unsigned int *length, 
		   	struct aosKernelApiParms *parms,
			char *errmsg, 
			const int errlen);
extern int aos_deny_page_clearall_config(char *data, 
		  	unsigned int *length, 
			struct aosKernelApiParms *parms,
			char *errmsg, 
			const int errlen);

#endif

