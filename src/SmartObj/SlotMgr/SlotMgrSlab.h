////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_SlotMgr_Slab
#define Aos_SmartObj_SlotMgr_Slab

#include "Rundata/Ptrs.h"
#include "SmartObj/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosSlotMgrSlab : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxSplit = 100000
	};

	AosSobjSlotMgrPtr	mMgr;
	int64_t 		mMinSize;
	int64_t 		mMaxSize;
	int				mNextTries;
	int				mIndex;
	vector<int64_t>	mStarts;
	vector<int64_t>	mSizes;

public:
	AosSlotMgrSlab(
			const int idx,
			const AosSobjSlotMgrPtr &mgr, 
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
	~AosSlotMgrSlab();

	bool addSlot(const int64_t &start, 
					const int64_t &size, 
					const AosXmlTagPtr &parms, 
					const AosRundataPtr &rdata);
	int64_t	getMinSize() const {return mMinSize;}
	int64_t	getMaxSize() const {return mMaxSize;}
	bool getSlot(int64_t &start, 
					int64_t &size, 
					const AosXmlTagPtr &parms, 
					const AosRundataPtr &rdata);
	int getNextTries() const {return mNextTries;}
	int getIndex() const {return mIndex;}
	bool serializeTo(OmnString &contents, const AosRundataPtr &rdata);

private:
	bool checkSize(const int64_t &size, const AosRundataPtr &rdata);
	bool init(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif

