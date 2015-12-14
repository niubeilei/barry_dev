////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ForwardTable.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AppProc_hpptproc_ForwardTable_h
#define Aos_AppProc_hpptproc_ForwardTable_h

#include "aosUtil/CharPtree.h"
#include "AppProc/ReturnCode.h"
#include "Ktcpvs/tcp_vs_def.h"

enum
{
	eAosHttpFwdtbl_MaxEntries = 5000,
	eAosHttpFwdtbl_MaxIdLen = 300,
	eAosHttpFwdtbl_MaxUrlLen = 256,

	eAosConnType_SSL = 0,
	eAosConnType_Plain = 1
};

struct aos_http_fwdtbl_entry
{
	char 			vsname[KTCPVS_IDENTNAME_MAXLEN];
	char		    url[eAosHttpFwdtbl_MaxUrlLen+1];
	u32				addr;
	u16				port;
	u8				ssl_flag;
};


struct aos_http_fwdtbl
{
	struct aosCharPtree	*entry_tree;
	u32					num_entries;
};

struct aosKernelApiParms;
extern int aos_http_fwdtbl_lookup(char *vsname, 
					   char *url,
					   u32 *addr,
					   u16 *port,
					   int *connType);
extern int aos_http_fwdtbl_init(void);
extern int aos_fwdtbl_entry_add_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_fwdtbl_entry_del_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_fwdtbl_show_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_fwdtbl_clearall_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,      
                       const int errlen);
extern int aos_fwdtbl_saveconf_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);

#endif

