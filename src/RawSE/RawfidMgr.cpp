////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	This class is used to manage raw files whose content are not the concern
//
// Modification History:
// 2014-11-21 created by White
////////////////////////////////////////////////////////////////////////////
#include "RawSE/RawfidMgr.h"

#include <cstdlib>

#include "ErrorMgr/ErrmsgId.h"
#include "SEUtil/EnumFieldName.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

AosRawfidMgr::AosRawfidMgr()
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtrNoLock()),
mFile(OmnNew OmnFile(AosMemoryCheckerArgsBegin)),
mLastAllocatedID(0),
mLastIDReadFromFile(0),
mIdBatchSize(eDftBatchSize)
{
	mHomeDir= getenv("HOME");
	if (NULL == mHomeDir)
	{
		mHomeDir = "./";
	}
}


AosRawfidMgr::~AosRawfidMgr()
{
	//This function tries to write back the head local file
	mFile->writelock();
	int64_t write_pos = 0;
	int64_t write_len = sizeof(mLastAllocatedID);
	mFile->write(write_pos,(const char*)&mLastAllocatedID, write_len, true);
	mFile->unlock();
	mFile->closeFile();	//what if this fails?
}


u64
AosRawfidMgr::allocateRawfid(
		AosRundata *rdata,
		u32 partition_id)
{
	//This function tries to open the local file id file
	//and read the starting id of available file ids and mark
	//them as used by writing the biggest one back when the class
	//does not have any available file ids.
	//the file stores only one local file id as the starting
	//id for next allocation
	mLockRaw->lock();

	if ((mLastAllocatedID >= mLastIDReadFromFile + mIdBatchSize)	//this batch of local file ids have been exhausted
			||(0 == mLastIDReadFromFile))	//the first time
	{
		if(!readBatchFileIDs(rdata))
		{
			mLockRaw->unlock();
			OmnScreen << "rawfid allocation failed due to read local_file_id failed." << endl;
			return 0;
		}
		mLastAllocatedID = mLastIDReadFromFile;
	}

	u64 local_id = ++mLastAllocatedID;
	u64 partition_id_to_compose = partition_id;
	partition_id_to_compose = partition_id_to_compose << 56;
	mLockRaw->unlock();
	// Byte 7 	(partition id)
	// Byte 5-6	(reserved)
	// Byte 4-0	(Raw File Local ID)
	return partition_id_to_compose + local_id;
}


bool
AosRawfidMgr::readBatchFileIDs(AosRundata *rdata)
{
	OmnString local_id_file = mHomeDir;
	local_id_file += "/local_file_id";	//TODO: get it from the configuration
	if (!mFile->openFile1(local_id_file, OmnFile::eReadWrite))
	{
		OmnScreen << "open local_id_file failed." << endl;
		return false;
	}
	mFile->writelock();
	u64 read_write_pos = 0;
	u64 guard = 0;
	mLastIDReadFromFile = mFile->readU64(read_write_pos, guard);
	if (guard == mLastIDReadFromFile)	//meaning failed
	{
		mFile->unlock();
		mFile->closeFile();
		mLockRaw->unlock();
//		AosLogError(rdata, false, AosErrmsgId::eInternalError)
//			<< AosEnumFieldName::eErrorType << mLastAllocatedID << enderr;
		OmnScreen << "read local_file_id failed." << endl;
		return false;
	}
	OmnScreen << "local_file_id read from file:'" << mLastIDReadFromFile << "'" << endl;
	u64 id_to_write_back = mLastIDReadFromFile + mIdBatchSize;
	OmnString id_string = "";
	id_string << id_to_write_back;
	mFile->write(read_write_pos, id_string.getBuffer(), id_string.length(), true);
	mFile->unlock();
	mFile->closeFile();
	mLockRaw->unlock();
	OmnScreen << "local_file_id written to file:'" << id_to_write_back << "' after reading." << endl;
	return true;
}

