////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 04/23/2010	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/DataReader.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SearchEngine/DocServer1.h"
#include "SearchEngine/DocServerCb.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"

#if 0

AosSeDataReader::AosSeDataReader()
:
mLock(OmnNew OmnMutex())
{
}


AosSeDataReader::~AosSeDataReader()
{
}


AosXmlTagPtr
AosSeDataReader::startRead(const u64 docid)
{
	// It starts reading the 'num'-th document. Normally
	// 'num' should be 0.
	mLock->lock();
	mCrtDocid = docid;
	AosXmlDoc header;
	AosXmlTagPtr doc = header.loadXmlFromFile(docid);
	mLock->unlock();
	return doc;

	/*
	mSeqno = 0;
	mCrtFile = getFilePriv(mSeqno);
	aos_assert_rl(mCrtFile, mLock, false);

	// Skip all the documents.
	mOffset = 0;
	int len = 0;
	int idx = 0;
	while (len != -1)
	{
		len = 0;
		while (idx < num)
		{
			len = mCrtFile->readBinaryInt(mOffset, -1);
			if (len == -1) break;
			aos_assert_rl(len > 0, mLock, false);
			mOffset += 4 + (u32)len;
			idx++;
		}

		if (len != -1) break;

		// Need to get the next file
		mCrtFile = getFilePriv(++mSeqno);
		if (!mCrtFile)
		{
			// The requested doc does not exist
			mLock->unlock();
			return false;
		}
		mOffset = 0;
	}

	// Now 'mOffset' points to the doc to be read.
	len = mCrtFile->readBinaryInt(mOffset, -1);
	if (len == -1)
	{
		// It may be in the next file
		mCrtFile = getFilePriv(++mSeqno);
		if (!mCrtFile) 
		{
			mLock->unlock();
			return false;
		}

		mOffset = 0;
		len = mCrtFile->readBinary(mOffset, -1);
		if (len == -1)
		{
			mLock->unlock();
			return false;
		}
	}

	mOffset += 4;
	aos_assert_rl(doc.setMemory(len), mLock, false);
	char *data = doc.data();
	int bytesread = mCrtFile->readToBuff(mOffset, (u32)len, data);
	mLock->unlock();
	aos_assert_r(len == bytesread, false);
	mOffset += len;
	return true;
	*/
}


AosXmlTagPtr
AosSeDataReader::readNext()
{
	// It starts reading the 'num'-th document. Normally
	// 'num' should be 0.
	mLock->lock();
	AosXmlDoc header;
	AosXmlTagPtr doc = header.loadXmlFromFile(++mCrtDocid);
	mLock->unlock();
	return doc;
}

#endif
