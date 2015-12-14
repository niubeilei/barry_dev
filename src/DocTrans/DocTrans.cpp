////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 07/21/2011   Created by Ketty 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/29
#if 0

#include "DocTrans/DocTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "DocTrans/DocTransProc.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"


AosDocTrans::AosDocTrans(
		const OmnString &fname,
		const AosDocTransProcPtr &proc)
:
mProc(proc),
mFilename(fname),
mLock(OmnNew OmnMutex()),
mFileSize(0),
mBlockNum(0)
{
	aos_assert(proc);
	
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert(mFile->isGood());
	}
}


AosDocTrans::~AosDocTrans()
{
}


bool
AosDocTrans::isGood()
{
	if(!mFile || !mProc) return false;
	return mFile->isGood();
}


bool
AosDocTrans::initTmpFile(
		const u64 &entryId,
		const vector<u64> &trans)
{
	// The temporary data is in the format of:
	// 	Meta Data Portion:
	//		Byte 0-3	Poison1		to indicate whethe the file is good
	// 		Byte 4		File Status: 
	// 		Byte 5-8	File Size: 
	// 		Byte 9-16	docid (or iilid)
	// 		Byte 17-20	Number of Transaction IDs
	// 		Byte 21-..	Transactions (u64)
	// 		...
	// 		Bytes		Header size
	// 		Bytes		Header contents
	// 		Bytes		Body size
	// 		Bytes		Body contents
	// 		Byte xxx	Poison 2
	// file Status vaule:
	// 		a: this means the file is new
	//		b: set poison successful
	//		c. modify successful

	// init tmp file Status 	
	char c = 'a'; 
	bool rslt = mFile->setStr(eTmpStatus, &c, 1, true);
	aos_assert_r(rslt, false);

	// Create meta data portion (length is dependent on the number of transactions).
	int numTrans = trans.size();
	mFileSize = eTmpTrans + sizeof(u64) * numTrans;
	mBlockNum = 0;

	AosBuff buff(mFileSize, 0 AosMemoryCheckerArgs);
	memset(buff.data(), 0, mFileSize);
	
	//set poison1;
	buff.setU32(eTmpPoisonNum);
	buff.setChar('b');		//file Status
	buff.setU32(mFileSize);	//file size
	buff.setU64(entryId);
	buff.setInt(mBlockNum);
	buff.setU32(numTrans);

	buff.setCrtIdx(eTmpTrans);
	for (int i=0; i<numTrans; i++)
	{
		buff.setU64(trans[i]);
	}
	rslt = mFile->put(0, buff.data(), mFileSize, false);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDocTrans::addBlock(
		AosBuff &headerBuff, 
		AosBuffPtr &bodyBuff)
{
	mBlockNum++;
	bool rslt = mFile->setInt(eTmpBlockNum, mBlockNum, false);
	aos_assert_r(rslt, false);

	u32 headerSize = headerBuff.dataLen();
	u32 headerStart = mFileSize + sizeof(u32);	//sizeof(u32) is the header Size
	u32 bodySize = bodyBuff->dataLen();
	u32 bodyStart = headerStart + headerSize + sizeof(u32);	//sizeof(u32) is the Body Size
	mFileSize = bodyStart + bodySize;

//OmnScreen << "head:" << headerStart << "," << headerSize
//	<< ", body:" << bodyStart << "," << bodySize << endl;

	//copy header to temp file
	rslt = mFile->setInt(headerStart - sizeof(u32), headerSize, false); //set header size
	aos_assert_r(rslt, false);
	rslt = mFile->put(headerStart, headerBuff.data(), headerSize, false);
	aos_assert_r(rslt, false);
	
	//copy doc to temp file
	rslt = mFile->setInt(bodyStart - sizeof(u32), bodySize, false); //set body size
	aos_assert_r(rslt, false);
	rslt = mFile->put(bodyStart, bodyBuff->data(), bodySize, false);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDocTrans::initTmpFileFinish()
{
	mFileSize += sizeof(u32);
	bool rslt = mFile->setU32(eTmpFileSize, mFileSize, false);
	aos_assert_r(rslt, false);

	rslt = mFile->setU32(mFileSize - sizeof(u32), eTmpPoisonNum, true);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDocTrans::finishTrans(
		vector<u64> &trans,
		const AosRundataPtr &rdata)
{
	// 4. modify tmp file Status 	
	char c = 'c'; 
	mFile->setStr(eTmpStatus, &c, 1, true);
	
	// 5.modify trans status
	mProc->finishTrans(trans, rdata);
	
	// 6.remove tmp
	mFile->setInt(0, 0xffffffff,true);

	return true;
}
#endif 
