////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SystemCli_ReturnCode_h
#define Aos_SystemCli_ReturnCode_h

#include "aos/aosReturnCode.h"


enum
{
	eAosRc_InvalidInterface = eAosRc_FirewallStart+1,
	eAosRc_InvalidChain,
	eAosRc_InvalidRate,
	eAosRc_InvalidInterval,
	eAosRc_LoadConfigError,
	eAosRc_SaveConfigError,
	eAosRc_ClearConfigError,
	eAosRc_InitError,
	eAosRc_MinInitError,
	eAosRc_MidInitError,
	eAosRc_MaxInitError,
	eAosRc_DelRuleNotExist,
	eAosRc_InvalidFwAction,
	eAosRc_InvalidProtocol,
	eAosRc_InvalidFileFormat,
	eAosRc_RuleIsExist,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	// eAosRc_MacStart+1,
	//eAosRc_MallocError,
	eAosRc_MacNotChanged = eAosRc_MacStart+1,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	eAosRc_InvalidHostname = eAosRc_DnsmasqStart+1,
	eAosRc_LabelAlreadyUsed,
	eAosRc_LabelNotFound,
	eAosRc_InvalidLeaseTime,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	eAosRc_InvalidFreq = eAosRc_WanStart+1,
	eAosRc_WanNotConnect,
	eAosRc_WanHasConnected,
	eAosRc_HCConditionNotSet,
	eAosRc_PPPoEConfigNotSet,
	eAosRc_StaticIpNotSet,
	eAosRc_SetAdslPasswdError,
	eAosRc_SetAdslUserError,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	eAosRc_VlanTagAlreadyUsed = eAosRc_VlanStart+1,
	eAosRc_VlanNoExist,
	eAosRc_InvalidVlanTag,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	 eAosRc_DmzAliasExist = eAosRc_DmzStart+1,
	 eAosRc_DmzAliasNotExist,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	 eAosRc_EntryExist = eAosRc_RouterStart+1,
	 eAosRc_EntryNotExist,
	 eAosRc_InvalidRouterEntry,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

enum
{
	eAosRc_FailedModifyFstab = eAosRc_DiskQuotaStart+1,
	eAosRc_StatusAlready,
	eAosRc_InvaldStatus,
	eAosRc_InvalidPartition,
	eAosRc_FailedOpenConfigFile,
	eAosRc_StatusNoOn,
	eAosRc_SizeError,
	eAosRc_UserNoExist,
	eAosRc_UserNoBeLimit,
	eAosRc_InvalidDir,
	eAosRc_DirNoBeLimit,
	eAosRc_DirNoExist,
	eAosRc_FailedOpenFile,
	eAosRc_FailedClearStatus,
};

enum
{
	 eAosRc_PptpUserExist = eAosRc_PptpStart+1,
	 eAosRc_PptpUserNotExist,
};
enum
{
	 eAosRc_SystemTimeSetFaild = eAosRc_SystemTimeStart+1,
	 eAosRc_SystemDateSetFaild,
	 eAosRc_SystemTimeFastSetFaild,
	 eAosRc_SystemTimeSlowSetFaild,
	 eAosRc_SystemTimezoneSetFaild,
	 eAosRc_SystemTimeInvalidZoneParm,
	 eAosRc_SystemTimeLinkFileErr,
	 eAosRc_SystemTimeCommandNoExist,
};


#endif

