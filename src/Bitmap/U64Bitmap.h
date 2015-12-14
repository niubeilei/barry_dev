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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Bitmap_U64Bitmap_h
#define Aos_Bitmap_U64Bitmap_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

class AosSimpleMutex;
class AosU64BmNodeRoot;

class AosU64Bitmap : public OmnRCObject
{
private:
	AosSimpleMutex *	mLock;
	AosU64BmNodeRoot*	mRootNode;
	vector<u8>			mIndex;

public:
	AosU64Bitmap();
	~AosU64Bitmap();
	
	bool setDocid(const u64 &docid);
	void appendDocid(const u64 docid);
	bool checkDoc(const u64 docid)const;
	void removeDocid(const u64 docid);
	void saveToFile(const AosRundataPtr &rdata);
	bool loadFromFile(const AosRundataPtr &rdata);
};
#endif

