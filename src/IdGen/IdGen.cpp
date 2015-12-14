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
//	This class generates a string ID. It has an array of:
//		[min, max]
//		[min, max]
//		...
//	Each defines the allowed ASCII values for the string ID. 
//	When generating the next ID, it increments the last byte. 
//	If the last byte is inside the above defined ranges, return.
//	Otherwise, it moves to the next range, and return. 
//	If no more ranges, the next byte repeats the above process. 
//
//  As an example, if the current ID is:
//  		"4a64"
//  the next id should be "4a65", "4a66", ..., "4a6A", ..., "4a6Z", 
//  	"4a6a", ..., "4a6z", "4a70", ...
//  	
//  IDs are normally no longer than 8 bytes. These IDs can be converted
//  into u64, which is converted as below:
//  	u64					string
//  	Lowerest byte:		the last byte 
//  	...					...
//
//	This class is used to generate DocID. 
//
// Modification History:
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IdGen/IdGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "IdGen/IdGenDef.h"
#include "Util/StrSplit.h"
#include "Util/File.h"
#include "SiteMgr/SyncServer.h"
#include "XmlUtil/XmlTag.h"

extern AosXmlTagPtr gAosAppConfig;
const OmnString sgFileFlag = "idgen_dcv163356adf8dgod5623dc5";


AosIdGen::AosIdGen(const OmnString &name)
:
mNumRanges(0),
mBase(0),
mBlocksize(0)
{
	start(name);
}


AosIdGen::~AosIdGen()
{
}


bool
AosIdGen::start(const OmnString &name)
{
	// It retrieves the definition from the database and
	// initializes itself.
	mDef = AosIdGenMgrSelf->getIdDef(name); 
	aos_assert_r(mDef, false);

	mBlocksize = mDef->getBlockSize();
	mMaxLen = mDef->getMaxLen();

	//	min,max,min,max,...
	OmnString def = mDef->getRanges();
	aos_assert_r(def != "", false);
	const int max_ranges = 20;
	OmnString parts[max_ranges];
	bool finished;
	AosStrSplit split(def.data(), ",", parts, max_ranges, finished);
	mBase = 0;
	for (int i=0; i<split.entries()-1; i+=2)
	{
		if (mNumRanges >= eMaxRanges)
		{
			OmnAlarm << "Too many ranges: " << mNumRanges
				<< ". Max allowed: " << eMaxRanges << enderr;
			return false;
		}

		mRangeMin[mNumRanges] = atoi(parts[i].data());
		mRangeMax[mNumRanges] = atoi(parts[i+1].data());
		aos_assert_r(mRangeMin[mNumRanges] <= mRangeMax[mNumRanges], false);
		mBase += mRangeMax[mNumRanges] - mRangeMin[mNumRanges] + 1;
		mNumRanges++;
	}

	// Set mCrtRangeIdx
	char *data = (char *)mCrtId.data();
	for (int i=mCrtId.length()-1; i>=0; i--)
	{
		u8 c = data[i];
		bool found = false;
		for (int kk=0; kk<mNumRanges; kk++)
		{
			if (c >= mRangeMin[kk] && c <= mRangeMax[kk])
			{
				mCrtRangeIdx[i] = kk;
				found = true;
				break;
			}
		}

		if (!found)
		{
			OmnAlarm << "Current char not in any range: "
				<< i << ". Id: " << mCrtId << enderr;
			return false;
		}
	}

	if (mBlocksize == 0) aos_assert_r(loadBlock(), false);
	return true;
}


bool
AosIdGen::loadBlock()
{
	// If mBlocksize != -1, it is a blocked IdGen, which means
	// that it should get IDs from an ID Server. Dataids are 
	// retrieved in blocks. It always keep an extra block. When 
	// it used up the current block, it will use the extra one, 
	// and will request a new block from the ID server. The block
	// size should be big enough for this site to run for at least
	// a few days or longer without having to sync with the server. 
	// The block size is set to mLoadBlocksize. 
	
	// Currently not implemented yet. It simply allocate another
	// block locally
	mBlocksize += eDftBlocksize;
	return true;
}


OmnString
AosIdGen::getNextId()
{
	OmnString id;
	if (getNextId(id)) return id;
	return "";
}


OmnString
AosIdGen::getNextIdBlock(const int num)
{
	aos_assert_r(num > 0, "");
	if (num == 1) return getNextId();

	OmnString startid, ss;
	aos_assert_r(getNextId(startid, false), "");
	for (int i=1; i<num; i++) aos_assert_r(getNextId(ss, false), "");
	mDef->updateId(mCrtId);
	return startid;
}


bool
AosIdGen::getNextId(OmnString &id, const bool flag)
{
	// The caller should use lock to make sure it is thread
	// safe. This class is not thread safe.
	unsigned char c;
	char *data = (char *)mCrtId.data();
	for (int i=mCrtId.length()-1; i>=0; i--)
	{
		c = ++data[i];
		int rangeIdx = mCrtRangeIdx[i];
		if (c <= mRangeMax[rangeIdx])	
		{
			id = mCrtId;
			if (flag) mDef->updateId(mCrtId);
			if (mBlocksize != -1) 
			{
				mBlocksize--;
				if (mBlocksize == 0) loadBlock();
			}
			return true;
		}

		// Try the next range, if any
		rangeIdx++;
		if (rangeIdx < mNumRanges)
		{
			mCrtRangeIdx[i]++;
			data[i] = mRangeMin[rangeIdx];
			id = mCrtId;
			if (flag) mDef->updateId(mCrtId);
			if (mBlocksize != -1) 
			{
				mBlocksize--;
				if (mBlocksize == 0) loadBlock();
			}
			return true;
		}

		// No more ranges. 
		// Check whether it is the last byte
		if (i == 0)
		{
			// It is the last byte. Check whether it can increase
			// the length.
			if (mCrtId.length() >= mMaxLen)
			{
				// It reached the maximum.
				return false;
			}

			// Need to increase the length;
			mCrtId.setLength(mCrtId.length() + 1);
			data = (char *)mCrtId.data();
			
			// Reset all to '0'
			c = mRangeMin[0];
			for (int kk=mCrtId.length()-1; kk >= 0; kk--)
			{
				data[kk] = c;
				mCrtRangeIdx[kk] = 0;
			}
			id = mCrtId;
			if (flag) mDef->updateId(mCrtId);
			if (mBlocksize != -1) 
			{
				mBlocksize--;
				if (mBlocksize == 0) loadBlock();
			}
			return true;
		}

		// Need to reset all the current ones
		// to the minimum of the first range, and advance
		c = mRangeMin[0];
		data[i] = c;
		mCrtRangeIdx[i] = 0;
	}

	OmnShouldNeverComeHere;
	OmnAlarm << "crtid: " << mCrtId << enderr;
	return false;
}


u64
AosIdGen::getNextU64(const u64 &dft)
{
	OmnString id;
	aos_assert_r(getNextId(id), dft);
	aos_assert_r(id.length() <= 8, dft);
	return strToU64(id, dft);
}


bool	
AosIdGen::isValidId(const OmnString &id)
{
	int len = id.length();
	if (len > mMaxLen) return false;

	// Check whether all its characters are in the ranges
	for (int i=0; i<len; i++)
	{
		char c = id.data()[i];
		for (int k=0; k<mNumRanges; k++)
		{
			if (c < mRangeMin[k] || c > mRangeMax[k]) return false;
		}
	}

	if (strcmp(id.data(), mCrtId.data()) <= 0) return true;
	return false;
}


u64 
AosIdGen::convertDocIdToU64(const u64 &docid, const u64 &dft)
{
	// This function converts the id into an integer. 
	// This is used by DocId. 
	// For the first byte:
	//	'0' - '9' 	convert to	0-9
	//	'A' - 'Z'	convert to  10 - 35
	//  'a'	- 'z'	convert to  36 - 61

	u64 value = 0;
	u32 vv;
	u64 factor = 1;
	u8 c;
	int len = 8;
	u64 did = docid;
	for (int i=len-1; i>=0; i--)
	{
		c = (u8)did;
		did >>= 8;
		if (c == 0) return value;

		if (c >= '0' && c <= '9') vv = c - '0';
		else if (c >= 'A' && c <= 'Z') vv = 10 + c - 'A';
		else if (c >= 'a' && c <= 'z') vv = 36 + c - 'a';
		else 
		{
			OmnAlarm << "Docid contains incorrect char: " << (int)c << enderr;
			return dft;
		}
		value += factor * (vv+1);
		factor *= mBase;
	}

	return value;
}


bool
AosIdGen::setCrtId(const OmnString &id)
{
	mCrtId = id;
	return mDef->updateId(mCrtId);
}


bool
AosIdGen::syncServerCb(const AosSyncServerReqType, void *data)
{
	OmnNotImplementedYet;
	return false;
}


OmnString
AosIdGen::u64ToStr(const u64 &docid)
{
	// Starting from the lowest byte, convert the value into 
	// character:
	char buff[9];
	buff[8] = 0;
	u64 did = docid;
	for (int i=7; i>=0; i--)
	{
		u8 cc = (u8)did;
		if (cc == 0) 
		{
			aos_assert_r(i!=7, "");
			return OmnString(&buff[i+1]);
		}
		did >>= 8;
		buff[i] = (char)cc;
	}
	return OmnString(buff);
}


u64
AosIdGen::strToU64(const OmnString &id, const u64 dft)
{
	int len = id.length();
	if (len > 8) return dft;
	const char *data = id.data();
	u64 did = 0;
	for (int i=0; i<len; i++)
	{
		did <<= 8;
		did += (u8)data[i];
	}
	return did;	
}

