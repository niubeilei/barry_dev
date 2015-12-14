////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ForeignTransId.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TransMgr_ForeignTransId_h
#define Omn_TransMgr_ForeignTransId_h

#include "aosUtil/Types.h"
#include "TransMgr/ForeignTransId.h"
#include "Util/IpAddr.h"


class OmnForeignTransId
{
public:
	OmnIpAddr	mAddr;
	int			mPort;
	int64_t		mTransId;

	OmnForeignTransId()
		:
	mPort(0),
	mTransId(0)
	{
	}

	OmnForeignTransId(const OmnIpAddr &addr, const int port, const int64_t &transId)
		:
	mAddr(addr),
	mPort(port),
	mTransId(transId)
	{
	}

	int getHashKey() const
	{
		return (mAddr.getHashKey()+ 
			   (mPort & 0xffff) + 
				aos_ll_hashkey(mTransId));
	}

	bool operator == (const OmnForeignTransId &rhs) const
	{
		return mAddr == rhs.mAddr && 
			   mPort == rhs.mPort &&
			   mTransId == rhs.mTransId;
	}
};


#endif
