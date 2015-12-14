////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosTcpVsCntl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Ktcpvs_aosTcpVsCntl_h
#define Ktcpvs_aosTcpVsCntl_h

#include "aos/aosReturnCode.h"
#include "aosUtil/Types.h"
#include "KernelSimu/string.h"
#include "KernelSimu/aosKernelMutex.h"
#include "Ktcpvs/tcp_vs_def.h"

struct aosKernelApiParms;
struct tcp_vs_service;

// 
// KTCPVS Return Codes
//
enum AosKtcpvsReturnCode
{
	eAosRc_InvalidWeight = eAosRc_KtcpvsModuleStart + 1,
	eAosRc_RealServerExists,
	eAosRc_MissingVSName,
	eAosRc_DestNotFound,
	eAosRc_RuleListNotEmpty
};

struct aosKernelApiParms;

extern struct semaphore __tcp_vs_mutex;

extern int aos_tcpvs_add(const char *name,
               const char *sname, 
               u32 addr, 
               u16 port, 
               u8 sslFlag,
               char *errmsg);
extern int aos_tcpvs_remove(const char *name, char *errmsg);
extern int aos_tcpvs_start(const char *name, char *errmsg);
extern int aos_tcpvs_stop(const char *name, char *errmsg);

extern int aos_ktcpvs_set_autologin(const char *vsname, 
							 const char *flag,
							 char *errmsg);
extern struct tcp_vs_service *
			tcp_vs_lookup_byident(const struct tcp_vs_ident *id);
static inline struct tcp_vs_service * tcp_vs_get(const char *vsname)
{
	struct tcp_vs_ident id;

	if (strlen(vsname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		return 0;
	}

	strcpy(id.name, vsname);
	return tcp_vs_lookup_byident(&id);
};

extern int aos_ktcpvs_init(void); 
extern int aos_ktcpvs_init_cli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms,
						  char *errmsg, 
						  const int errlen);
extern int aos_ktcpvs_add_rs_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_ktcpvs_del_rs_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_ktcpvs_remove_rs_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_ktcpvs_saveAutologin(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int aos_ktcpvs_showAutologin(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
extern int tcp_vs_del_dest(struct tcp_vs_service *svc,
                __u32 daddr,
                __u16 dport,
                char *errmsg);
extern int aos_tcpvs_show_service(
			char *name,
			char *data, 
			unsigned int length,
			unsigned int *index, 
			char *errmsg);
extern int aos_vs_get_service_entries(
			char *data, 
			unsigned int length,
			unsigned int *index, 
			char *errmsg);
extern int tcp_vs_add_dest(struct tcp_vs_service *svc,
        	__u32 daddr, 
			__u16 dport, 
			int weight, 
			char *errmsg);
//extern int ktcpvs_init(void);

extern void aos_ktcpvs_saveconfig(char *buf, unsigned int *index, const unsigned int length);
extern int aos_ktcpvs_clearconfig(char *errmsg, const int errlen);
extern int aos_tcpvs_pmi(const char *name, int pmi, char *errmsg);
extern int aos_tcpvs_ssl_clientauth(const char *name, int set, char *errmsg);
extern int aos_tcpvs_ssl_front(const char *name, int set, char *errmsg);
extern int aos_tcpvs_ssl_backend(const char *name, int set, char *errmsg);

extern int aosAppProxy_pmiSysnameCli(
	char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int aosAppProxy_forwardTableCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int aosAppProxy_maxClientsCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int aosAppProxy_maxConnsCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);
extern int aosAppProxy_registerCli(void);
extern int aosAppProxy_unregisterCli(void);
#endif

