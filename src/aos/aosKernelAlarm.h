////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelAlarm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelAlarm_h
#define Omn_aos_KernelAlarm_h


#define eAosMaxDebugFlag 10

extern int aosDebugFlags[eAosMaxDebugFlag];

enum 
{
	eAos_Success = 0,

	// Rated errors must start first
	eAosErrR_firstRatedErr,
	eAosErrR_invalidRatedErrId,

	eAosAlarmR_DevNull,
	eAosAlarmR_BridgedSkbNotCorrect,
	eAosAlarmR_BridgeNull,
	eAosAlarmR_EthNull,
	eAosAlarmR_ForwardedSkbNotCorrect,
	eAosAlarmR_InvalidMethod,
	eAosAlarmR_InvalidKey,
	eAosAlarmR_MemErr,
	eAosAlarmR_NoDevice,
	eAosAlarmR_NoReceiver,
	eAosAlarmR_RuleMismatch,
	eAosAlarmR_UnknownMethod,

	eAosErrR_lastRatedErr,

	eAosRc_NotFound,

	eAosErr_aosCoreInit, 
	eAosErr_aosCoreInit2,

	eAosErr_CharPtree_createNode,
	eAosErr_CliCmdIncorrect,
	eAosErr_DataSizeMismatch,
	eAosErr_TimerIdNotDefined,
	eAosErr_InvalidIndex,
	eAosErr_InvalidSkbIndex,
	eAosErr_NullPtr,
	eAosErr_UnrecognizedCmd,
	eAosErr_NoRoute,
	eAosErr_DeviceNotFound,
	eAosErr_MemoryError,
	eAosErr_MacNotFound,
	eAosErr_PromiscuityPositive,
	eAosErr_PromiscuityNegative,
	eAosErr_BridgeNotFound,

	eAosAlarm,

	eAosAlarm_aosAaaServer_destructor1,
	eAosAlarm_aosAaaServerEntry_destructor,

	eAosAlarm_aosAppProxy_constructor,
	eAosAlarm_aosAppProxy_destructor,

	eAosAlarm_aosAllocTcpSkb,
	eAosAlarm_aosMacCtlr_addCli1,
	eAosAlarm_aosMacCtlr_addCli2,
	eAosAlarm_aosMacCntl_addCli3,
	eAosAlarm_aosMacRule_constructor1,
	eAosAlarm_aosMacRule_constructor2,
	eAosAlarm_aosRule_constructor1,
	eAosAlarm_aosRule_constructor2,
	eAosAlarm_aosTcpSockBridge_constructor,
	eAosAlarm_aosTcpSockBridge_create,
	eAosAlarm_aosTcpSockBridge_forward,

	eAosAlarm_IntegrityCheckFail,

	eAosAlarmBranchNumErr,
	eAosAlarmBridgeExist,
	eAosAlarmBridgeNotFound,
	eAosAlarmBridgeNull,
	eAosAlarmBuffTooShort,
	eAosAlarmCopyFromUserErr,
	eAosAlarmCopyToUserErr,
	eAosAlarmDelObjInList,
	eAosAlarmDeviceInBridge,
	eAosAlarmDeviceInPeer,
	eAosAlarmDeviceNotFound,
	eAosAlarmDeviceNotInBridge,
	eAosAlarmDeviceNull,
	eAosAlarmDeviceOverlap,
	eAosAlarmDevNameTooLong,
	eAosAlarmForwarderNotDefined,
	eAosAlarmEmptyKey,
	eAosAlarmEntryAlreadyExist,
	eAosAlarmFatal,
	eAosAlarmIncorrectNumBridges,
	eAosAlarmIndexErr,
	eAosAlarmInvalidChar, 
	eAosAlarmInvalidDeviceId,
	eAosAlarmInvalidDeviceIndex,
	eAosAlarmInvalidNumDev,
	eAosAlarmInvalidNumRecv,
	eAosAlarmInvalidOptLen,
	eAosAlarmInvalidRuleTypeInDest,

	eAosAlarm_MacCtlr_proc,
	eAosAlarmMatchedIsNotLeaf,
	eAosAlarmMcDefined,
	eAosAlarmMemErr,

	eAosAlarm_FailedToAllocBuckets,
	eAosAlarm_FailedToAllocTable,
	eAosAlarm_HashTableBlockAllocErr,
	eAosAlarm_HashTableBlockOverflow,
	eAosAlarm_HashTableOverflow,
	eAosAlarm_NetInputPcp_proc,
	eAosAlarm_NotImplementedYet,

	eAosAlarmNoDev,
	eAosAlarmNoForwarder,
	eAosAlarmNullPointer,
	eAosAlarmNumDevErr,
	eAosAlarmNotImplementedYet,
	eAosAlarmOptLenErr,
	eAosAlarmOptLenTooShort,
	eAosAlarmParentIsNull,
	eAosAlarmPeerErr,
	eAosAlarmPeerNull,
	eAosAlarmPrevNull,
	eAosAlarmProgErr,
	eAosAlarmRootIsNull,
	eAosAlarmRuleDestructorNull,
	eAosAlarmRuleFuncNull,
	eAosAlarmSlabAlreadyDefined,
	eAosAlarmSlotNotEmpty,
	eAosAlarmStrTooLong,
	eAosAlarmTooManyBridges,
	eAosAlarmTooManySlabs,
	eAosAlarmUnrecognizedConfigId,

	eAosWarnBridgeExist,
	eAosWarnDeviceNotFound,
	eAosWarnRuleExist,
	eAosWarnRuleNotFound,

	eAosLastError
};

void aosInitRatedErr(void);
int aosAlarm4Imp(const char *file, 
					   int line, 
					   int id,
					   int int1, 
					   int int2, 
					   const char *str1, 
					   const char *str2);
int aosAlarmRatedImp(const char *file, 
					   int line, 
					   int id,
					   unsigned int freq,
					   int int1, 
					   int int2, 
					   const char *str1, 
					   const char *str2);
int aosAlarmImp(const char *file, 
					   int line, 
					   int id);
int aosAlarmIntImp(const char *file, 
					   int line, 
					   int id,
					   int value);
int aosAlarmStrImp(const char *file, 
					   int line, 
					   int id,
					   const char *value);
int aosWarnIntImp(const char *file, 
					   int line, 
					   int id,
					   int value);
int aosWarnStrImp(const char *file, 
					   int line, 
					   int id,
					   const char *value);
int aosAlarmInt2Imp(const char *file, 
					   int line, 
					   int id,
					   int value1, 
					   int value2);
int aosAlarmStr2Imp(const char *file, 
					   int line, 
					   int id,
					   const char *value1, 
					   const char *value2);
int aosWarnInt2Imp(const char *file, 
					   int line, 
					   int id,
					   int value1, 
					   int value2);
int aosWarnStr2Imp(const char *file, 
					   int line, 
					   int id,
					   const char *value1, 
					   const char *value2);

extern void aosAssertFailed(const char *file, int line);

#ifndef aosAlarm4
#define aosAlarm4(id, int1, int2, str1, str2) \
		aosAlarm4Imp(__FILE__, __LINE__, (id), (int1), (int2), (str1), (str2))
#endif

#ifndef aosAlarmInt
#define aosAlarmInt(id, value) \
		aosAlarmIntImp(__FILE__, __LINE__, (id), (value))
#endif

#ifndef aosAlarmInt2
#define aosAlarmInt2(id, value1, value2) \
		aosAlarmInt2Imp(__FILE__, __LINE__, (id), (value1), (value2))
#endif

#ifndef aosAlarmStr
#define aosAlarmStr(id, value) \
		aosAlarmStrImp(__FILE__, __LINE__, (id), (value))
#endif

#ifndef aosAlarmStr2
#define aosAlarmStr2(id, value1, value2) \
		aosAlarmStr2Imp(__FILE__, __LINE__, (id), (value1), (value2))
#endif

#ifndef aosAlarmR
#define aosAlarmR(id) \
		aosAlarmRatedImp(__FILE__, __LINE__, (id), (1), 0, 0, 0, 0)
#endif

#ifndef aosAlarmRated
#define aosAlarmRated(id, freq, int1, int2, str1, str2) \
		aosAlarmRatedImp(__FILE__, __LINE__, (id), (freq), (int1), (int2), (str1), (str2))
#endif


#ifndef aosAlarm
#define aosAlarm(id) \
		aosAlarmImp(__FILE__, __LINE__, (id))
#endif

#ifndef aosWarnInt
#define aosWarnInt(id, value) \
		aosWarnIntImp(__FILE__, __LINE__, (id), (value))
#endif

#ifndef aosWarnInt2
#define aosWarnInt2(id, value1, value2) \
		aosWarnInt2Imp(__FILE__, __LINE__, (id), (value1), (value2))
#endif

#ifndef aosWarnStr
#define aosWarnStr(id, value) \
		aosWarnStrImp(__FILE__, __LINE__, (id), (value))
#endif

#ifndef aosWarnStr2
#define aosWarnStr2(id, value) \
		aosWarnStr2Imp(__FILE__, __LINE__, (id), (value))
#endif

#ifndef aosKernelAssert
#define aosKernelAssert(x, y) if (!(x)) {aosAssertFailed(__FILE__, __LINE__); return (y);}
#endif

#include "aos/aosReturnCode.h"

#endif

