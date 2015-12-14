////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AmRcvBuff.cpp
// Description:
//	When receiving data from a TCP connection, we need to read the data
//  into a buffer and pass the buffer back to the caller. This involves
//  memory allocation issue. There are two ways to handle the issue. One
//  is the reading function allocates memory and passes back the memory
//  to its caller. The caller must delete the memory when not used. Or
//  the caller provides memory. However, the caller may not know how 
//  big the memory should be. 
//
//
//  When sending a data to a connection, the caller also needs to allocate
//  memory for the data to send. It should use this class, too.   
//
// Modification History:
// 2007-04-10: Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "AmUtil/AmRcvBuff.h"

#include "AmUtil/AmUtil.h"

#include "Alarm/Alarm.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util1/MemPool.h"
#include "UtilComm/Comm.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/Ptrs.h"

#define AM_MIN_MSG_SZ 9

AosAmRcvBuff::AosAmRcvBuff(const OmnTcpClientPtr &conn)
:
mConn(conn)
{
	mBuff = OmnNew OmnConnBuff;
}


AosAmRcvBuff::AosAmRcvBuff(const OmnTcpClientPtr &conn, const OmnConnBuffPtr &buff)
:
mConn(conn)
{
	mBuff = OmnNew OmnConnBuff(*buff);
}


AosAmRcvBuff::~AosAmRcvBuff()
{
}


OmnTcpClientPtr 
AosAmRcvBuff::getConn() const 
{
	return mConn;
}


bool
AosAmRcvBuff::append(const OmnConnBuff &rhs)
{
//	if(!mBuff)
//	{
//		mBuff = OmnNew OmnConnBuff;
//	}
	mBuff->append(rhs);
	return true;
}


bool
AosAmRcvBuff::append(const OmnConnBuffPtr &rhs)
{
//	if(!mBuff)
//	{
//		mBuff = OmnNew OmnConnBuff;
//	}
	mBuff->append(rhs);
	return true;
}


AosAmTransPtr
AosAmRcvBuff::getTrans()
{
	char * buff = mBuff->getData();
	u16 len = AosAmUtil::getU16(&buff[1]);
	
	if(len + 3 <= (u16)mBuff->getDataLength())
	{
		OmnConnBuffPtr connBuff = OmnNew OmnConnBuff(buff, len + 3);
		AosAmTransPtr trans = OmnNew AosAmTrans(mConn, connBuff);
		mBuff->removeHeader(len + 3);
		return trans;
	}

	return 0;
}


int	
AosAmRcvBuff::getHashKey()
{
	return (mConn->getSock() & 0x03ff);
}


int	
AosAmRcvBuff::hasSameObjId(const AosAmRcvBuffPtr &obj)
{
	return ((mConn->getSock() == obj->getConn()->getSock()) ? 1:0);
}
