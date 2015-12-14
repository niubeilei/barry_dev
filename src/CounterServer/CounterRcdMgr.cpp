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
// This class is responsible for creating and reading counter records. 
// The class assumes the following file:
// 	1. Control File: it contains the information about the manager
//  2. Record Files: all records are stored in record files. Each record
//     file is fixed length. Counter records are fixed length. The first
//     record in each record file is reserved as the control record.
//   
//  Record Format is:
//  	Total				u32	
//  	CounterId			u64
//  	CounterValue		int64
//  	Name IILID			u64
//  	Value IIIID			u64
//  	Docid				u64
//  	Seqno				u32
//  	Offset				u64
//  	Time Record Size	u32
//  	Time Record Type	u32
//  	Max					int64
//  	Min					int64
//  	Mean				int64
//  	Standard Error		int64
//  	Number of Counters	int64
//  	Extension Seqno		u32
//  	Extension Offset	u64
//  	Cname				variable
//
// Modification History:
// 05/26/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterServer/CounterRcdMgr.h"

#include "Util/StrSplit.h"
#include "CounterUtil/CounterStatType.h"
#include "XmlUtil/XmlTag.h"


AosCounterRcdMgr::AosCounterRcdMgr(const AosXmlTagPtr &cfg, const u32 id)
:
mLock(OmnNew OmnMutex()),
mId(id),
mCrtFileSeqno(0),
mCrtFileLength(0)
{
	config(cfg);
}


bool
AosCounterRcdMgr::config(const AosXmlTagPtr &config) 
{
	aos_assert_r(config, false);

	mFileDir = config->getAttrStr("control_filedir");
	mCtlFileName = config->getAttrStr("control_filename");
	mRecordFileName = config->getAttrStr("record_filename");
	aos_assert_r(mFileDir != "", false);
	aos_assert_r(mCtlFileName != "", false);
	aos_assert_r(mRecordFileName != "", false);

	reset();

	if (!mCtlFile) openControlFile();   // opens the control file

	aos_assert_r(mCtlFile, false);

	OmnFilePtr crtRecordFile = getFile(mCrtFileSeqno); // gets the current record file
	aos_assert_r(crtRecordFile, false);

	mCrtFileLength = crtRecordFile->getLength();  // reads the current file length
	return true;
}


bool
AosCounterRcdMgr::reset()
{
	if (!mCtlFile) openControlFile(); 
	aos_assert_r(mCtlFile, false);

	char mem[20];
	AosBuff buff(mem, 20, 0, 0 AosMemoryCheckerArgs);
	buff.setU32(0); // resets the flag
	buff.setU32(0); // resets the seqno
	buff.setU32(0); // resets the offset

	mCtlFile->put(0, mem, 20, true);

	mCrtFileSeqno = 0;
	mCrtFileLength = 0;
	return true;
}


bool
AosCounterRcdMgr::openControlFile()
{
	// The function reset the control file when 'reset' is true.
	// The file fisrt 16 bytes should be in the form:
	// 		4 bytes   flag
	// 		4 bytes   crtSeqno
	// 		4 bytes   crtOffset
	// In Addtion, make sure that This class keep crtSeqno and crtOffset information.
	aos_assert_r(mFileDir != "" && mCtlFileName != "", false);
	OmnString fullPath = mFileDir;
	fullPath << "/" << mCtlFileName << "_" << mId;
	mCtlFile = OmnNew OmnFile(fullPath, OmnFile::eReadWrite);
	if(!mCtlFile->isGood())
	{
		mCtlFile = OmnNew OmnFile(fullPath, OmnFile::eCreate);
	}
	if (!mCtlFile->isGood())
	{
		OmnString errmsg = "Failed to open th control file";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	char mem[20];
	AosBuff buff(mem, 20, 20, 0 AosMemoryCheckerArgs);
	mLock->lock();
	mCtlFile->readToBuff(0, 20, mem);
	mLock->unlock();
	buff.getU32(0);
	mCrtFileSeqno = buff.getU32(0);

	return true;
}


OmnFilePtr
AosCounterRcdMgr::openCrtRecordFile(const u32 &seqno)
{
	OmnString fullRcdPath = mFileDir;
	fullRcdPath << "/" << mRecordFileName << "_" << mId <<  "_" << seqno;
	OmnFilePtr file;
	file = OmnNew OmnFile(fullRcdPath, OmnFile::eReadWrite);
	if (!file->isGood())
	{
		file = OmnNew OmnFile(fullRcdPath, OmnFile::eCreate);
	}
	if(!file->isGood())
	{
		OmnString errmsg =  "Failed to open the current record file";
		OmnAlarm << errmsg << enderr;
		return 0;
	}

	return file;
}


bool
AosCounterRcdMgr::modifyCounter(
		const AosCounterRecordPtr &rcd,
		const AosRundataPtr &rdata)
{
	u32 localSeqno = 0;
	u32 localOffset = 0;
	if (!getLocation(rcd->getCounterId(), localSeqno, localOffset)) return false;

	OmnFilePtr file = getFile(localSeqno);
	aos_assert_rr(file, rdata, false);
	u32 thisRecordSize = file->readBinaryU32(localOffset, 0);
	if (thisRecordSize == 0)
	{
		rdata->setError() << "Read file error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	char *recordMem = OmnNew char[thisRecordSize + 10];
	AosBuff recordBuff(recordMem, thisRecordSize + 10, thisRecordSize + 10, 0 AosMemoryCheckerArgs);
	if (!file->readToBuff(localOffset + sizeof(u32), thisRecordSize, recordMem))
	{
		rdata->setError() << "Read file error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnDelete [] recordMem;
		return false;
	}

	u64 dbCounterId = recordBuff.getU64(0); 		// counterid
	aos_assert_rr(rcd->getCounterId() == dbCounterId, rdata, false);

	recordBuff.setI64(rcd->getCounterValue());		// modify cvalue
	recordBuff.getU64(0);							// nameiilid
	recordBuff.getU64(0);							// valueiilid
	recordBuff.setU32(rcd->getCounterSeqno());		// seqno
	recordBuff.setU64(rcd->getCounterOffset());		// offset
	recordBuff.setU32(rcd->getCounterTimeSize());	// modify size 
	recordBuff.getU32(0);							// time type 
	recordBuff.getU64(0);							// max
	recordBuff.getU64(0);							// min
	recordBuff.getU64(0);							// mean
	recordBuff.getU64(0);							// stderr
	recordBuff.setU64(rcd->getCounterNum());		    // seqno


	file->put(localOffset + sizeof(u32), recordBuff.data(), thisRecordSize, true);

	rdata->setOk();
	OmnDelete [] recordMem;
	return true;
}


AosCounterRecordPtr
AosCounterRcdMgr::readCounter(
		const u64 &counter_id, 
		const AosRundataPtr &rdata)
{
	u32 localSeqno = 0;
	u32 localOffset = 0;
	if (!getLocation(counter_id, localSeqno, localOffset)) return 0; 
	OmnFilePtr file = getFile(localSeqno);
	aos_assert_rr(file, rdata, 0);
	u32 thisRecordSize = file->readBinaryU32(localOffset, 0);
	if (thisRecordSize == 0)
	{
		rdata->setError() << "Read file error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	char *recordMem = OmnNew char[thisRecordSize + 10];
	AosBuff recordBuff(recordMem, thisRecordSize + 10, thisRecordSize + 10, 0 AosMemoryCheckerArgs);
	if (!file->readToBuff(localOffset + sizeof(u32), thisRecordSize, recordMem))
	{
		rdata->setError() << "Read file error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnDelete [] recordMem;
		return 0;
	}

	AosCounterRecordPtr record = OmnNew AosCounterRecord();
	u64 dbCounterId = recordBuff.getU64(0);				// counterid
	aos_assert_rr(dbCounterId == counter_id, rdata, 0);

	record->setCounterId(counter_id);
	record->setCounterValue(recordBuff.getI64(0)); 	// value
	record->setCounterNameIILID(recordBuff.getU64(0)); 	// nameIILID
	record->setCounterValueIILID(recordBuff.getU64(0)); 	// valueIILID
	record->setCounterSeqno(recordBuff.getU32(0));		// time seqno
	record->setCounterOffset(recordBuff.getU64(0));      // time offset
	record->setCounterTimeSize(recordBuff.getU32(0));	// time size 
	record->setCounterTimeType(recordBuff.getU32(0));	// time type 
	record->setCounterMax(recordBuff.getI64(0));		// max
	record->setCounterMin(recordBuff.getI64(0));		// min
	record->setCounterMean(recordBuff.getI64(0));		// mean
	record->setCounterStderr(recordBuff.getI64(0));	// stderr 
	record->setCounterNum(recordBuff.getI64(0));		// number
	record->setCounterExtSeqno(recordBuff.getU32(0));	// ext seqno
	record->setCounterExtOffset(recordBuff.getU64(0));   // ext offset
	record->setCounterName(recordBuff.getStr(""));		// cname

	rdata->setOk();
	OmnDelete [] recordMem;
	return record;
}


OmnFilePtr
AosCounterRcdMgr::getFile(const u32 &seqno)
{
	mLock->lock();
	OmnFilePtr file = mFiles[seqno];
	if (!file) 
	{
		file = openCrtRecordFile(seqno); 
		aos_assert_rl(file, mLock, NULL);
		mFiles[seqno] = file;
	}
	mLock->unlock();
	return file;
}


bool
AosCounterRcdMgr::getLocation(
		const u64 &counterId,
		u32 &seqno,
		u32 &offset)
{
	offset = (u32)counterId;
	seqno = (u32)((counterId - offset) >> 32);
	return true;
}


bool 
AosCounterRcdMgr::createRecord(
		const AosCounterRecord &rcd, 
		const AosRundataPtr &rdata)
{
	// 1. Open the last record file, if not opened yet.
	// 2. Check whether the file is full. If yes, create the next one.
	// 3. Append a new record to the file. 
	// 4. Return the new counter_id
	// The record should be in the form:
	// 		4 bytes 		record size
	// 		8 btyes			counterId
	// 		8 bytes 		value
	// 		8 bytes			name_iilid
	// 		8 bytes			value_iilid
	// 		4 bytes			seqno
	// 		8 bytes			offset
	// 		4 bytes			time size
	// 		4 bytes			time type	
	// 		8 bytes 		max
	// 		8 bytes 		min
	// 		8 bytes 		mean
	// 		8 bytes 		stderr
	// 		8 bytes 		number
	//		4 bytes			extSeqno
	//		8 bytes			extOffset	
	//		n bytes			cname
	
	AosCounterRecord record = rcd;
	u32 localSeqno = 0, localOffset = 0;
	getLocation(record.getCounterId(), localSeqno, localOffset);
	aos_assert_r(localSeqno == mCrtFileSeqno, false);

	OmnFilePtr file = getFile(mCrtFileSeqno); 
	aos_assert_r(file, false);
	u32 thisRecordSize = record.getRecordSize();
	aos_assert_r(localOffset + thisRecordSize == mCrtFileLength, false);

	// proc statistics counter
	if (record.getCounterStatType() != "")
	{
		procStatCounter(record, rdata);
	}
	char *mem = OmnNew char[thisRecordSize + 10];
	AosBuff buff(mem, thisRecordSize + 10, 0, 0 AosMemoryCheckerArgs);

	setRecordBuff(buff, record);
	if (!file->setU32(localOffset, buff.dataLen(), false) ||
		!file->put(localOffset + sizeof(u32), mem, buff.dataLen(), true))
	{
		rdata->setError() << "Write file error!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		OmnDelete [] mem;
		return false;
	}
	OmnDelete [] mem;
	return true;
}


bool
AosCounterRcdMgr::procStatCounter(
		AosCounterRecord &record, 
		const AosRundataPtr &rdata)
{
	// The record should be have a data field called statType.
	// This type should be in the form:
	// max,min,mean....
	aos_assert_r(rdata, false);
	OmnString statType = record.getCounterStatType();
	if (statType == "")
	{
		return true;
	}
	AosStrSplit parser(statType, ",");
	OmnString word;
	while((word = parser.nextWord()) != "")
	{
		AosCounterStatType::E type = AosCounterStatType::toEnum(word);
		switch(type)
		{
		case AosCounterStatType::eCounterMax:
			if (record.getCounterValue() > record.getCounterMax())
			{
				record.setCounterMax(record.getCounterValue());
			}
			break;

		case AosCounterStatType::eCounterMin:
			if (record.getCounterValue() < record.getCounterMin())
			{
				record.setCounterMin(record.getCounterValue());
			}
			break;

		case AosCounterStatType::eCounterMean:
			break;

		case AosCounterStatType::eCounterStderr:
			break;

		case AosCounterStatType::eCounterNumber:
			record.setCounterNum(record.getCounterNum() + 1);
			break;

		default : 
			OmnAlarm << "Invalid counter statistics type: " << word << enderr;
		}
	}
	return true;
}
#endif
