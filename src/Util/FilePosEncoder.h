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
#ifndef Aos_Util_FilePosEncoder_h
#define Aos_Util_FilePosEncoder_h

#include "Thread/Mutex.h"
#include "Util/String.h"
#include <vector>
#include <map>

class AosFilePosEncoder
{
private:
	static OmnMutexPtr			mLock;
	static u32					mCrtCode;
	static map<OmnString, u32>	mMap;
	static vector<OmnString>	mFilenames;
	static vector<int>			mLine;

public:
	AosFilePosEncoder();

	static u32 getCode(const OmnString &file, const int line);
	static bool decodePos(const u32 code, OmnString &file, int &line);
};

#define AosFilePosCode AosFilePosEncoder::getCode(__FILE__, __LINE__)
#define AosFileAndLine __FILE__, __LINE__

#endif

