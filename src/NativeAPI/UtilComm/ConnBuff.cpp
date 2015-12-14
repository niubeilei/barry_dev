////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ConnBuff.cpp
// Description:
//	When receiving data from a connection, we need to read the data
//  into a buffer and pass the buffer back to the caller. This involves
//  memory allocation issue. There are two ways to handle the issue. One
//  is the reading function allocates memory and passes back the memory
//  to its caller. The caller must delete the memory when not used. Or
//  the caller provides memory. However, the caller may not know how 
//  big the memory should be. 
//
//  To solve this problem, we will use this class. The reading function
//  generate an instance of this class when it reads data in. The instance
//  will allocate memory for it. The class self-handles the memory so that
//  no one else needs to worry about it. When no one uses this instance
//  anymore, the memory is returned automatically.
//
//  When sending a data to a connection, the caller also needs to allocate
//  memory for the data to send. It should use this class, too.   
//
// Modification History:
// 		In OmnConnBuff::OmnConnBuff(const OmnConnBuffPtr &rhs, bool)
// 		we didn't initialize mDataPtr and mDataLength. That causes serious
// 		problem. 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/ConnBuff.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util1/MemPool.h"
#include "UtilComm/Comm.h"
#include "UtilComm/TcpClient.h"


static int sgNumObjs = 0;

OmnConnBuff::OmnConnBuff()
:
mLocalPort(0),
mRemotePort(0),
mDynamic(0),
mDataLength(0),
mSessionId(0),
mSegId(0),
mFlags(0)
{
	sgNumObjs++;
	mDynamic = OmnNew char[eDefaultBuffSize];
	// if (sgNumObjs % 1000 == 0) cout << "Number of connbuff: " << sgNumObjs << endl;
	mBuffLength = eDefaultBuffSize;
	mDataLength = 0;
	//mDataPtr[0] = 0;
	mDataPtr = mDynamic;
	mDataPtr[0] = 0;
}


OmnConnBuff::OmnConnBuff(const uint length)
:
mLocalPort(0),
mRemotePort(0),
mDynamic(0),
mDataLength(0),
mSessionId(0),
mSegId(0),
mFlags(0)
{
	sgNumObjs++;
	// if (sgNumObjs % 1000 == 0) cout << "Number of connbuff: " << sgNumObjs << endl;

	if (length > eMaxBuffSize)
	{
		//
		// It is too big. Throw an exception.
		//
		OmnExcept e(OmnFileLine, OmnErrId::eBuffTooBig, 
			OmnString("ConnBuff too big: ") << length);
		throw e;
	}

	mDynamic = OmnNew char[length+1];
	mDataPtr = mDynamic;
	mBuffLength = length;

	mDataPtr[0] = 0;
}


OmnConnBuff::OmnConnBuff(const char *data, const uint length, const bool)
:
mLocalPort(0),
mRemotePort(0),
mDynamic(0),
mDataLength(length),
mSessionId(0),
mSegId(0),
mFlags(0)
{
	sgNumObjs++;
	// if (sgNumObjs % 1000 == 0) cout << "Number of connbuff: " << sgNumObjs << endl;

	if (length > eMaxBuffSize)
	{
		// It is too big. Throw an exception.
		OmnAlarm << "Data Too Long: " << length << ":" << eMaxBuffSize << enderr;
		OmnExcept e(OmnFileLine, OmnErrId::eBuffTooBig, 
			OmnString("ConnBuff too big: ") << length);
		throw e;
	}

	mDynamic = OmnNew char[length+1];
	mDataPtr = mDynamic;
	mBuffLength = length;

	memcpy(mDataPtr, data, length);
	mDataPtr[length] = 0;
}


OmnConnBuff::OmnConnBuff(const OmnConnBuff &rhs)
:
mLocalAddr(rhs.mLocalAddr),
mLocalPort(rhs.mLocalPort),
mRemoteAddr(rhs.mRemoteAddr),
mRemotePort(rhs.mRemotePort),
mDynamic(0),
mDataLength(rhs.mDataLength),
mComm(rhs.mComm),
mSessionId(rhs.mSessionId),
mSegId(rhs.mSegId),
mFlags(rhs.mFlags)
{
	sgNumObjs++;
	// if (sgNumObjs % 1000 == 0) cout << "Number of connbuff: " << sgNumObjs << endl;

	mDynamic = OmnNew char[mDataLength+1];
	mDataPtr = mDynamic;
	mBuffLength = mDataLength;
	memcpy(mDataPtr, rhs.mDataPtr, mDataLength);
}


OmnConnBuff &
OmnConnBuff::operator = (const OmnConnBuff &rhs)
{
	if (this == &rhs)
	{
		return *this;
	}

	mLocalAddr = rhs.mLocalAddr;
	mLocalPort = rhs.mLocalPort;
	mRemoteAddr = rhs.mRemoteAddr;
	mRemotePort = rhs.mRemotePort;

	mComm = rhs.mComm;
	mSessionId = rhs.mSessionId;
	mSegId = rhs.mSegId;
	mFlags = rhs.mFlags;

	mDataLength = rhs.mDataLength;
	if (mDynamic)
	{
		OmnDelete [] mDynamic;
		mDynamic = 0;
	}

	mDynamic = OmnNew char[mDataLength+1];
	mDataPtr = mDynamic;
	mBuffLength = mDataLength;

	memcpy(mDataPtr, rhs.mDataPtr, mDataLength);
	mDataPtr[mDataLength] = 0;
	return *this;
}


OmnConnBuff::~OmnConnBuff()
{
	sgNumObjs--;
	if (mDynamic)
	{
		OmnDelete [] mDynamic;
		mDynamic = 0;
	}
}


OmnString
OmnConnBuff::getString() const
{
	//
	// It converts the data buffer into a string.
	//
	return OmnString((char*)mDataPtr, mDataLength);
}


bool
OmnConnBuff::attachSenderAddr()
{
	// 
	// It attaches sender IP address (i.e., mRemoteAddr) and port
	// to the end of the buffer. It needs 6 bytes.
	//
	if (mDataLength + 6 >= mBuffLength && !determineMemory(mDataLength+6))
	{
		OmnAlarm << "ConnBuff out of bound: " << mDataLength
			<< ":" << mBuffLength << enderr;
		return false;
	}	

	return attachSenderAddr(mDataPtr, mDataLength, 
				mRemoteAddr, mRemotePort);
}


bool
OmnConnBuff::attachSenderAddr(char *data, 
							  uint &length,
							  const OmnIpAddr &remoteAddr,
							  const int port)
{
	unsigned int addr = remoteAddr.toInt();
	data[length]   = (unsigned char)((addr >> 24) & 0xff);
	data[length+1] = (unsigned char)((addr >> 16) & 0xff);
	data[length+2] = (unsigned char)((addr >> 8) & 0xff);
	data[length+3] = (unsigned char)(addr & 0xff);
	data[length+4] = (unsigned char)((port >> 8) & 0xff);
	data[length+5] = (unsigned char)(port & 0xff);

	length += 6;
	data[length] = 0;
	return true;
}


bool
OmnConnBuff::restoreSenderAddr()
{
	// 
	// It assumes the original sender IP address and port are attached
	// at the end of the buffer. 
	//
	if (mDataLength < 6)
	{
		OmnAlarm << "Data length too short to contain sender address: " 
			<< mDataLength << enderr;
		return false;
	}

	OmnTraceHB << "Before restoring: " << mRemoteAddr.toString()
		<< ":" << mRemotePort << endl;
	int addr = ((mDataPtr[mDataLength-6] << 24) & 0xff000000)+
			   ((mDataPtr[mDataLength-5] << 16) & 0xff0000)+ 
			   ((mDataPtr[mDataLength-4] << 8) & 0xff00) +
			   (mDataPtr[mDataLength-3] & 0xff);
	mRemoteAddr.set(addr);

	mRemotePort = ((mDataPtr[mDataLength-2] << 8) & 0xff00) + 
				  (mDataPtr[mDataLength-1] & 0xff);

	// 
	// Note that if mRemotePort == 0xffffffff, it should -1, not 
	// 65535.
	//			Chen Ding, 05/02/2003
	//
	if (mRemotePort == 0xffff)
	{
		mRemotePort = -1;
	}
	
	mDataLength -= 6;
	mDataPtr[mDataLength] = 0;
	OmnTraceHB << "After restoring: " << mRemoteAddr.toString()
		<< ":" << mRemotePort << endl;
	return true;
}


bool
OmnConnBuff::set(const char *data, const uint length)
{
	//
	// This function copies 'data' with 'length' to mDataPtr. 
	//
	OmnCheckAReturn(data, "Null pointer passed in", false);

	if (length >= mBuffLength && !determineMemory(length))
	{
		OmnAlarm << "Data too big: " << length << enderr;
		return false;
	}

	memcpy(mDataPtr, data, length);
	mDataLength = length;
	mDataPtr[mDataLength] = 0;
	return true;
}


void
OmnConnBuff::set(const char *data, 
				 const int length, 
				 const OmnConnBuffPtr &rhs)
{
	// 
	// This function copies 'data' (length number of bytes) to its mDataPtr. 
	//
	mLocalAddr = rhs->mLocalAddr;
	mLocalPort = rhs->mLocalPort;
	mRemoteAddr = rhs->mRemoteAddr;
	mRemotePort = rhs->mRemotePort;

	mComm = rhs->mComm;
	mSessionId = rhs->mSessionId;
	mSegId = rhs->mSegId;
	mFlags = rhs->mFlags;

	if (mDataLength > mBuffLength && !determineMemory(rhs->mDataLength))
	{
		OmnAlarm << OmnErrId::eConnBuffMemoryError
			<< "Failed to expand: " << mDataLength << enderr;
		return;
	}

	mDataLength = rhs->mDataLength;
	memcpy(mDataPtr, rhs->mDataPtr, mDataLength);
}


bool
OmnConnBuff::append(const OmnConnBuffPtr &rhs)
{
	uint newLength = mDataLength + rhs->mDataLength;
	if (newLength >= mBuffLength && !determineMemory(newLength))
	{
		OmnAlarm << OmnErrId::eConnBuffMemoryError
			<< "ConnBuff overflow: " 
			<< newLength << enderr;
		return false;
	}

	memcpy(&mDataPtr[mDataLength], rhs->mDataPtr, rhs->mDataLength);
	mDataLength += rhs->mDataLength;
	mDataPtr[mDataLength] = 0;

	return true;
}


bool
OmnConnBuff::append(const OmnConnBuff &rhs)
{
	uint newLength = mDataLength + rhs.mDataLength;
	if (newLength >= mBuffLength && !determineMemory(newLength))
	{
		OmnAlarm << OmnErrId::eConnBuffMemoryError
			<< "ConnBuff overflow: " 
			<< newLength << enderr;
		return false;
	}

	memcpy(&mDataPtr[mDataLength], rhs.mDataPtr, rhs.mDataLength);
	mDataLength += rhs.mDataLength;
	mDataPtr[mDataLength] = 0;

	return true;
}


void
OmnConnBuff::append(const char *data, const int length)
{
	OmnFuncEntry("append");
	OmnCheck(data, "");
	OmnCheck(length > 0, "")

	if (mDataLength + length >= mBuffLength && !determineMemory(mDataLength + length))
	{
		OmnAlarm << OmnErrId::eConnBuffMemoryError
			<< "ConnBuff overflow: " 
			<< mDataLength + length << enderr;
		OmnFuncExit(1);
		return;
	}
	
	memcpy(&mDataPtr[mDataLength], data, length);
	mDataLength += length;
	mDataPtr[mDataLength] = 0;

	OmnFuncExit(2);
	return;
}


OmnConnBuff &
OmnConnBuff::operator << (const int value)
{
	//
	// We always assume sizeof(int) == 4.
	//
	if (mDataLength + 4 >= mBuffLength && !determineMemory(mDataLength+4))
	{
		OmnAlarm << "ConnBuff overflow: " << mDataLength + 4 << enderr;
		return *this;
	}

	mDataPtr[mDataLength]   = value & 0xff;
	mDataPtr[mDataLength+1] = (value >> 8) & 0xff;
	mDataPtr[mDataLength+2] = (value >> 16) & 0xff;
	mDataPtr[mDataLength+3] = (value >> 24) & 0xff;
	mDataLength += 4;
	mDataPtr[mDataLength] = 0;

	return *this;
}

// 
// Jenny Gu, 08/17/2003, 2003-0223
//
void
OmnConnBuff::dumpHex(OmnString &str)
{
	for (uint i=0; i<mDataLength; i++)
	{
		str << " " << (int)mDataPtr[i];
	}
}


OmnRslt
OmnConnBuff::prepareToSend()
{
	// 
	// One should call this function before sending a connBuff over the 
	// streamer if it wants to attach the flags to the buffer. The flags
	// are used for the receiver to determine whether the buffer is a 
	// stream payload or a message. It will put segment ID, stream ID, 
	// and flags to the end of the buffer. 
	//
	if (mDataLength + 10 >= mBuffLength && !determineMemory(mDataLength + 10))
	{
		return OmnAlarm << "ConnBuff overflow: " 
			<< mDataLength + 10 << enderr;
	}

	//
	// Set the segment ID
	//
	mDataPtr[mDataLength++] = (mSegId) & 0xff;	
	mDataPtr[mDataLength++] = (mSegId >> 8) & 0xff;
	mDataPtr[mDataLength++] = (mSegId >> 16) & 0xff;
	mDataPtr[mDataLength++] = (mSegId >> 24) & 0xff;

	// 
	// Set the stream ID
	//
	mDataPtr[mDataLength++] = (mSessionId) & 0xff;	
	mDataPtr[mDataLength++] = (mSessionId >> 8) & 0xff;
	mDataPtr[mDataLength++] = (mSessionId >> 16) & 0xff;
	mDataPtr[mDataLength++] = (mSessionId >> 24) & 0xff;

	// 
	// Set the flags (two bytes)
	// 
	mDataPtr[mDataLength++] = (mFlags) & 0xff;	
	mDataPtr[mDataLength++] = (mFlags >> 8) & 0xff;

	mDataPtr[mDataLength] = 0;

	return true;
}


OmnRslt
OmnConnBuff::prepareToProcRecv()
{
	// 
	// One should call this function after received a conn buff from 
	// a streamer. It retrieves segment ID, stream ID, and flags from the
	// buffer. 
	//
	if (mDataLength < 10)
	{
		return OmnAlarm << "ConnBuff too short: " 
			<< mDataLength << enderr;
	}

	mFlags = ((unsigned char)mDataPtr[mDataLength-1] << 8) + (unsigned char)mDataPtr[mDataLength-2];
	mDataLength -= 2;

	mSessionId = ((unsigned char)mDataPtr[mDataLength-1] << 24) +
				 ((unsigned char)mDataPtr[mDataLength-2] << 16) + 
				 ((unsigned char)mDataPtr[mDataLength-3] << 8)  +
				  (unsigned char)mDataPtr[mDataLength-4];
	mDataLength -= 4;

	mSegId =     ((unsigned char)mDataPtr[mDataLength-1] << 24) +
				 ((unsigned char)mDataPtr[mDataLength-2] << 16) + 
				 ((unsigned char)mDataPtr[mDataLength-3] << 8)  +
				  (unsigned char)mDataPtr[mDataLength-4];
	mDataLength -= 4;
	mDataPtr[mDataLength] = 0;

	return true;
}


bool
OmnConnBuff::determineMemory(const uint newLength)
{
	// This function ensures that the buff length is at least
	// 'newLength' long. 
	OmnCheckAReturn(newLength > 0, "", false);
	OmnCheckAReturn(newLength < eMaxBuffSize, "", false);

	if (newLength <= mBuffLength)
	{
		return true;
	}

	// 
	// The new length is longer than the current one. Need to allocate 
	// the memory. 
	//
	try
	{
		char *buff = OmnNew char[newLength + eIncrSize + 5];
		aos_assert_r(buff, false);
		memcpy(buff, mDataPtr, mDataLength);
		OmnDelete [] mDynamic;
		mDynamic = buff;
		mDataPtr = buff;
		mBuffLength = newLength + eIncrSize;
		return true;
	}

	catch (const OmnExcept &e)
	{
		e.procException();
		return false;
	}

	OmnShouldNeverComeToThisPoint;
	return false;
}


bool
OmnConnBuff::setDataLength(const uint len)
{
	if (len > mBuffLength)
	{
		OmnAlarm << OmnErrId::eProgError
			<< "Buffer length incorrect: " << len << ":" << mBuffLength << enderr;
		return false;
	}

	mDataLength = len;

	//
	// Chen Ding, 05/23/2007
	//
	mDataPtr[len] = 0;
	return true;
}

bool	
OmnConnBuff::removeHeader(const int len)
{
	if(mDataLength < (uint)len || len < 0)	
	{
		return false;
	}
	
	int newLen = mDataLength - len;
	for(int i = 0;i < newLen; i++)
	{
		*(mDataPtr + i)  = *(mDataPtr + i + len);
	}
		
	mDataLength -= len;
	return true;
}


void
OmnConnBuff::setConn(const OmnTcpClientPtr &conn)
{
	mConn = conn;
}


OmnTcpClientPtr 
OmnConnBuff::getConn() const
{
	return mConn;
}


