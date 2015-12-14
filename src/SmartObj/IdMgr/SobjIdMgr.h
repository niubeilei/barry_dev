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
// 03/16/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocidMgr_SobjIdMgr_h
#define AOS_DocidMgr_SobjIdMgr_h

#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/UtUtil.h"
#include <vector>
using namespace std;


class AosSobjIdMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftInitDocid = 1000000,
		eDftDocidIncValue = 1,
		eMaxVirtual = 1000000
	};

private:
	OmnMutexPtr		mLock;
	OmnString		mName;
	vector<u64>		mCrtIds;
	vector<int>		mNumIds;
	u64				mInitId;
	int				mNumVirtuals;
	int				mIdIncValue;

public:
	AosSobjIdMgr();
	~AosSobjIdMgr();

    bool   	start();
    bool    stop();
    bool	config(const AosXmlTagPtr &def);

	u64		nextId(const int vid, const AosRundataPtr &rdata);
	void 	returnId(const u64 &id, const AosRundataPtr &rdata);

private:
	bool	retrieveIdsLocked(const int vid, const AosRundataPtr &rdata);
};
#endif
