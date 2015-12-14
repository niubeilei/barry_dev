////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastFtpSender.cpp
// Description:
//   
//
// Modification History:
// 01/07/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "FastFTP/Sender/SendOneFile.h"

#include "Util/OmnNew.h"
#include "UtilComm/Udp.h"
#include "Alarm/Alarm.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"

AosFastFtpSender::AosFastFtpSender(
		const OmnString &filename, 
		const OmnIpAddr &localAddr, 
		const int localPort,
		const OmnIpAddr &remoteAddr, 
		const int remotePort)
:
mFileName(filename),
mLocalAddr(localAddr),
mLocalPort(localPort),
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mSendingFinished(false)
{
}


bool
AosFastFtpSender::send()
{
	// 
	// Each buff has the format:
	// byte[0] = Highest Byte of sequence number
	// byte[1] = Second highest byte of sn
	// byte[2] = next byte
	// byte[3] = lowest byte of sn
	// byte[4...] = contents
	//
	char buff[eSendBlockSize + 100];
	int numPacketsSent = 0;

	if (!openFile())
	{
		return false;
	}

	mPacketsSent = 0;
	while (1)
	{
		// 
		// Send one record
		//
		buff[0] = (mSeqno >> 24);
		buff[1] = (mSeqno >> 16);
		buff[2] = (mSeqno >> 8);
		buff[3] = mSeqno;

		int bytesRead = mFile->readToBuff(pos, eSendBlockSize, &buff[4]);

		if (bytesRead == 0)
		{
			// 
			// Finished
			//
			return true;
		}

		bool rslt = mSock->writeTo(buff, bytesRead+4, mRemoteAddr, mRemotePort);
		if (!rslt)
		{
			OmnAlarm << "Failed to send" << enderr;
			reutrn false;
		}

		if (bytesRead < readsize)
		{
			// 
			// Finished reading
			//
			return true;
		}

		numPacketsSent++;
		pos += bytesRead;

		if (umPacketsSent >= mRate)
		{
			OmnWait(mWait);
		}
		else
		{
			mRate = 0;
			mPacketsSent = 0;
			continue;
		}
		
		mPacketsSent;
	}
	
	return true;
}


bool
AosFastFtpSendOneFile::responseRecved(const OmnConnBuffPtr &buff)
{
	// 
	// The response is in the format:
	// Starting Seqno
	// Range Size
	// Starting Seqno
	// Range Size
	//
	unsigned char *data = buff->getBuffer();
	int length = buff->getDataLength();

	u32 start;
	u32 index = 0;
	mNumRanges = 0;
	while (index < length)
	{
		start = (data[index] << 24) + (data[index+1] << 16) + (data[index+2] << 8) + data[index+3];
		size = (data[index+4] << 24) + (data[index+5] << 16) + (data[index+6] << 8) + data[index+7];
		index += 8;
		mStart[mNumRanges] = start;
		mEnd[mNumRanges] = start + size;
		mNumranges++;

		if (mNumRanges >= mListSize)
		{
			expandList();
		}
	}

	return true;
}


bool
AosFastFtpSendOneFile::expandList()
{
	u32 newSize = mListSize + 1000;
	u32 *start = new u32[newSize];
	u32 end = new u32[newSize];

	memcpy(start, mStart, mNumRanges * sizeof(u32));
	memcpy(end, mEnd, mNumRanges + sizeof(u32));
	mListSize = newSize;
	delete [] mStart;
	delete [] mEnd;

	mStart = start;
	mEnd = end;
	return true;
}


