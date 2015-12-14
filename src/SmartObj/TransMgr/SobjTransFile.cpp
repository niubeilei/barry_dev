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
// A TransFile is used to save and manage transactions. When a new transaction
// is added, it appends to the file. When a transaction finishes, it marks 
// the transaction as been finished. The concept 'transaction' is generic. 
// Anything that may occur sequentially may be considered as transactions.
// Transactions have certain lifecycles. Transactions may have responses. 
// There may be aging algorithm to remove old transactions. There may be
// various queries and statistics for transactions. 
//
// The main purpose of this class is to make transaction management more 
// efficient. It maintains two file pointers: one for appending and one for
// random accesses. In most cases, it does not use the random accessing.
// For instance, when updating finising, it only append a record to the
// file. 
//
// Model
// A Transaction may or may not have responses. A transaction may have 
// multiple transactions. If so, one of the responses serves as a 'true'
// response and all others are used as 'markers'. For instance, a 
// transaction requires a response as the true response, and a response
// marking the transaction was canceled, or a response marking the 
// transaction is put on hold, etc. Each of these is called a Transaction 
// Event. This model will ignore duplicated events, which 
// means that if a transaction already received 'hold' event, subsequent
// 'hold' events are ignored. Some events only need to be marked, while
// others need to save the responses and record the positions.
//
// File Format:
// 		File Header
// 		Transaction Position Array:		Size = (u32 * mMaxTrans)
// 		Response Position Arrays:		Size = ((u32 * mMaxTrans) + sizeof(int)) * N
// 		Bitmaps:						Size = ((mMaxTrans/8) + sizeof(int)) * M
//		Transactions and Responses
//
// where 'N' is the number of positions to record, and 'M' is the number of
// bitmaps.
//
// Adding Transactions:
// Each file maintains mMaxTrans number of transactions. When this number
// is reached or if the file is too big, new transactions should be added 
// to the next transaction file. Otherwise, the transaction is appended.
//
// mTransPos:
// The class uses 'mTransPos[]' to keep track of the position of all 
// the transactions saved in this file.
//
// mRespPos:
// The class uses 'mRespPos[]' to keep track of the positions of all 
// the responses saved in this file.
//
// Response Type:
// The class assumes all responses have an attribute indicating the response
// type. The attribute name is defined by 'mRespTypeAname'. 
//
// Modification History:
// 02/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/TransMgr/SobjTransFile.h"

#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "SmartObj/TransMgr/SobjTransMgr.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


static u32 sgBitmap[] = 
{
	0x00000001LL,
	0x00000002LL,
	0x00000004LL,
	0x00000008LL,
	0x00000010LL,
	0x00000020LL,
	0x00000040LL,
	0x00000080LL,

	0x00000100LL,
	0x00000200LL,
	0x00000400LL,
	0x00000800LL,
	0x00001000LL,
	0x00002000LL,
	0x00004000LL,
	0x00008000LL,

	0x00010000LL,
	0x00020000LL,
	0x00040000LL,
	0x00080000LL,
	0x00100000LL,
	0x00200000LL,
	0x00400000LL,
	0x00800000LL,

	0x01000000LL,
	0x02000000LL,
	0x04000000LL,
	0x08000000LL,
	0x10000000LL,
	0x20000000LL,
	0x40000000LL,
	0x80000000LL
};


SobjTransFile::SobjTransFile(
		const OmnString &fname, 
		const u32 seqno, 
		const u32 start_transid,
		const int maxTrans, 
		const int maxFilesize, 
		const OmnString &trans_aname,
		const bool flush_flag,
		const vector<bool> &bitmaps, 
		const vector<bool> &respPos, 
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mIsGood(false),
mStartTransid(start_transid),
mMaxTrans(maxTrans),
mLastSavedPos(0),
mLastModTime(0),
mFname(fname),
mTransAname(trans_aname),
mFileMaxSize(maxFilesize),
mFlushFlag(flush_flag),
mSeqno(seqno),
mNextTransId(start_transid)
{
	bool rslt = createNewFile(bitmaps, respPos, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
	mIsGood = true;
}


SobjTransFile::SobjTransFile(
		const OmnString &fname, 
		const OmnString &trans_aname,
		const bool flush_flag,
		const vector<bool> &bitmaps, 
		const vector<bool> &respPos, 
		const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mIsGood(false),
mStartTransid(0),
mMaxTrans(0),
mLastSavedPos(0),
mLastModTime(0),
mFname(fname),
mTransAname(trans_aname),
mFlushFlag(flush_flag),
mNextTransId(0)
{
	bool rslt = openFileOnCreation(bitmaps, respPos, rdata);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
	mIsGood = true;
}


SobjTransFile::~SobjTransFile()
{
}


bool 
SobjTransFile::addTrans(
		u32 &transid, 
		const AosXmlTagPtr &trans, 
		bool &needCreateNewFile,
		const AosRundataPtr &rdata)
{
	// When adding a transaction, this class needs to assign a transaction
	// id to it. It first generates a transaction id, and then appends the
	// transaction to the end of the file. If the file is too big, the 
	// transaction will not be appended and 'needCreateNewFile' is set to
	// true.
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);
	needCreateNewFile = false;
	aos_assert_rr(mTransAname != "", rdata, false);
	mLock->lock();
	aos_assert_rl(mAppendFile, mLock, false);
	transid = getNextTransid();
	aos_assert_rl(transid >= mStartTransid, mLock, false);
	u32 idx = transid - mStartTransid;
	if (idx > (u32)mMaxTrans || isFileTooBig())
	{
		// Too many transactions in this file. Need to create a new 
		// transaction file.
		needCreateNewFile = true;
		mLock->unlock();
		return false;
	}

	trans->setAttr(mTransAname, transid);

	int64_t pos = appendRecord(transid, 0, false, false, trans, rdata);
	aos_assert_rl(pos > 0 && pos < eMaxFileSize, mLock, false);
	aos_assert_rl(mTransPos, mLock, false);
	mTransPos[idx] = (u32)pos;
	mLock->unlock();
	return true;
}


bool
SobjTransFile::procResponse(
		const u32 transid,
		const AosXmlTagPtr &response, 
		const int event_id,
		const bool mark,
		const bool track,
		const SobjTransMgrPtr &mgr,
		const AosRundataPtr &rdata)
{
	// After a transaction is sent, it may receive responses. Different responses
	// may trigger diffferent events. 
	// This class assumes that responses have an attribute indicating the 
	// response type. 
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(response, rdata, false);
	aos_assert_rr(mStartTransid > 0, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);

	// Retrieving the transaction ID
	if (transid >= (u32)mStartTransid && transid < (u32)mMaxTrans)
	{
		mgr->logError(AOSTERM("invalid_transid_???", rdata), response);
		return false;
	}

	if (event_id < 0 || event_id >= eMaxEvents)
	{
		mgr->logError(AOSTERM("invalid_event_id_001", rdata), response);
		return false;
	}

	mLock->lock();
	if (mark)
	{
		bool rslt = markBitmapLocked(event_id, transid, response, mgr, rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	bool saved = false;
	if (track)
	{
		bool rslt = saveResponseLocked(event_id, transid, mark, track, response, mgr, rdata);
		if (!rslt)
		{
			mLock->unlock();
			mgr->logError(AOSTERM("failed_save_resp", rdata), response);
			return false;
		}
		saved = true;
	}

	if (mark && !saved)
	{
		appendRecord(transid, event_id, mark, track, rdata);
	}
	mLock->unlock();
	return true;
}


bool
SobjTransFile::createNewFile(
		const vector<bool> &bitmaps,
		const vector<bool> &resp_pos,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0 && mMaxTrans < eMaxTransLimit, rdata, false);
	aos_assert_rr(bitmaps.size() < eMaxEvents, rdata, false);
	aos_assert_rr(resp_pos.size() < eMaxEvents, rdata, false);

	mLock->lock();
	int num_bitmap_entries = getBitmapEntries();
	try
	{
		// 1. Create mTransPos
		mTransPos = OmnNew u32[mMaxTrans];

		// 2. Create Bitmaps and resp pos
		for (u32 i=0; i<bitmaps.size(); i++)
		{
			if (bitmaps[i])
			{
				mBitmaps[i] = OmnNew u32[num_bitmap_entries];
			}
			else
			{
				mBitmaps[i] = 0;
			}

			if (resp_pos[i])
			{
				mRespPos[i] = OmnNew u32[mMaxTrans];
			}
			else
			{
				mRespPos[i] = 0;
			}
		}
	}

	catch (...)
	{
		OmnAlarm << "Run out of memory!" << enderr;
		mLock->unlock();
		return false;
	}

	bool rslt = openFileLocked(rdata);
	aos_assert_rl(rslt, mLock, false);

	rslt = saveControlInfoLocked(rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}


bool
SobjTransFile::openFileOnCreation(
		const vector<bool> &bitmaps,
		const vector<bool> &resp_pos,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0 && mMaxTrans < eMaxTransLimit, rdata, false);
	aos_assert_rr(bitmaps.size() < eMaxEvents, rdata, false);
	aos_assert_rr(resp_pos.size() < eMaxEvents, rdata, false);

	mLock->lock();
	int num_bitmap_entries = getBitmapEntries();
	try
	{
		// 1. Create mTransPos
		mTransPos = OmnNew u32[mMaxTrans];

		// 2. Create Bitmaps and resp pos
		for (u32 i=0; i<bitmaps.size(); i++)
		{
			if (bitmaps[i])
			{
				mBitmaps[i] = OmnNew u32[num_bitmap_entries];
			}
			else
			{
				mBitmaps[i] = 0;
			}

			if (resp_pos[i])
			{
				mRespPos[i] = OmnNew u32[mMaxTrans];
			}
			else
			{
				mRespPos[i] = 0;
			}
		}
	}

	catch (...)
	{
		OmnAlarm << "Run out of memory!" << enderr;
		mLock->unlock();
		return false;
	}

	bool rslt = openFileLocked(rdata);
	aos_assert_rl(rslt, mLock, false);

	rslt = readFromFile(rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
SobjTransFile::markBitmapLocked(
		const int event_id, 
		const u32 transid, 
		const AosXmlTagPtr &response,
		const SobjTransMgrPtr &mgr,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);
	aos_assert_rr(event_id >= 0 && event_id < eMaxEvents, rdata, false);
	u32 bitmap_size = getBitmapEntries();
	if (!mBitmaps[event_id])
	{
		if (mgr)
		{
			mgr->logError(AOSTERM("internal_error", rdata), response);
			return false;
		}
	}

	int offset;
	int entry_idx = getBitmapEntryIdx(transid, offset);
	if (entry_idx < 0 || (u32)entry_idx >= bitmap_size)
	{
		if (mgr)
		{
			mgr->logError(AOSTERM("invalid_entry_idx", rdata), response);
			return false;
		}
		OmnAlarm << "Invalid entry index: " << entry_idx << enderr;
		return false;
	}

	if (offset < 0 || offset >= 32)
	{
		if (mgr)
		{
			mgr->logError(AOSTERM("invalid_offset_001", rdata), response);
			return false;
		}
		OmnAlarm << "Invalid offset: " << offset << enderr;
		return false;
	}

	mBitmaps[event_id][entry_idx] |= sgBitmap[offset];
	return true;
}


bool 
SobjTransFile::saveResponseLocked(
		const int event_id,
		const u32 transid,
		const bool mark,
		const bool track,
		const AosXmlTagPtr &response, 
		const SobjTransMgrPtr mgr,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0 && mMaxTrans < eMaxTransLimit, rdata, false);
	aos_assert_rr(event_id >= 0 && event_id < eMaxEvents, rdata, false);
	aos_assert_rr(mAppendFile, rdata, false);
	aos_assert_rr(mgr, rdata, false);
	if (!mRespPos[event_id])
	{
		if (mgr)
		{
			mgr->logError(AOSTERM("invalid_event_id_001", rdata), response);
			return false;
		}
		OmnAlarm << "Invalid event id: " << event_id << enderr;
		return false;
	}

	int pos_idx = transid - mStartTransid;
	if (pos_idx < 0 || pos_idx >= mMaxTrans)
	{
		mgr->logError(AOSTERM("invalid_position", rdata), response);
		return false;
	}

	if (mRespPos[event_id][pos_idx] > 0)
	{
		// The response has already been received. Do nothing.
		return true;
	}

	aos_assert_rr(response, rdata, false);
	int64_t pos = appendRecord(transid, event_id, mark, track, response, rdata);
	aos_assert_r(pos > 0, false);
	mRespPos[event_id][pos_idx] = (u32)pos;
	return true;
}


bool 
SobjTransFile::updateRespPosLocked(
		const int event_id,
		const u32 transid,
		const u32 pos,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);
	aos_assert_rr(event_id >= 0 && event_id < eMaxEvents, rdata, false);
	aos_assert_rr(mAppendFile, rdata, false);
	aos_assert_rr(transid >= mStartTransid, rdata, false);
	aos_assert_rr(mMaxTrans > 0 && mMaxTrans <= eMaxTransLimit, rdata, false);
	aos_assert_rr(mRespPos[event_id], rdata, false);

	int pos_idx = transid - mStartTransid;
	aos_assert_rr(pos_idx >= 0 && pos_idx < mMaxTrans, rdata, false);
	aos_assert_rr(!mRespPos[event_id][pos_idx], rdata, false);
	mRespPos[event_id][pos_idx] = pos;
	return true;
}


int64_t
SobjTransFile::appendRecord(
		const u32 transid,
		const int event_id,
		const bool mark,
		const bool track,
		const AosXmlTagPtr &trans, 
		const AosRundataPtr &rdata)
{
	// This function appends a record to the file.
	// Record format is:
	// 		Length			four bytes
	// 		poison			four bytes
	// 		transid			four bytes
	// 		event_id		two bytes 
	// 		mark and track	one byte
	// 		contents		variable (optional)	
	// 		next transid	four bytes
	aos_assert_rr(mIsGood, rdata, -1);
	aos_assert_rr(mAppendFile, rdata, -1);
	aos_assert_rr(trans, rdata, -1);
	OmnString str = trans->toString();
	int bufflen = str.length() + 100;
	AosBuff buff(bufflen, 0 AosMemoryCheckerArgs);

	char bb = 0;
	if (mark) bb |= eMarkBitmask;
	if (track) bb |= eTrackBitmask;
	int record_len = eRecordHeaderSize + sizeof(u32) + str.length();

	buff.setU32(record_len);
	buff.setU32(ePoison);
	buff.setU32(transid);
	buff.setInt16(event_id);
	buff.setChar(bb);
	buff.setOmnStr(str);
	buff.setU32(mNextTransId);

	return mAppendFile->append(buff.data(), buff.dataLen(), mFlushFlag);
}


int64_t
SobjTransFile::appendRecord(
		const u32 transid,
		const int event_id,
		const bool mark,
		const bool track,
		const AosRundataPtr &rdata)
{
	// This function appends a record to the file.
	// Record format is:
	// 		Length			four bytes
	// 		poison			four bytes
	// 		transid			four bytes
	// 		event_id		two bytes 
	// 		mark and track	one byte
	// 		next transid	four bytes
	aos_assert_rr(mIsGood, rdata, -1);
	aos_assert_rr(mAppendFile, rdata, -1);
	int size = eRecordHeaderSize;
	char mem[size];
	AosBuff buff(mem, size, size, 0 AosMemoryCheckerArgs);
	buff.reset();

	char bb = 0;
	if (mark) bb |= eMarkBitmask;
	if (track) bb |= eTrackBitmask;
	int record_len = eRecordHeaderSize + sizeof(u32);

	buff.setU32(record_len);
	buff.setU32(ePoison);
	buff.setU32(transid);
	buff.setInt16(event_id);
	buff.setChar(bb);
	buff.setU32(mNextTransId);

	return mAppendFile->append(buff.data(), buff.dataLen(), mFlushFlag);
}


bool
SobjTransFile::readOneRecord(
		u32 &pos, 
		u32 &transid, 
		int &event_id, 
		bool &mark, 
		bool &track, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// All records are in the following format:
	// 		Length			four bytes
	// 		poison			four bytes
	// 		transid			four bytes
	// 		event_id		two bytes 
	// 		mark and track	one byte
	// 		contents		variable (optional)	
	// 		next transid	four bytes
	// This function does not read the cotents.
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mReadFile, rdata, false);

	int size = eRecordHeaderSize;
	char mem[size];
	int rslt = mReadFile->readToBuff(pos, size, mem);
	aos_assert_rr(rslt >= 0, rdata, false);
	if (rslt < size)
	{
		finished = true;
		return true;
	}

	aos_assert_rr(rslt == size, rdata, false);
	AosBuff buff(mem, size, size, 0 AosMemoryCheckerArgs);

	u32 len 		= buff.getU32(0);
	u32 poison 		= buff.getU32(0);
	transid 		= buff.getU32(0);
	event_id 		= buff.getI16(0);
	char bb 		= buff.getChar(0);

	mark = bb & eMarkBitmask;
	track = bb & eTrackBitmask;
	aos_assert_r(poison == ePoison, false);
	pos += len;
	return true;
}


bool
SobjTransFile::readFromFile(const AosRundataPtr &rdata)
{
	// This function is called when the file is first opened. It checks
	// whether the file is dirty. If yes, it rebuilds the control info.
	// 		File Header
	// 		Transaction Position Array:		Size = (u32 * mMaxTrans)
	// 		Response Position Arrays:		Size = ((u32 * mMaxTrans) + sizeof(int)) * N
	// 		Bitmaps:						Size = ((mMaxTrans/8) + sizeof(int)) * M
	//		Transactions and Responses
	//
	// where 'N' is the number of positions to record, and 'M' is the number of
	// bitmaps.
	aos_assert_rr(mIsGood, rdata, false);

	mLock->lock();
	bool rslt = openFileLocked(rdata);
	aos_assert_r(rslt, false);

	// 1. Read file header
	rslt = readHeaderLocked(rdata);
	aos_assert_rl(rslt, mLock, false);

	// 2. Read Transaction Position Array
	u32 pos;
	rslt = readTransPosLocked(pos, rdata);
	aos_assert_rl(rslt, mLock, false);

	// 3. Read Response Position Arrays
	rslt = readRespPositionsLocked(pos, rdata);
	aos_assert_rl(rslt, mLock, false);

	// 4. Read Bitmaps.
	rslt = readBitmapsLocked(pos, rdata);
	aos_assert_rl(rslt, mLock, false);

	// 5. Rebuild Control Info as needed
	rslt = rebuildControlInfoLocked(rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
SobjTransFile::readHeaderLocked(const AosRundataPtr &rdata)
{
	// File Header Format:
	// 		File Seqno					four bytes
	// 		Start Transid				four bytes
	// 		Maximum Number of Trans 	four bytes
	// 		Max File Size				four bytes
	// 		Poison						four bytes
	// 		Last Save Position			four bytes
	// 		Last Modification Time		four bytes
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mReadFile, rdata, false);

	char mem[eFileHeaderSize];
	int nn = mReadFile->readToBuff(eFileHeaderOffset, eFileHeaderSize, mem);
	aos_assert_rr(nn == eFileHeaderSize, rdata, false);
	AosBuff buff(mem, eFileHeaderSize, eFileHeaderSize, 0 AosMemoryCheckerArgs);

	mSeqno				= buff.getU32(0);
	mStartTransid 		= buff.getU32(0);
	mMaxTrans 			= buff.getU32(0);
	mFileMaxSize 		= buff.getInt(0);
	u32 poison 			= buff.getU32(0);
	mLastSavedPos		= buff.getU32(0);
	mLastModTime 		= buff.getU32(0);

	aos_assert_rr(mStartTransid, rdata, false);
	aos_assert_rr(mMaxTrans, rdata, false);
	aos_assert_rr(poison == ePoison, rdata, false);
	aos_assert_rr(mLastSavedPos, rdata, false);
	aos_assert_rr(mLastModTime, rdata, false);

	return true;
}


bool
SobjTransFile::readTransPosLocked(
		u32 &pos,
		const AosRundataPtr &rdata)
{
	// This function reads in the transaction position array. 
	// The start position is eTransPosOffset. Upon success, it sets 
	// 'pos' to the position right after the last byte this 
	// function consumed.
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);
	aos_assert_rr(mReadFile, rdata, false);
	if (!mTransPos)
	{
		try
		{
			mTransPos = OmnNew u32[mMaxTrans];
		}
		
		catch (...)
		{
			OmnAlarm << "failed allocating memory!" << enderr;
			return false;
		}
	}

	u32 size = mMaxTrans * sizeof(u32);
	char *buff = (char*)mTransPos;
	int num_read = mReadFile->readToBuff(eTransPosOffset, size, buff);
	aos_assert_rr(num_read >= 0 && (u32)num_read == size, rdata, false);
	pos += size;
	return true;
}


bool
SobjTransFile::readRespPositionsLocked(
		u32 &pos, 
		const AosRundataPtr &rdata)
{
	// This function reads in all the response position arrays.
	// 'pos' points to the beginning of the section. Its format
	// is:
	// 		num_positions		four bytes
	// 		event_id			four bytes
	// 		contents			mMaxTrans*sizeof(u32)	
	// 		event_id			four bytes
	// 		contents			mMaxTrans*sizeof(u32)	
	// 		...
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_r(mMaxTrans > 0, false);
	u32 size = mMaxTrans * sizeof(u32);
	int num_positions = mReadFile->readInt(pos, -1);
	pos += sizeof(u32);
	aos_assert_rr(num_positions <= mMaxPositions, rdata, false);
	for (int i=0; i<num_positions; i++)
	{
		int event_id = mReadFile->readInt(pos, -1);
		pos += sizeof(int);
		aos_assert_rr(event_id >= 0 && event_id < eMaxEvents, rdata, false);
		aos_assert_rr(!mRespPos[event_id], rdata, false);
		mRespPos[event_id] = OmnNew u32[mMaxTrans];
		char *buff = (char*)mRespPos[event_id];
		int num_read = mReadFile->readToBuff(pos, size, buff);
		aos_assert_rr(num_read >= 0 && (u32)num_read == size, rdata, false);
		pos += size;
	}
	return true;
}


bool
SobjTransFile::readBitmapsLocked(u32 &pos, const AosRundataPtr &rdata)
{
	// This function reads in all the bitmaps.
	// 'pos' points to the beginning of the section. Its format
	// is:
	// 		num_bitmaps		four bytes
	// 		event_id		four bytes
	// 		contents		mMaxTrans/8	
	// 		event_id		four bytes
	// 		contents		mMaxTrans/8	
	// 		...
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_r(mMaxTrans > 0, false);
	u32 size = mMaxTrans/8;
	u32 num_entries = size/sizeof(u32);
	int num_bitmaps = mReadFile->readInt(pos, -1);
	pos += sizeof(u32);
	aos_assert_rr(num_bitmaps <= mMaxBitmaps, rdata, false);
	for (int i=0; i<num_bitmaps; i++)
	{
		int event_id = mReadFile->readInt(pos, -1);
		pos += sizeof(u32);
		aos_assert_rr(event_id >= 0 && event_id < eMaxEvents, rdata, false);
		aos_assert_rr(!mBitmaps[event_id], rdata, false);
		mBitmaps[event_id] = OmnNew u32[num_entries];
		char *buff = (char*)mRespPos[event_id];
		int num_read = mReadFile->readToBuff(pos, size, buff);
		aos_assert_rr(num_read >= 0 && (u32)num_read == size, rdata, false);
		pos += size;
	}
	return true;
}


bool
SobjTransFile::rebuildControlInfoLocked(const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mLastSavedPos, rdata, false);
	bool finished = false;
	u32 pos = mLastSavedPos;
	u32 transid;
	while (!finished)
	{
		u32 crt_pos = pos;
		bool mark, track;
		int event_id;
		bool rslt = readOneRecord(pos, transid, event_id, mark, track, finished, rdata);
		aos_assert_rr(rslt, rdata, false);
		if (finished) break;
		if (mark)
		{
			bool rslt = markBitmapLocked(event_id, transid, 0, 0, rdata);
			aos_assert_rr(rslt, rdata, false);
		}

		if (track)
		{
			bool rslt = updateRespPosLocked(event_id, transid, crt_pos, rdata);
			aos_assert_rr(rslt, rdata, false);
		}
	}

	return saveControlInfoLocked(rdata);
}


bool
SobjTransFile::saveControlInfoLocked(const AosRundataPtr &rdata)
{
	// This function saves the control info into the file.
	u32 pos;
	saveTransPosLocked(pos, rdata);
	saveRespPosLocked(pos, rdata);
	saveBitmapsLocked(pos, rdata);
	saveHeaderLocked(rdata);
	return true;
}


bool
SobjTransFile::saveHeaderLocked(const AosRundataPtr &rdata)
{
	// Header Format:
	// 		Seqno					four bytes
	// 		Start Transid			four bytes
	// 		Maximum Trans			four bytes
	// 		Poison					four bytes
	// 		Last Saved Position		four bytes
	// 		Last Modification Time	four bytes
	AosBuff buff(eFileHeaderSize, 0 AosMemoryCheckerArgs);
	aos_assert_r(mReadFile, false);
	aos_assert_r(mAppendFile, false);

	buff.setU32(mSeqno);
	buff.setU32(mStartTransid);
	buff.setU32(mMaxTrans);
	buff.setInt(mFileMaxSize);
	buff.setU32(ePoison);
	buff.setU32(mAppendFile->getLength());
	buff.setU32(OmnGetSecond());

	bool rslt = mReadFile->put(eFileHeaderOffset, buff.data(), 
			eFileHeaderSize, true);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
SobjTransFile::saveBitmapsLocked(u32 &pos, const AosRundataPtr &rdata)
{
	int num_bitmaps = 0;
	for (int i=0; i<eMaxEvents; i++)
	{
		if (mBitmaps[i]) num_bitmaps++;
	}

	aos_assert_rr(mReadFile, rdata, false);
	mReadFile->setInt(pos, num_bitmaps, false);
	u32 size = getBitmapNumBytes();
	pos += sizeof(u32);
	int nn = 0;
	for (int i=0; i<eMaxEvents; i++)
	{
		if (mBitmaps[i])
		{
			nn++;
			bool rslt = mReadFile->setInt(pos, i, false);
			aos_assert_rr(rslt, rdata, false);
			pos += sizeof(u32);
			char *buff = (char *)mBitmaps[i];
			rslt = mReadFile->put(pos, buff, size, false);
			aos_assert_rr(rslt, rdata, false);
			aos_assert_rr(nn <= num_bitmaps, rdata, false);
			pos += size;
		}
	}
	return true;
}


bool
SobjTransFile::saveRespPosLocked(u32 &pos, const AosRundataPtr &rdata)
{
	int num_pos = 0;
	for (int i=0; i<eMaxEvents; i++)
	{
		if (mRespPos[i]) num_pos++;
	}

	aos_assert_rr(mReadFile, rdata, false);
	mReadFile->setInt(pos, num_pos, false);
	u32 size = mMaxTrans * sizeof(u32);
	pos += sizeof(u32);
	int nn = 0;
	for (int i=0; i<eMaxEvents; i++)
	{
		if (mRespPos[i])
		{
			nn++;
			bool rslt = mReadFile->setInt(pos, i, false);
			aos_assert_rr(rslt, rdata, false);
			pos += sizeof(u32);
			char *buff = (char*)mRespPos[i];
			rslt = mReadFile->put(pos, buff, size, false);
			aos_assert_rr(rslt, rdata, false);
			aos_assert_rr(nn <= num_pos, rdata, false);
			pos += size;
		}
	}
	return true;
}


bool
SobjTransFile::saveTransPosLocked(u32 &pos, const AosRundataPtr &rdata)
{
	aos_assert_rr(mReadFile, rdata, false);
	aos_assert_rr(mTransPos, rdata, false);
	aos_assert_rr(mMaxTrans > 0, rdata, false);
	char *buff = (char*)mTransPos;
	u32 size = mMaxTrans * sizeof(u32);
	bool rslt = mReadFile->put(eTransPosOffset, buff, size, false);
	aos_assert_rr(rslt, rdata, false);
	pos = eTransPosOffset + size;
	return true;
}


bool
SobjTransFile::openFileLocked(const AosRundataPtr &rdata)
{
	aos_assert_r(mFname != "", false);
	if (!mAppendFile)
	{
		mAppendFile = OmnNew AosAppendFile(mFname);
		aos_assert_r(mAppendFile && mAppendFile->isGood(), false);
		mAppendFile->gotoEnd();
	}

	if (!mReadFile)
	{
		mReadFile = OmnNew OmnFile(mFname, OmnFile::eWriteCreate);
		aos_assert_r(mReadFile && mReadFile->isGood(), false);
	}
	return true;	
}


u32
SobjTransFile::getCrtTransid(const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, 0);

	mLock->lock();
	aos_assert_rl(mReadFile, mLock, 0);
	int64_t length = mReadFile->getLength();
	aos_assert_rl(length > sizeof(u32), mLock, 0);
	u32 transid = mReadFile->readBinaryU32(length-sizeof(u32), 0);
	mLock->unlock();
	aos_assert_rr(transid, rdata, 0);
	return transid;
}


bool
SobjTransFile::closeFile(const AosRundataPtr &rdata)
{
	aos_assert_rr(mIsGood, rdata, 0);
	if (!mReadFile && !mAppendFile)
	{
		return true;
	}

	mLock->lock();
	bool rslt = saveControlInfoLocked(rdata);
	mLock->unlock();
	return rslt;
}


bool
SobjTransFile::isFileTooBig()
{
	aos_assert_r(mAppendFile, false);
	int64_t fsize = mAppendFile->getLength();
	return (fsize > mFileMaxSize);
}

