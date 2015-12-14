////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelApiStruct.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_pkt_engine_KernelApiStruct_h
#define Omn_pkt_engine_KernelApiStruct_h


//#include <linux/types.h>
#include "aos/KernelEnum.h"
#include "aos/aosCoreComm.h"

#define AOS_BUFFER_HEAD_LENGTH 228

struct aosKapiHeader
{
	unsigned int	mLength;
	unsigned short 	mOprId;
};


struct aosKapi1
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	char 			mStr1[eAosGenericStrLen];
};


struct aosKapi2
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
};


struct aosKapi4
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	int				mInt3;
	int				mInt4;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
	char 			mStr3[eAosGenericStrLen];
	char			mStr4[eAosGenericStrLen];
};


struct aosKapi1k
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	int				mInt3;
	int				mInt4;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
	char 			mStr3[eAosGenericStrLen];
	char			mStr4[eAosGenericStrLen];
	unsigned int	mIndex;
	char			mStr[1010];
};


struct aosKapi2k
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	int				mInt3;
	int				mInt4;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
	char 			mStr3[eAosGenericStrLen];
	char			mStr4[eAosGenericStrLen];
	unsigned int	mIndex;
	char			mStr[2000];
};


struct aosKapi5k
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	int				mInt3;
	int				mInt4;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
	char 			mStr3[eAosGenericStrLen];
	char			mStr4[eAosGenericStrLen];
	unsigned int	mIndex;
	char			mStr[5000];
};


struct aosKapi10k
{
	unsigned int	mLength;
	unsigned short 	mOprId;
	int				mInt1;
	int				mInt2;
	int				mInt3;
	int				mInt4;
	char 			mStr1[eAosGenericStrLen];
	char			mStr2[eAosGenericStrLen];
	char 			mStr3[eAosGenericStrLen];
	char			mStr4[eAosGenericStrLen];
	unsigned int	mIndex;
	char			mStr[10101];
};


struct OmnKernelApi_sendPkt
{
	int				mIndex;
	unsigned long	mSendAddr;
	unsigned short	mSendPort;
	unsigned long	mRecvAddr;
	unsigned short	mRecvPort;
	unsigned int	mSize;
	unsigned char	mTos;
	char			mDeviceName[101];
	char			mData;
};
	
/*
struct OmnKernelApi_DevOpt
{
	char			mName[eAosMaxDevNameLen];
	int				mRslt;
};

struct OmnKernelApi_GetMac
{
	unsigned char	mMac[eAosMacAddressLen];
	unsigned int	mIpAddr;
	char			mDevName[eAosMaxDevNameLen];
};
		
struct OmnKernelApi_Timer
{
	enum OmnKernelTimerId	mTimerId;
	int						mValue;
	long					mData;
};


struct OmnKernelApi_pktTest
{
	int 			mIndex;
	unsigned int	mSize;
};


struct OmnKernelApiData
{
	unsigned short	mOprId;

	union
	{
		struct OmnKernelApi_Timer	mTimerData;
		struct OmnKernelApi_sendPkt	mSendPktData;
		struct OmnKernelApi_pktTest	mTestPktData;
		struct OmnKernelApi_GetMac	mGetMac;
		struct OmnKernelApi_DevOpt	mDev;
	} mData;
};

struct aosKernelApi_BridgeData
{
	unsigned short		mOprId;

	unsigned char 		mBridgeId;
	int					mNumDev1;
	char				mNames1[eAosMaxBridgeDev][eAosMaxDevNameLen];
	int					mWeight1[eAosMaxBridgeDev];
	int					mNumDev2;
	char				mNames2[eAosMaxBridgeDev][eAosMaxDevNameLen];
	int					mWeight2[eAosMaxBridgeDev];
	struct aosPktStat	mStat;
};

struct aosKernelApi_bridgeDev
{
	unsigned short	mOprId;

	unsigned char	mBridgeId;
	unsigned short	mFlags;
	char			mName[eAosMaxDevNameLen];
	int				mWeight;
};
*/


/*
struct aosKernelApi_BridgeDef
{
	int		mBridgeId;
	int		mNumDev1;
	char	mNames1[eAosMaxDev][eAosMaxDevNameLen];
	int		mWeight1[eAosMaxDev];
	int		mNumDev2;
	char	mNames2[eAosMaxDev][eAosMaxDevNameLen];
	int		mWeight2[eAosMaxDev];
};

struct aosKernelApi_ConfigCore
{
	unsigned short	mOprId;
	char			mConfigId;

	int				mBridgeStatus;
	int				mNumBridges;
	struct aosKernelApi_BridgeDef mBridgeDef[eAosMaxDev];
};


struct aosKernelApi_addBridgeDev
{
	unsigned short	mOprId;
	int				mBridgeId;
	int				mSide;
	char			mName[eAosMaxDevNameLen];
	int				mWeight;
};
*/


#endif

