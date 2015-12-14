////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelAPI.cpp
// Description:
//	Singleton Dependency:
//		AlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelAPI/KernelAPI.h"

#include "Alarm/Alarm.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosKernelAlarm.h"
#include "aos_core/aosSockCtl.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "UtilComm/CommGroup.h"
#include "UtilComm/TcpCommClt.h"
#include "Util1/Time.h"



#ifdef AOS_KERNEL_SIMULATE

#include <KernelSimu/net.h>
#include <KernelSimu/sock.h>
#include <KernelSimu/socket.h>

struct socket	sgTestSock;
#endif

static OmnMutex sgLock;
int	   OmnKernelApi::mSSock = 0;

#ifdef SG_ERR_STRING
static OmnString	sgErrName[eAosRc_LastReturnCode];
#endif

static AosKernelApiFunc sgKernelApiFunc = 0;


#include <sys/types.h>
#include <sys/socket.h>

int
OmnKernelApi::init()
{
#ifdef SG_ERR_STRING
	for (int i=0; i<eAosRc_LastReturnCode; i++)
	{
		sgErrName[i] = "Error Code: ";
		sgErrName[i] << i << ":" << eAosRc_FirstReturnCode;
	}
	setErrStr();
#endif

	mSSock = ::socket(AF_INET, SOCK_DGRAM, 0);
	int value = 0;

	int ret;
	if (sgKernelApiFunc)
	{
		ret = sgKernelApiFunc(SOL_SOCKET, AOS_SO_INIT_AOS, (char *)&value, 
			sizeof(int));
	}
	else
	{
		ret = aosKernelCli1(mSSock, (char*)&value, sizeof(int));
	}

	return ret;
}


/*
int
OmnKernelApi::startTimer(const OmnKernelTimerId timerId,
						 const int value, 
						 const long userData)
{
	// 
	// It starts the timer "timerId". The timer value is "value" in 
	// jiffies. 
	// 
    OmnTraceKernelApi << "Start timer: " 
		<< timerId << ":" << value << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_StartTimer;
	data.mData.mTimerData.mTimerId = timerId;
	data.mData.mTimerData.mValue = value;
	data.mData.mTimerData.mData = userData;

	return sendToKernel(data);
}


int
OmnKernelApi::sendPkt(const int index, 
					  const unsigned long  sendAddr,
					  const unsigned short sendPort,
					  const unsigned long  recvAddr,
					  const unsigned short recvPort,
					  const unsigned int size, 
					  const int tos, 
					  const char *deviceName)
{
    OmnTrace << "Send Packet: " 
		<< "Index: " << index << ", "
		<< "Sender: " << sendAddr << ":" << sendPort << ", "
		<< "Recver: " << recvAddr << ":" << recvPort << ", "
		<< "Size: " << size << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SendPkt;
	data.mData.mSendPktData.mIndex = index;
	data.mData.mSendPktData.mSendAddr = sendAddr;
	data.mData.mSendPktData.mSendPort = sendPort;
	data.mData.mSendPktData.mRecvAddr = recvAddr;
	data.mData.mSendPktData.mRecvPort = recvPort;
	data.mData.mSendPktData.mSize = size;
	data.mData.mSendPktData.mTos = (unsigned char)tos;
	strcpy(data.mData.mSendPktData.mDeviceName, deviceName);
	
	return sendToKernel(data);
}


int
OmnKernelApi::skb_put(const int index, const unsigned int size)
{
    OmnTraceKernelApi << "skb_put: " << index << " Size: " << size << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SkbPut;
	data.mData.mTestPktData.mIndex = index;
	data.mData.mTestPktData.mSize = size;
	return sendToKernel(data);
}


int
OmnKernelApi::skb_reserve(const int index, const unsigned int size)
{
    OmnTraceKernelApi << "skb_reserve: " << index << "Size: " << size << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SkbReserve;
	data.mData.mTestPktData.mIndex = index;
	data.mData.mTestPktData.mSize = size;
	return sendToKernel(data);
}


int
OmnKernelApi::skb_tailroom(const int index)
{
    OmnTraceKernelApi << "skb_tailroom: " << index << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SkbTailroom;
	data.mData.mTestPktData.mIndex = index;
	int ret = sendToKernel(data);

	if (ret == 0)
	{
		OmnTrace << "Tailroom: " << data.mData.mTestPktData.mSize << endl;
	}
	return ret;
}


int
OmnKernelApi::skb_push(const int index, const unsigned int size)
{
    OmnTraceKernelApi << "skb_put: " << index << " Size: " << size << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SkbPush;
	data.mData.mTestPktData.mIndex = index;
	data.mData.mTestPktData.mSize = size;
	return sendToKernel(data);
}


int
OmnKernelApi::skb_pull(const int index, const unsigned int size)
{
    OmnTraceKernelApi << "skb_pull: " << index << " Size: " << size << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_SkbPull;
	data.mData.mTestPktData.mIndex = index;
	data.mData.mTestPktData.mSize = size;
	return sendToKernel(data);
}


int
OmnKernelApi::showSkb(const int index)
{
    OmnTraceKernelApi << "show skb: " << index << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_ShowSkb;
	data.mData.mTestPktData.mIndex = index;
	return sendToKernel(data);
}


int
OmnKernelApi::sendToKernel(const OmnKernelApiData &data)
{
	sgLock.lock();
    int ret = aosKernelCli(mSSock, (char *)&data, sizeof(data));
	sgLock.unlock();

    if (ret != eAos_Success)
    {
        //
        // Failed to add rules.
        //
        OmnAlarm << "Failed to run kernel API: "
            << getErr(ret)
            << enderr;
    }

    return ret;
}
*/


int
OmnKernelApi::sendToKernel(char *data, const int size)
{
	int ret;

	if (sgKernelApiFunc)
	{
		ret = sgKernelApiFunc(SOL_SOCKET, AOS_SO_KAPI, data, size);
	}
	else
	{
		sgLock.lock();
    	ret = aosKernelCli(mSSock, data, size);
		sgLock.unlock();
	}

    return ret;
}


/*
int
OmnKernelApi::sendToKernel(unsigned short apiId, const int value)
{
	aosKernelApi_int data;
	data.mOprId = apiId;
	data.mValue = value;
	
	sgLock.lock();
    int ret = aosKernelCli(mSSock, (char *)&data, sizeof(aosKernelApi_int));
	sgLock.unlock();

    if (ret != eAos_Success)
    {
        //
        // Failed to add rules.
        //
        OmnAlarm << "Failed to run kernel API: "
            << getErr(ret)
            << enderr;
    }

    return ret;
}
*/


/*
int
OmnKernelApi::sendToKernelRef(unsigned short apiId, int &value)
{
	aosKernelApi_int data;
	data.mOprId = apiId;
	data.mValue = value;
	
	sgLock.lock();
    int ret = aosKernelCli(mSSock, (char *)&data, sizeof(aosKernelApi_int));
	sgLock.unlock();

    if (ret != eAos_Success)
    {
        //
        // Failed to add rules.
        //
        OmnAlarm << "Failed to run kernel API: "
            << getErr(ret)
            << enderr;
    }

	value = data.mValue;
    return ret;
}
*/



/*
int
OmnKernelApi::sendToKernel(unsigned short apiId, const char *value)
{
	if (strlen(value) >= eAosKernelApiMaxStrLen)
	{
		OmnAlarm << "String too long: " << strlen(value)
			<< ". Maximum: " << eAosKernelApiMaxStrLen << enderr;
		return eAosAlarmStrTooLong;
	}

	aosKernelApi_int data;
	data.mOprId = apiId;
	strcpy(data.mValue, value);
	
	sgLock.lock();
OmnTrace << "To call kernel!" << endl;
int ret = 0;
    // int ret = setsockopt(mSSock, SOL_SOCKET, AOS_SO_KAPI, 
	// 	(const char *)&data, sizeof(aosKernelApi_str));
	sgLock.unlock();

    if (ret != eAos_Success)
    {
        //
        // Failed to add rules.
        //
        OmnAlarm << "Failed to run kernel API: "
            << getErr(ret)
            << enderr;
    }

    return ret;
}
*/


#ifdef SG_ERR_STRING
void
OmnKernelApi::setErrStr()
{
    sgErrName[eAos_Success] = "Success";

    sgErrName[eAosErrR_firstRatedErr] = "First Rated Err";
    sgErrName[eAosErrR_invalidRatedErrId] = "InvalidRatedErrId";

    sgErrName[eAosAlarmR_DevNull] = "DevNull";
    sgErrName[eAosAlarmR_BridgeNull] = "BridgeNull";
    sgErrName[eAosAlarmR_BridgedSkbNotCorrect] = "BridgeSkbNotCorrect";
    sgErrName[eAosAlarmR_EthNull] = "EthNull";
    sgErrName[eAosAlarmR_ForwardedSkbNotCorrect] = "ForwardedSkbNotCorrect";
    sgErrName[eAosAlarmR_InvalidMethod] = "InvalidMethod";
    sgErrName[eAosAlarmR_MemErr] = "MemErr";
    sgErrName[eAosAlarmR_RuleMismatch] = "RuleMismatch";
    sgErrName[eAosAlarmR_NoDevice] = "NoDevice";
    sgErrName[eAosAlarmR_NoReceiver] = "NoReceiver";
    sgErrName[eAosAlarmR_UnknownMethod] = "UnknownMethod";

    sgErrName[eAosErrR_lastRatedErr] = "LastRatedErr";

    sgErrName[eAosErr_BridgeNotFound] = "Bridge not found";
    sgErrName[eAosErr_DataSizeMismatch] = "DataSizeMismatch";
    sgErrName[eAosErr_DeviceNotFound] = "DeviceNotFound";
    sgErrName[eAosErr_InvalidIndex] = "InvalidIndex";
    sgErrName[eAosErr_InvalidSkbIndex] = "InvalidSkbIndex";
    sgErrName[eAosErr_MacNotFound] = "MacNotFounc";
    sgErrName[eAosErr_MemoryError] = "MemoryError";
    sgErrName[eAosErr_NoRoute] = "NoRoute";
    sgErrName[eAosErr_NullPtr] = "NullPointer";
    sgErrName[eAosErr_PromiscuityPositive] = "PromiscuityPositive";
    sgErrName[eAosErr_PromiscuityNegative] = "PromiscuityNegative";
    sgErrName[eAosErr_TimerIdNotDefined] = "TimerIdNotDefined";
    sgErrName[eAosErr_UnrecognizedCmd] = "UnrecognizedCmd";


    sgErrName[eAosAlarmBranchNumErr] = "BranchNumErr";
    sgErrName[eAosAlarmBridgeExist] = "BridgeExist";
    sgErrName[eAosAlarmBridgeNull] = "BridgeNull";
    sgErrName[eAosAlarmBridgeExist] = "BridgeExist";
    sgErrName[eAosAlarmBridgeNotFound] = "Bridge Not Found";
    sgErrName[eAosAlarmBuffTooShort] = "BuffTooShort";
    sgErrName[eAosAlarmCopyFromUserErr] = "CopyFromUserErr";
    sgErrName[eAosAlarmCopyToUserErr] = "CopyToUserErr";
    sgErrName[eAosAlarmDelObjInList] = "DelObjInList";
    sgErrName[eAosAlarmDevNameTooLong] = "DevNameTooLong";
    sgErrName[eAosAlarmDeviceInBridge] = "Device already in bridge";
    sgErrName[eAosAlarmDeviceInPeer] = "DeviceInPeer";
    sgErrName[eAosAlarmDeviceNotFound] = "DeviceNotFound";
    sgErrName[eAosAlarmDeviceNotInBridge] = "DeviceNotInBridge";
    sgErrName[eAosAlarmDeviceNull] = "DeviceNull";
    sgErrName[eAosAlarmDeviceOverlap] = "DeviceOverlap";
    sgErrName[eAosAlarmEntryAlreadyExist] = "EntryAlreadyExist";
    sgErrName[eAosAlarmForwarderNotDefined] = "ForwarderNotDefined";
    sgErrName[eAosAlarmIndexErr] = "IndexErr";
    sgErrName[eAosAlarmInvalidDeviceId] = "InvalidDeviceId";
    sgErrName[eAosAlarmIncorrectNumBridges] = "IncorrectNumBridges";
    sgErrName[eAosAlarmInvalidChar] = "InvalidChar";
    sgErrName[eAosAlarmInvalidDeviceIndex] = "InvalidDeviceIndex";
    sgErrName[eAosAlarmInvalidNumDev] = "InvalidNumDev";
    sgErrName[eAosAlarmInvalidNumRecv] = "InvalidNumRecv";
    sgErrName[eAosAlarmInvalidOptLen] = "InvalidOptLen";
    sgErrName[eAosAlarmInvalidRuleTypeInDest] = "InvalidRuleTypeInDest";
    sgErrName[eAosAlarmMemErr] = "MemErr";
    sgErrName[eAosAlarmMatchedIsNotLeaf] = "MatchedIsNotLeaf";
    sgErrName[eAosAlarmMcDefined] = "McDefined";
    sgErrName[eAosAlarmNoDev] = "NoDev";
    sgErrName[eAosAlarmNumDevErr] = "NumDevErr";
    sgErrName[eAosAlarmNoForwarder] = "NoForwarder";
    sgErrName[eAosAlarmNotImplementedYet] = "NotImplementedYet";
    sgErrName[eAosAlarmOptLenErr] = "OptLenErr";
    sgErrName[eAosAlarmOptLenTooShort] = "OptLenTooShort";
    sgErrName[eAosAlarmPeerErr] = "PeerErr";
    sgErrName[eAosAlarmPeerNull] = "PeerNull";
    sgErrName[eAosAlarmPrevNull] = "PrevNull";
    sgErrName[eAosAlarmProgErr] = "ProgErr";
    sgErrName[eAosAlarmRuleFuncNull] = "RuleFuncNull";
    sgErrName[eAosAlarmRuleDestructorNull] = "RuleDestructorNull";
    sgErrName[eAosAlarmStrTooLong] = "StrTooLong";
    sgErrName[eAosAlarmSlotNotEmpty] = "SlotNotEmpty";
    sgErrName[eAosAlarmTooManyBridges] = "TooManyBridges";
    sgErrName[eAosAlarmUnrecognizedConfigId] = "UnrecognizedConfigId";

    sgErrName[eAosWarnBridgeExist] = "Bridge Exist";
    sgErrName[eAosWarnDeviceNotFound] = "Device Not Found";
    sgErrName[eAosWarnRuleExist] = "RuleExist";
    sgErrName[eAosWarnRuleNotFound] = "RuleNotFound";

    sgErrName[eAosRc_AppProxyDefined] = "Application Proxy Defined";
    sgErrName[eAosRc_AppProxyNotDefined] = "Application Proxy Not Defined";
    sgErrName[eAosRc_CliCmdNotRecognized] = "Command not recognized";
}


OmnString
OmnKernelApi::getErr(const int code)
{
	if (code >= 0 && code < eAosRc_LastReturnCode)
	{
		return sgErrName[code];
	}

	return OmnString("Outside Scope: ") << code << ". First RC: " << eAosRc_FirstReturnCode;
}
#endif

// 
// It retrieves the ARP table for the MAC address of 'ipaddr'. If found, 'haddr'
// contains the MAC address and 0 is returned. Otherwise, 1 is returned. 
// If needed, the function will send an ARP request. 
//
/*
int aosGetMac(unsigned char *haddr,
               const char *ipaddr,
               const char *name)
{
    OmnTrace << "To retrieve MAC: " << ipaddr << ", dev = " << name << endl;

	OmnKernelApiData data;
	data.mOprId = eAosKid_GetMac;
	memset(data.mData.mGetMac.mMac, 0, eAosMacAddressLen);
	strcpy(data.mData.mGetMac.mDevName, name);
	data.mData.mGetMac.mIpAddr = OmnIpAddr(ipaddr).toInt();

	int ret = OmnKernelApi::sendToKernel(data);
	if (ret == 0)
	{
		for (int i=0; i<eAosMacAddressLen; i++)
		{
			printf("%x:", (unsigned char)data.mData.mGetMac.mMac[i]);
		}
		cout << endl;
	}

	return ret;
}
		

int aosSetDevPromiscuity(const char *name)
{
	OmnKernelApiData data;
	data.mOprId = eAosKid_SetDevPromiscuity;
	strcpy(data.mData.mDev.mName, name);

	return OmnKernelApi::sendToKernel(data);
}


int aosResetDevPromiscuity(const char *name)
{
	OmnKernelApiData data;
	data.mOprId = eAosKid_ResetDevPromiscuity;
	strcpy(data.mData.mDev.mName, name);

	return OmnKernelApi::sendToKernel(data);
}


int aosIsDevPromiscuity(const char *name, int *rslt)
{
	OmnKernelApiData data;
	data.mOprId = eAosKid_IsDevPromiscuity;
	strcpy(data.mData.mDev.mName, name);

	int ret = OmnKernelApi::sendToKernel(data);
	if (ret == 0)
	{
		*rslt = data.mData.mDev.mRslt;
		if (data.mData.mDev.mRslt)
		{
			cout << "Device: " << name << " is in promiscuous mode" << endl;
		}
		else
		{
			cout << "Device: " << name << " is not in promiscuous mode" << endl;
		}
	}

	return ret;
}


int aosAddBridge(unsigned char bridgeId,
                 int numDev1, 
				 char names1[eAosMaxBridgeDev][eAosMaxDevNameLen], 
				 int weight1[],
                 int numDev2, 
				 char names2[eAosMaxBridgeDev][eAosMaxDevNameLen], 
				 int weight2[])
{
	OmnTrace << "To add a bridge: " << bridgeId 
		<< " numDev1: " << numDev1 
		<< ", numDev2: " << numDev2 << endl;

	aosKernelApi_BridgeData data;
	int i;

	data.mOprId = eAosKid_BridgeAdd;
	data.mBridgeId = bridgeId;
	data.mNumDev1 = numDev1;
	data.mNumDev2 = numDev2;

	for (i=0; i<numDev1; i++)
	{
		strcpy(data.mNames1[i], names1[i]);
		data.mWeight1[i] = weight1[i];
	}

	for (i=0; i<numDev2; i++)
	{
		strcpy(data.mNames2[i], names2[i]);
		data.mWeight2[i] = weight2[i];
	}

OmnTrace << "To call kapi: " << sizeof(struct aosKernelApi_BridgeData) << endl;
	return OmnKernelApi::sendToKernel(
		(char *)&data, sizeof(struct aosKernelApi_BridgeData));
}


int aosRemoveBridge(int bridgeId)
{
	return OmnKernelApi::sendToKernel(eAosKid_BridgeRemove, bridgeId);
}
*/


/*
int aosIsBridgeDefined(unsigned char bridgeId)
{
	return OmnKernelApi::sendToKernel(eAosKid_IsBridgeDefined, bridgeId);
}
*/


/*
int aosAddBridgeDev(unsigned char bridgeId,
                    unsigned short flags,
                 	char *name, 
                 	int weight)
{
	struct aosKernelApi_bridgeDev data;
	data.mOprId = eAosKid_BridgeDevAdd;
	data.mBridgeId = bridgeId;
	data.mFlags = flags;
	strcpy(data.mName, name);
	data.mWeight = weight;

	return OmnKernelApi::sendToKernel(
		(char *)&data, sizeof(struct aosKernelApi_bridgeDev));
}


int aosRemoveBridgeDev(unsigned char bridgeId, 
					unsigned short flags,
					char *name)
{
	struct aosKernelApi_bridgeDev data;
	data.mOprId = eAosKid_BridgeDevRemove;
	data.mBridgeId = bridgeId;
	data.mFlags = flags;
	strcpy(data.mName, name);

	return OmnKernelApi::sendToKernel((char *)&data, 
		sizeof(struct aosKernelApi_bridgeDev));
}
*/


/*
int aosGetBridge(unsigned char bridgeId, aosKernelApi_BridgeData &data)
{
	data.mOprId = eAosKid_BridgeGet;
	data.mBridgeId = bridgeId;
	return OmnKernelApi::sendToKernel((char *)&data, 
		sizeof(struct aosKernelApi_BridgeData));
}
*/
	

/*
int aosEnableNetworkInputPcp()
{
	return OmnKernelApi::sendToKernel(eAosKid_EnableNetworkInputPcp, 0);
}


int aosDisableNetworkInputPcp()
{
	return OmnKernelApi::sendToKernel(eAosKid_DisableNetworkInputPcp, 0);
}


int aosIsNetworkInputPcpOn(int &status)
{
	return OmnKernelApi::sendToKernelRef(eAosKid_IsNetworkInputPcpOn, status);
}


int aosSetDebugFlag(const int index)
{
	return OmnKernelApi::sendToKernel(eAosKid_SetDebugFlag, index);
}

	
int aosResetDebugFlag(const int index)
{
	return OmnKernelApi::sendToKernel(eAosKid_ResetDebugFlag, index);
}


int aosEnableBridging()
{
	return OmnKernelApi::sendToKernel(eAosKid_EnableBridging, 0);
}


int aosDisableBridging()
{
	return OmnKernelApi::sendToKernel(eAosKid_DisableBridging, 0);
}


int aosIsBridgingOn(int &status)
{
	return OmnKernelApi::sendToKernelRef(eAosKid_IsBridgingOn, status);
}
*/

void		
OmnKernelApi::setKernelApiFunc(AosKernelApiFunc f)
{
	sgKernelApiFunc = f;
}

	
