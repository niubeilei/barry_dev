////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelEnum.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_pkt_engine_KernelEnum_h
#define Omn_pkt_engine_KernelEnum_h

#define AOS_POINTER_POISON ((void *)0x00a00a)

enum
{
    eAosMacAddressLen = 6,
    eAosMaxDevNameLen = 20,
    eAosMaxBridgeDev = 4,
	eAosKernelApiMaxStrLen = 500,
    eAosMaxDev    = 256,		// Must be the power of 2.
	eAosBridge_Left = 0x01,
	eAosBridge_Right = 0x02,
	eAOS_BRIDGE_SIDES = 0x03,

	eAosBridgeStatus_Inactive = 0,
	eAosBridgeStatus_Active = 1,

	eAosMinChar = 32,
	eAosMaxChar = 126,
	eAosGenericStrLen = 50,
	eAosStr1k   = 1010,
	eAosStr5k   = 5000,
	eAosStr10k  = 10101,

	eAosBridgeMethod_WRR 		= 1,
	eAosBridgeMethod_IPFLOW_WRR = 2,
};


enum AosKid
{
	eAosKid_Invalid,

	eAosKid_InitKernelApi,

	eAosKid_LoadConfig,
	eAosKid_SaveConfig,

	eAosKid_StartTimer,
	eAosKid_StopTimer,
	eAosKid_SendPkt,
	eAosKid_CreatePkt,
	eAosKid_SkbPut,
	eAosKid_SkbTailroom,
	eAosKid_SkbReserve,
	eAosKid_SkbPush,
	eAosKid_SkbPull,
	eAosKid_ShowSkb,
	eAosKid_GetMac,
	eAosKid_SetDevPromiscuity,
	eAosKid_ResetDevPromiscuity,
	eAosKid_IsDevPromiscuity,

	// Bridge CLI commands
	eAosKid_BridgeAdd, 
	eAosKid_BridgeRemove,
	eAosKid_BridgeShow,
	eAosKid_BridgeReset,
	eAosKid_BridgeClearAll,
	eAosKid_BridgeSaveConfig,
	eAosKid_BridgeStatus,
	eAosKid_BridgeList,
	eAosKid_BridgeDevAdd,
	eAosKid_BridgeDevRemove,
	eAosKid_BridgeHealthCheck,

	eAosKid_EnableNetworkInputPcp,
	eAosKid_DisableNetworkInputPcp,
	eAosKid_IsNetworkInputPcpOn,
	eAosKid_SetDebugFlag,
	eAosKid_ResetDebugFlag,
	eAosKid_ShowDebugFlags,
	eAosKid_EnableBridging,
	eAosKid_DisableBridging,
	eAosKid_IsBridgingOn,
};




enum OmnKernelTimerId
{
	eOmnKernelTimerId_Invalid,
	eOmnKernelTimerId_Test
};
#endif

