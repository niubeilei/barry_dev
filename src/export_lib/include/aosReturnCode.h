///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosReturnCode.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_aosReturnCode_h
#define aos_aos_aosReturnCode_h

//#include "aos/aosKernelAlarm.h"

enum 
{
	eAosRc_Success,

	eAosRc_DenyPageModuleStart 	= 100000,
	eAosRc_KtcpvsModuleStart 	= 101000,
	eAosRc_AppProcStart         = 102000,
	eAosRc_CertVerifyStart      = 103000,
	eAosRc_SSLStart         	= 104000,
	eAosRc_PlatformStart        = 105000,
	eAosRc_CertMgrStart         = 105000,
	eAosRc_AppProxyStart		= 106000,
	eAosRc_AosUtilStart			= 107000,
	eAosRc_DenyPageStart		= 108000,
	eAosRc_UtilCommStart		= 109000,
	eAosRc_SystemRouterStart	= 110000,
	eAosRc_FirewallStart		= 111000,
	eAosRc_WebwallStart			= 112000,
	eAosRc_ResMgrStart 			= 113000,
	eAosRc_DiskQuotaStart 		= 113600,
	eAosRc_RosOSStart			= 113900,
	eAosRc_DnsmasqStart 		= 114000,
	eAosRc_VlanStart 			= 115000,
	eAosRc_WanStart 			= 116000,
	eAosRc_RouterStart 			= 117000,
	eAosRc_DmzStart 			= 118000,
	eAosRc_MacStart 			= 119000,
	eAosRc_InetctrlStart 		= 120000,
	eAosRc_PptpStart 			= 121000,
        eAosRc_SystemTimeStart          = 122000,

	eAosRc_FirstReturnCode = 50000,
	eAosRc_AppProxyDefined,
	eAosRc_AppProxyNotDefined,
	eAosRc_AppProxyUsedByOthers,
	eAosRc_AppServerDefined,
	eAosRc_AppServerNotDefined,
	eAosRc_BridgeHcErr,
	eAosRc_BridgeNotFound,
	eAosRc_BufferTooShort,
	eAosRc_CliParmNumMismatch,
	eAosRc_CliTreeNull,
	eAosRc_CliCmdNotRecognized,
	eAosRc_CliCmdAlreadyDefined,
	eAosRc_CliCmdParmError,
	eAosRc_ContAosPath,
	eAosRc_Continue,
	eAosRc_CopyToUserErr,
	eAosRc_DecodeFailed,
	eAosRc_EntryAlreadyInList,
	eAosRc_EntryFound,
	eAosRc_EntryNotFound,
	eAosRc_FailedToCreateSocket,
	eAosRc_FailedToLock,
	eAosRc_IncorrectHexAscii,
	eAosRc_IncorrectObjId,
	eAosRc_IndexOutBound,
	eAosRc_InternalError,
	eAosRc_InvalidAppServerProtocol,
	eAosRc_InvalidAppProxyProtocol,
	eAosRc_InvalidArg,
	eAosRc_InvalidMac,
	eAosRc_InvalidMethod,
	eAosRc_InvalidParm,
	eAosRc_InvalidPriority,
	eAosRc_InvalidValue,
	eAosRc_InvalidSize,
	eAosRc_IncorrectSide,
	eAosRc_InvalidKernelApiCode,
	eAosRc_LengthTooLong,
	eAosRc_MemErr,
	eAosRc_NameTooLong,
	eAosRc_NoAosPath,
	eAosRc_NodeNotFound,
	eAosRc_NoRule,
	eAosRc_NotImplementedYet,
	eAosRc_NullPointer,
	eAosRc_ObjectDefined,
	eAosRc_ObjectNotFound,
	eAosRc_OptlenTooShort,
	eAosRc_ParmErr,
	eAosRc_PktConsumed,
	eAosRc_ProgErr,
	eAosRc_ProtocolNotSupported,
	eAosRc_ReturnDataTooLong,
	eAosRc_ServerCannotDelete,
	eAosRc_ServerDefined,
	eAosRc_ServerNotDefined,
	eAosRc_ServerNotFound,
	eAosRc_ServerGroupDefined,
	eAosRc_ServerGroupNotDefined,
	eAosRc_ServerGroupUsedByOthers,
	eAosRc_ServerNotInGroup,
	eAosRc_ServerUsedByOthers,
	eAosRc_TableIsEmpty,
	eAosRc_TooManyEntries,
	eAosRc_UnknownPcp,
// add by gb	
	eAosRc_ConfigureError,
	eAosRc_ServerAlreadyStopped,
	eAosRc_PermissionDenied,
	eAosRc_MallocError,
	eAosRc_SystemError,
	// By CHK 2007-01-31
	// Description: used in resource manager (CpuMgr MemMgr DiskMgr)
	// Begin
	eAosRc_LoadConfigFail,
	eAosRc_SaveConfigFail,
	eAosRc_FileNotFound,
	eAosRc_FileFailRead,
	eAosRc_NullPointerPassed,
	// End

	// used in interface between tcpproxy and appproc 
	eAosRc_ForwardToPeer,
	eAosRc_Drop,
	eAosRc_ReturnToPeer,
	eAosRc_DoNothing,
	eAosRc_LenTooShort,	
	eAosRc_LastReturnCode,

	//Add by lijing 12/12/2005
	eAosRc_NotUTF8
};

#endif



