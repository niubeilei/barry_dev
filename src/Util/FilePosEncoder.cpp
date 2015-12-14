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
//
// Modification History:
// 08/22/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/FilePosEncoder.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"


const u32 sgInitCode = 100;

OmnMutexPtr			AosFilePosEncoder::mLock = OmnNew OmnMutex();
u32					AosFilePosEncoder::mCrtCode = sgInitCode;
map<OmnString, u32>	AosFilePosEncoder::mMap;
vector<OmnString>	AosFilePosEncoder::mFilenames;
vector<int>			AosFilePosEncoder::mLine;
AosFilePosEncoder   sgFilePosEncoder;

AosFilePosEncoder::AosFilePosEncoder()
{
	if (sgInitCode != 0)
	{
		for (u32 i=0; i<sgInitCode; i++)
		{
			mFilenames.push_back("");
			mLine.push_back(-1);
		}
	}
}


u32 
AosFilePosEncoder::getCode(const OmnString &file, const int line)
{
	// It creates a new string:
	// 	file + "_" + line
	// It then checks whether it is already in the map. If not, it creates
	// the entry. Otherwise, it returns the code.
	OmnString key = file;
	key << "_" << line;

	mLock->lock();
	map<OmnString, u32>::iterator itr = mMap.find(key);
	if (itr != mMap.end())
	{
		// Found it.
		u32 code = itr->second;
		mLock->unlock();
		return code;
	}

	// Did not find it. Create one.
	u32 code = mCrtCode++;
	mMap[key] = code;
	mFilenames.push_back(file);
	mLine.push_back(line);
	mLock->unlock();
	return code;
}


bool 
AosFilePosEncoder::decodePos(const u32 code, OmnString &file, int &line)
{
	aos_assert_r(code < mCrtCode, false);

	mLock->lock();
	aos_assert_rl(code < mFilenames.size(), mLock, false);
	aos_assert_rl(code < mLine.size(), mLock, false);
	file = mFilenames[code];
	line = mLine[code];
	mLock->unlock();
	return true;
}

