////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/01/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdGen/IdGenDef.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Except.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "Util/File.h"
#include "Util/FileWBack.h" 
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosIdGenDef::AosIdGenDef(
		const AosFileWBackPtr &file, 
		const u32 idx)
:
mIdx(idx),
mMaxLen(0),
mBlockSize(0),
mNextBlockSize(0),
mMaxId(0),
mRemoteAddr("192.168.99.26"), 
mRemotePort(5580) 
{
	OmnString errmsg;
	if (!readDef(file, idx, errmsg))
	{
		OmnAlarm << "Failed to read the def: " << errmsg << enderr;
		OmnExcept e(OmnFileLine, errmsg);
		throw e;
	}

	/*
	//john 2010/11/15
	OmnString connerrmsg;
	do
	{
		if (connerrmsg != "")
			OmnAlarm << connerrmsg << enderr;
		mConn = OmnNew OmnTcpClient("IdGenDef", mRemoteAddr, mRemotePort, 1, eAosTLT_FirstFourHigh);
		OmnSleep(1);
	}
	while(!mConn->connect(connerrmsg));
	*/
}


AosIdGenDef::~AosIdGenDef()
{
}


bool	
AosIdGenDef::readDef(
		const AosFileWBackPtr &file, 
		const int idx,
		OmnString &errmsg)
{
	// It reads the info into this class. The record is identified
	// by 'idx', which should not be 0. 
	// 	Record Size				u32
	// 	Idx						u32
	// 	Max Length				u32
	// 	BlockSize				u32
	// 	Next Block Size			u32
	// 	IdName					100 (max)
	// 	CrtId					100 (max)
	// 	MaxId					20
	// 	def						
	aos_assert_r(file, false);
	u32 start = idx * eRecordSize + eRecordStart;

	// Read the record size, which should be eRecordSize
	u32 size;
	bool rslt = file->readBinaryU32(start + eOffsetRecordSize, 
		size, 0, eRecordSize, eRecordSize);
	aos_assert_r(rslt, false);
	if (size != eRecordSize)
	{
		errmsg = "Size incorrect: ";
		errmsg << size << ":" << eRecordSize;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Read the flag (for error detection)
	u32 flag;
	rslt = file->readBinaryU32(start + eOffsetFlag, 
		flag, 0, eFlag, eFlag);
	aos_assert_r(rslt, false);
	if (flag != eFlag)
	{
		errmsg = "Data corrupted: ";
		errmsg << flag << ":" << eFlag;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Read max length
	rslt = file->readBinaryU32(start + eOffsetMaxLen, 
		mMaxLen, 0, 0, eMaxLenValue);
	aos_assert_r(rslt, false);

	// Read block size
	rslt = file->readBinaryU32(start + eOffsetBlockSize, 
		mBlockSize, 0, 1, eMaxBlockSize);
	aos_assert_r(rslt, false);
	if (mBlockSize == 0)
	{
		errmsg = "block size failed!";
		return false;
	}

	// Read next block size 
	rslt = file->readBinaryU32(start + eOffsetNextBlockSize, 
		mNextBlockSize, 0, 1, eMaxBlockSize);
	aos_assert_r(rslt, false);
	if (mNextBlockSize == 0)
	{
		errmsg = "Next block size failed";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Read the name
	char buff[eIdNameMaxLen+1];
	int bytesread = file->readToBuff(start + eOffsetIdName, eIdNameMaxLen, buff);
	mIdName = buff;
	if (mIdName.length() > eIdNameMaxLen)
	{
		errmsg = "IdName failed";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Read current id
	char buff1[eCrtIdMaxLen+1];
	bytesread = file->readToBuff(start + eOffsetCrtId, eCrtIdMaxLen, buff1);
	mCrtId = buff1;
	if (mCrtId.length() > eCrtIdMaxLen)
	{
		errmsg = "CrtId failed!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Read Max id
	bytesread = file->readToBuff(start + eOffsetMaxId, eMaxIdMaxLen, buff1);
	mMaxId = atoll(buff1);

	// Read the ranges
	char buff2[eRangesMaxLen+1];
	bytesread = file->readToBuff(start + eOffsetRanges, eRangesMaxLen, buff2);
	mRanges = buff2;
	if (mRanges.length() > eRangesMaxLen)
	{
		errmsg = "Ranges failed!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	return true;
}


bool
AosIdGenDef::updateId(const OmnString &crtid)
{
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);

	u32 start = mIdx * eRecordSize + eRecordStart;

	bool rslt = file->put(start + eOffsetCrtId, crtid.data(), crtid.length() + 1, true); 
	aos_assert_r(rslt, false);
	return true;
}


AosIdGenDefPtr	
AosIdGenDef::createNewDef(
		const OmnString &name,
		const AosFileWBackPtr &file, 
		const AosXmlTagPtr &def, 
		const int idx)
{
	// This function creates a new id gen definition. For the record
	// format, please refer to the comments in 'readDef(...)'.
	// The caller should have locked the file and the caller should
	// have affirmed that the slot was not used by someone else.
	
	aos_assert_r(file, 0);
	aos_assert_r(def, 0);

	// 'idx' must be the same as the number of records, or
	// 'idx' must be the last entry. 
	int num_records;
	bool rslt = file->readBinaryInt(eOffsetNumRecords, 
			num_records, -1, -2, eMaxNumRecords);
	aos_assert_r(rslt, 0);
	if (num_records < 0) num_records = 0;
	aos_assert_r(idx == num_records, 0);

	u32 start = idx * eRecordSize + eRecordStart;

	// Set the record size, which should be eRecordSize
	rslt = file->setU32(start + eOffsetRecordSize, eRecordSize, false);
	aos_assert_r(rslt, 0);

	// Set the flag (for error detection)
	rslt = file->setU32(start + eOffsetFlag, eFlag, false);
	aos_assert_r(rslt, 0);

	// Set max length
	u64 maxlen = def->getAttrU64(AOSCONFIG_MAXLEN, 0);
	rslt = file->setU32(start + eOffsetMaxLen, maxlen, false);
	aos_assert_r(rslt, 0);

	// Set block size
	//u64 blocksize = def->getAttrU64(AOSCONFIG_BLOCKSIZE, 0);
	//ken 2011/9/21
	u64 blocksize = def->getAttrU64(AOSCONFIG_BLOCKSIZE, 1);
	rslt = file->setU32(start + eOffsetBlockSize, blocksize, false);
	aos_assert_r(rslt, 0);

	// Read next block size 
	//u64 nextblocksize = def->getAttrU64(AOSCONFIG_NEXTBLOCKSIZE, 0);
	//ken 2011/9/21
	u64 nextblocksize = def->getAttrU64(AOSCONFIG_NEXTBLOCKSIZE, 1);
	rslt = file->setU32(start + eOffsetNextBlockSize, nextblocksize, false);
	aos_assert_r(rslt, 0);

	// Set the name
	aos_assert_r(name.length() < eIdNameMaxLen, 0);
	rslt = file->put(start + eOffsetIdName, name.data(), name.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set current id
	OmnString crtid = def->getAttrStr(AOSCONFIG_CRTID);
	aos_assert_r(crtid.length() < eCrtIdMaxLen, 0);
	rslt = file->put(start + eOffsetCrtId, crtid.data(), crtid.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set max id
	rslt = file->put(start + eOffsetMaxId, crtid.data(), crtid.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set the ranges
	OmnString ranges = def->getAttrStr(AOSCONFIG_RANGES);
	aos_assert_r(ranges.length() < eRangesMaxLen, 0);
	rslt = file->put(start + eOffsetRanges, ranges.data(), ranges.length()+1, true);
	aos_assert_r(rslt, 0);

	try
	{
		AosIdGenDefPtr def = OmnNew AosIdGenDef(file, idx);
		file->setInt(eOffsetNumRecords, idx+1, true);
		return def;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create: " << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool	
AosIdGenDef::readDefs(
		const AosFileWBackPtr &file, 
		AosIdGenDefPtr *defs, 
		int &num)
{
	aos_assert_r(file, false);

	bool rslt = file->readBinaryInt(eOffsetNumRecords, 
		num, -1, 0, eMaxNumRecords);
	aos_assert_r(rslt, false);
	if (num < 0)
	{
		OmnAlarm << "Failed to read IdGen Record: " << file->toString() << enderr;
		return false;
	}

	for (int i=0; i<num; i++)
	{
		defs[i] = OmnNew AosIdGenDef(file, i);
		aos_assert_r(defs[i], false);
	}

	return true;
}


bool 
AosIdGenDef::getNextBlock(
		u64 &newId, 
		u64 &maxId)
{
	// It reads the current id, increments by the block size, 
	// and sets the block size. It then updates the file. 
	
	/*
	//john 2010/11/15
	mConn->smartSend(mIdName.getBuffer(), mIdName.length());
	OmnConnBuffPtr buff;
	int bytes = mConn->smartRead(buff);
	aos_assert_r(bytes > 0, false);

	char *data = buff->getData();
	newId = atoll(data);
	aos_assert_r(newId>0, false);
	maxId = newId + mBlockSize;

	mCrtId = "";
	mCrtId << maxId;
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);
	u32 start = mIdx * eRecordSize + eRecordStart;
	bool rslt = file->put(start + eOffsetCrtId, mCrtId.data(), mCrtId.length()+1, true);
	aos_assert_r(rslt, false);

	OmnString mm;
	mm << maxId;
	rslt = file->put(start + eOffsetMaxId, mm.data(), mm.length()+1, true);
	return true;
	*/
	u64 crtid = atoll(mCrtId.data());
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);
	if (!incrementCrtId(file, newId))
	{
		OmnAlarm << "Failed to read!" << enderr;
		return false;
	}
	maxId = mMaxId;
	aos_assert_r((u64)atoll(mCrtId.data()) > crtid, false);

	return true;

}

/*
bool
AosIdGenDef::getNextBlock(u64 &newId, u32 &blocksize, OmnString name)
{
	// It reads the current id, increments by the block size, 
	// and sets the block size. It then updates the file. 
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);
	if (!incrementCrtId(file, newId))
	{
		OmnAlarm << "Failed to read!" << enderr;
		return false;
	}
	blocksize = mBlockSize;
	return true;
}
*/


bool
AosIdGenDef::incrementCrtId(const AosFileWBackPtr &file, u64 &crtid)
{
	char buff[eCrtIdMaxLen+1];
	u32 start = mIdx * eRecordSize + eRecordStart;
	int bytesread = file->readToBuff(start + eOffsetCrtId, eCrtIdMaxLen, buff);
	aos_assert_r(bytesread > 0, false);
	int len = strlen(buff);
	aos_assert_r(bytesread >= len, false);
	aos_assert_r(len < eCrtIdMaxLen && len > 0, false);
	crtid = atoll(buff);
	crtid += mBlockSize;
	mCrtId = "";
	mCrtId << crtid;
	bool rslt = file->put(start + eOffsetCrtId, mCrtId.data(), mCrtId.length()+1, true);
	aos_assert_r(rslt, false);

	OmnString maxid;
	mMaxId = crtid + mBlockSize - 1;
	rslt = file->put(start + eOffsetMaxId, maxid.data(), maxid.length()+1, true);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIdGenDef::getCrtidU64(u64 &crtid)
{
	char *endptr;
	crtid = strtoll(mCrtId.data(), &endptr, 10);
	aos_assert_r(crtid != 0, false);
	return true;
}


bool
AosIdGenDef::setCrtid(const u64 &id)
{
	mCrtId = "";
	mCrtId << id;
	aos_assert_r(modifyField(eOffsetCrtId, mCrtId), false);
	return true;
}


bool
AosIdGenDef::setBlocksize(const u32 &bsize)
{
	mBlockSize = bsize;
	aos_assert_r(modifyField(eOffsetBlockSize, mBlockSize), false);
	return true;
}


bool
AosIdGenDef::modifyField(const u32 offset, const OmnString &value)
{
	u32 start = mIdx * eRecordSize + eRecordStart + offset;
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);
	bool rslt = file->put(start, value.data(), value.length()+1, true);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIdGenDef::modifyField(const u32 offset, const u32 &value)
{
	u32 start = mIdx * eRecordSize + eRecordStart + offset;
	AosFileWBackPtr file = AosIdGenMgr::getSelf()->getFile();
	aos_assert_r(file, false);
	bool rslt = file->setU32(start, value, true);
	aos_assert_r(rslt, false);
	return true;
}


AosIdGenDefPtr	
AosIdGenDef::createNewDef(
		const AosFileWBackPtr &file, 
		const AosXmlTagPtr &def, 
		const int idx)
{
	// This function creates a new id gen definition. For the record
	// format, please refer to the comments in 'readDef(...)'.
	// The caller should have locked the file and the caller should
	// have affirmed that the slot was not used by someone else.
	
	aos_assert_r(file, 0);
	aos_assert_r(def, 0);

	// 'idx' must be the same as the number of records, or
	// 'idx' must be the last entry. 
	int num_records;
	bool rslt = file->readBinaryInt(eOffsetNumRecords, 
			num_records, -1, -2, eMaxNumRecords);
	aos_assert_r(rslt, 0);
	if (num_records < 0) num_records = 0;
	aos_assert_r(idx == num_records, 0);

	u32 start = idx * eRecordSize + eRecordStart;

	// Set the record size, which should be eRecordSize
	rslt = file->setU32(start + eOffsetRecordSize, eRecordSize, false);
	aos_assert_r(rslt, 0);

	// Set the flag (for error detection)
	rslt = file->setU32(start + eOffsetFlag, eFlag, false);
	aos_assert_r(rslt, 0);

	// Set max length
	u64 maxlen = def->getAttrU64(AOSCONFIG_MAXLEN, 0);
	rslt = file->setU32(start + eOffsetMaxLen, maxlen, false);
	aos_assert_r(rslt, 0);

	// Set block size
	u64 blocksize = def->getAttrU64(AOSCONFIG_BLOCKSIZE, 0);
	rslt = file->setU32(start + eOffsetBlockSize, blocksize, false);
	aos_assert_r(rslt, 0);

	// Read next block size 
	u64 nextblocksize = def->getAttrU64(AOSCONFIG_NEXTBLOCKSIZE, 0);
	rslt = file->setU32(start + eOffsetNextBlockSize, nextblocksize, false);
	aos_assert_r(rslt, 0);

	// Set the name
	OmnString name = def->getTagname();
	aos_assert_r(name.length() < eIdNameMaxLen, 0);
	rslt = file->put(start + eOffsetIdName, name.data(), name.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set current id
	OmnString crtid = def->getAttrStr(AOSCONFIG_CRTID);
	aos_assert_r(crtid.length() < eCrtIdMaxLen, 0);
	rslt = file->put(start + eOffsetCrtId, crtid.data(), crtid.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set max id
	rslt = file->put(start + eOffsetMaxId, crtid.data(), crtid.length()+1, false);
	aos_assert_r(rslt, 0);

	// Set the ranges
	OmnString ranges = def->getAttrStr(AOSCONFIG_RANGES);
	aos_assert_r(ranges.length() < eRangesMaxLen, 0);
	rslt = file->put(start + eOffsetRanges, ranges.data(), ranges.length()+1, true);
	aos_assert_r(rslt, 0);

	try
	{
		AosIdGenDefPtr def = OmnNew AosIdGenDef(file, idx);
		file->setInt(eOffsetNumRecords, idx+1, true);
		return def;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create: " << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


