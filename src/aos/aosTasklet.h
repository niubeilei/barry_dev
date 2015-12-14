////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosTasklet.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Tasklet_aosTasklet_h
#define Omn_Tasklet_aosTasklet_h

enum aosTaskletId
{
	eAosTasklet_sendPkt
};


struct aosTaskletData
{
	int						mTaskletId;
	int (*mFunc) (struct aosTaskletData *);
	struct aosTaskletData *	mPrev;
	struct aosTaskletData * mNext;
};


struct aosTaskletSendPktData
{
	// 
	// Common part
	//
	int				mTaskletId;
	int (*mFunc) (struct aosTaskletData *);
	struct aosTaskletData *	mPrev;
	struct aosTaskletData * mNext;

	unsigned int	mRecvAddr;
	unsigned short	mRecvPort;
	unsigned int	mSendAddr;
	unsigned short	mSendPort;
	char *			mData;
};


#endif
