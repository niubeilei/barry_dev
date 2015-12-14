////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBridge.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_core_bridge_h
#define aos_aos_core_bridge_h

#include "aos/aosCoreComm.h"
#include "aos/aosFwdDev.h"
#include "aos/KernelEnum.h"
#include "aos/KernelApiStruct.h"


struct net_device;
struct aosMultiCaster;
struct sk_buff;
extern int aosBridgeStatus;


struct aosBridgeDef
{
	unsigned char			mBridgeId;
	unsigned short			mFlags;
	struct aosBridgeDef *	mPeer;
	int						mNumDevs;
	struct aosFwdDev *		mDevInfo;
	struct aosFwdDev *		mCrtDev;
	unsigned int			mCrtHit;
	unsigned int			mCrtWeight;
	struct aosMultiCaster *	mCaster;
	struct aosPktStat		mStat;
	int (*mForward)(struct aosBridgeDef *self, struct sk_buff *skb);
};


struct aosKernelApiParms;

extern int aosBridge_init(void);
extern int aosBridge_resetBridge(struct aosBridgeDef *self);
extern int aosBridge_clearAll(const int lock, char *, const int);
extern int aosBridge_clearAllDev(struct aosBridgeDef *bridge);
extern int aosBridge_isDefined(int bridgeId);
extern int aosBridge_create(unsigned char bridgeId,
					char *method,
					char *names1,
					int weight1,
					char *names2, 
					int weight2);
extern struct aosBridgeDef *aosBridge_constructor(
					unsigned char bridgeId,
                    unsigned short flags,
					char *method,
					char *name,
                    int weight,
                    int *ret);

extern int aosBridge_resetCli(char *data, 
					unsigned int *length, 
				    struct aosKernelApiParms *parms, 
					char *errmsg, 
					const int errlen);
extern int aosBridge_statusCli(char *data, 
					unsigned int *length, 
				    struct aosKernelApiParms *parms, 
					char *, 
					const int);
extern int aosBridge_clearAllCli(char *data, 
					unsigned int *length, 
				    struct aosKernelApiParms *parms, 
					char *, 
					const int);
extern int aosBridge_removeCli(char *data, 
					unsigned int *optlen, 
				    struct aosKernelApiParms *parms, 
					char *, 
					const int);
extern int aosBridge_addDevCli(char *data, 
					unsigned int *length, 
				    struct aosKernelApiParms *parms, 
					char *, 
					const int);
extern int aosBridge_delDevCli(char *data, 
					 unsigned int *optlen, 
				     struct aosKernelApiParms *parms, 
					 char *, 
					 const int);
extern int aosBridge_showCli(char *data, 
					 unsigned int *optlen, 
				     struct aosKernelApiParms *parms, 
					 char *, 
					 const int);
extern int aosBridge_healthcheckCli(char *data,
					 unsigned int *optlen, 
				     struct aosKernelApiParms *parms, 
					 char *, 
					 const int);
extern int aosBridge_createKapiCli(char *data, 
					 unsigned int *optlen, 
				     struct aosKernelApiParms *parms, 
					 char *, 
					 const int);
extern int aosBridge_saveConfigCli(char *data, 
					 unsigned int *length, 
					 struct aosKernelApiParms *parms,
					 char *, 
					 const int);

extern int aosBridge_destructor(struct aosBridgeDef *bridge);
extern int aosBridge_remove(int bridgeId);
extern struct aosBridgeDef *aosBridge_get(int bridgeId, int flags);
extern int aosBridge_addDev(int bridgeId,
                    const char *name,
                    int weight,
                    const char *side);
extern int aosBridge_delDev(int bridgeId, 
                    char *flags,
                    const char *name);
extern void aosBridge_enableBridging(void);
extern void aosBridge_disableBridging(void);
extern void aosBridge_isBridgingOn(int *value);
extern int aosBridge_list(char *data,
					 unsigned int *optlen, 
					 char *, 
					 const int);
extern int aosBridge_show(char *data,
					 unsigned int *optlen, 
				     struct aosKernelApiParms *parms, 
					 char *, 
					 const int);
extern int aosBridge_show1(struct aosBridgeDef *bridge,
                     unsigned int *index,
                     char *result,
                     unsigned int length);
extern int aosBridge_saveBridge(struct aosBridgeDef *bridge, 
					 char *rsltBuff, unsigned int *rsltIdx, 
					 unsigned int len); 
extern int aosBridge_healthcheck(char *buffer, 
					 unsigned int *index, 
					 const unsigned int buflen);
extern int aosBridge_hcOnBridge(struct aosBridgeDef *bridge,
					 char *buffer, unsigned int *index, 
					 const unsigned int buflen); 
extern int aosBridge_hcOnOneSide(struct aosBridgeDef *bridge,
					 char *buffer, unsigned int *index, 
					 const unsigned int buflen); 

extern int aosBridge_procWrr(struct aosBridgeDef *self, struct sk_buff *skb);
extern int aosBridge_procWrrIpf(struct aosBridgeDef *self, struct sk_buff *skb);
extern int aosBridge_proc(struct sk_buff *skb);
#endif
