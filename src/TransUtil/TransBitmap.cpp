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
// 12/06/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TransBitmap.h"

#include "API/AosApi.h"
#include "SEUtil/LogFile.h"
#include "TransUtil/TransUtil.h"
#include "TransBasic/Trans.h"
#include "Util/File.h"


AosTransBitmap::AosTransBitmap(
		const OmnString &dir_name,
		const OmnString &file_name,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mShowLog(show_log)
{
	// Chen Ding, 12/10/2012
	//mBitmap = AosBitmapObj::getBitmapStatic();

	//mFnames[0] = dir_name;
	//mFnames[0] << "/" << file_name << "_tmp1";
	
	//mFnames[1] = dir_name;
	//mFnames[1] << "/" << file_name << "_tmp2";

	OmnString finish_log_fname = file_name;
	finish_log_fname << "_log";
	mFinishLog = OmnNew AosLogFile(dir_name, finish_log_fname, eMaxLogFileSize); 
	mFinishLog->start();	

	//recoverBitmap();
	procFinishLogFile();
}


AosTransBitmap::~AosTransBitmap()
{
}


/*
bool
AosTransBitmap::recoverBitmap()
{
	OmnFilePtr file1 = OmnNew OmnFile(mFnames[0], OmnFile::eReadWrite AosMemoryCheckerArgs);
	OmnFilePtr file2 = OmnNew OmnFile(mFnames[1], OmnFile::eReadWrite AosMemoryCheckerArgs);
	
	if (!file1->isGood() && !file2->isGood())
	{
		// This means there is no any bitmap file.
		initCrtFile(mFnames[0]);
		return true;
	}

	if (!file1->isGood())
	{
		// file2 will good.
		bool correct = fileIsCorrect(file2);
		if(!correct)
		{
			initCrtFile(mFnames[0]);
			return true;
		}
		
		initBitmap(file2);	
		initCrtFile(mFnames[0]);
		return true;
	}
	
	if (!file2->isGood())
	{
		// file1 will good.
		bool correct = fileIsCorrect(file1);
		if(!correct)
		{
			initCrtFile(mFnames[0]);
			return true;
		}
		
		initBitmap(file1);
		initCrtFile(mFnames[1]);
		return true;
	}

	// Chen Ding, 05/30/2012
	// If both files are empty, reset it.
	if (file1->getLength() == 0 && file2->getLength() == 0)
	{
		initCrtFile(mFnames[0]);
		return true;
	}

	bool correct1 = fileIsCorrect(file1);
	bool correct2 = fileIsCorrect(file2);
	if (!correct1 && !correct2)
	{
		OmnAlarm << "TransBitmap files corrupted: " 
			<< file1->getFileName() << ":" << file2->getFileName() << enderr;
		return false;
	}
	
	if (!correct1)
	{
		initBitmap(file2);
		initCrtFile(mFnames[0]);
		return true;
	}

	if (!correct2)
	{
		initBitmap(file1);
		initCrtFile(mFnames[1]);
		return true;
	}
		
	u32 create_time1 = file1->readBinaryU32(eCreateTimeOffset, 0);	
	u32 create_time2 = file2->readBinaryU32(eCreateTimeOffset, 0);
	if(create_time1 > create_time2)
	{
		initBitmap(file1);
		initCrtFile(mFnames[1]);
	}
	else
	{
		initBitmap(file2);
		initCrtFile(mFnames[0]);
	}

	return true;
}


bool
AosTransBitmap::fileIsCorrect(const OmnFilePtr &file)
{
	u32 fileSize = file->getLength();
	if(fileSize < 8)	return false;

	u32 p1 = file->readBinaryU32(eStartPoisonOffset, 0);
	u32 size = file->readBinaryU32(eBitmapSizeOffset, 0);
	if(fileSize < (eBitmapOffset + size + 4)) return false;

	u32 p2 = file->readBinaryU32(eBitmapOffset + size, 0);
	if(p1 != eTmpPoisonNum || p2 != eTmpPoisonNum)
	{
		// OmnScreen << "file name:" << file->getFileName()
		//		<< "; eBitmap_size:" << size << endl;
		return false;
	}
	
	return true;
}


bool
AosTransBitmap::initCrtFile(const OmnString &fname)
{
	mCrtFname = fname;
	mCrtFile = OmnNew OmnFile(mCrtFname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!mCrtFile || !mCrtFile->isGood())
	{
		OmnAlarm << "Failed to open the file:" << mCrtFname << enderr;
		return false;
	}
	return true;	
}


bool
AosTransBitmap::initBitmap(const OmnFilePtr &file)
{
	aos_assert_r(file && file->isGood(), false);

	u32 size = file->readBinaryU32(eBitmapSizeOffset, 0);
	aos_assert_r(size != 0, false);
	
	char data[size];
	int bytes_read = file->readToBuff(eBitmapOffset, size, data);
	aos_assert_r(bytes_read > 0 && (u32)bytes_read == size, false);

	AosBuffPtr buff = OmnNew AosBuff(data, size, size AosMemoryCheckerArgs);
	mBitmap->loadFromBuff(buff);
	return true;
}
*/

bool
AosTransBitmap::procFinishLogFile()
{
	if(mFinishLog->empty())	return true;

	int file_seqno = mFinishLog->begSeq();
	while(file_seqno >=0)
	{
		AosBuffPtr file_buff;
		mFinishLog->readEachFile(file_seqno, file_buff);
		aos_assert_r(file_buff, false);

		while(1)
		{
			AosTransId tid = AosTransId::serializeFrom(file_buff);
			if(tid == AosTransId::Invalid)	break;
			
			mFinishTids.insert(tid);
		}

		file_seqno = mFinishLog->getNextSeqno(file_seqno);
	}
	return true;
}


void
AosTransBitmap::addFinishTids(vector<AosTransId> &v_tids)
{
	// max finsih tid not means max recved.
	AosBuffPtr ids_buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	mLock->lock();
	for(u32 i=0; i<v_tids.size(); i++)
	{
		AosTransId tid = v_tids[i];
		mFinishTids.insert(tid);
		
		if(mShowLog)
		{
			OmnScreen << "Client log; finish trans; transid:" << tid.toString() 
				<< endl;
		}
		tid.serializeTo(ids_buff);
	}
	mFinishLog->addData(ids_buff->data(), ids_buff->dataLen());

	mLock->unlock();
}


void
AosTransBitmap::removeFinishTid(const AosTransId &trans_id)
{
	mLock->lock();
	set<AosTransId>::iterator itr = mFinishTids.find(trans_id);
	if(itr != mFinishTids.end())	mFinishTids.erase(itr);
	//mBitmap->removeDocid(trans_id);
	mLock->unlock();
}


bool
AosTransBitmap::checkFinish(const AosTransId &trans_id)
{
	mLock->lock();
	set<AosTransId>::iterator itr = mFinishTids.find(trans_id);
	bool exist = (itr != mFinishTids.end());
	mLock->unlock();
	return exist;
}


/*
void
AosTransBitmap::saveToFile()
{
	//if (!mCrtFile || !mCrtFile->isGood()) return;
	aos_assert(mCrtFile && mCrtFile->isGood());
	
	mLock->lock();
	AosBuffPtr buff = OmnNew AosBuff(100<<10 AosMemoryCheckerArgs);
	mBitmap->saveToBuff(buff);
	u32 size = buff->dataLen();

	u32 mm = eTmpPoisonNum;
	mCrtFile->append((char *)&mm, sizeof(u32), false);		// set start poison
	u32 time = OmnGetSecond();
	mCrtFile->append((char *)&time, sizeof(u32), false);
	
	mCrtFile->append((char *)&size, sizeof(u32), false);
	mCrtFile->append(buff->data(), buff->dataLen(), false);	
	mCrtFile->append((char *)&mm, sizeof(u32), true);	// set end poison

	mCrtFname = (mCrtFname == mFnames[0] ? mFnames[1] : mFnames[0]);
	mCrtFile = OmnNew OmnFile(mCrtFname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!mCrtFile || !mCrtFile->isGood())
	{
		OmnAlarm << "Failed to open the file:" << mCrtFname << enderr;
	}

	mFinishLog->deleteAllFiles();
	mLock->unlock();
}
*/


void
AosTransBitmap::saveToFile()
{
	
	AosBuffPtr ids_buff = OmnNew AosBuff(500, 0 AosMemoryCheckerArgs);
	mLock->lock();
	
	mFinishLog->deleteAllFiles();

	set<AosTransId>::iterator itr = mFinishTids.begin();
	for(; itr != mFinishTids.end(); itr++)
	{
		AosTransId tid = *itr;
		tid.serializeTo(ids_buff);
	}
	mFinishLog->addData(ids_buff->data(), ids_buff->dataLen());
	mLock->unlock();
}

