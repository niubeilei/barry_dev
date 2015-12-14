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
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocidMgrNew_h
#define AOS_DocClient_DocidMgrNew_h

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


class AosDocidMgrNew : virtual public OmnRCObject
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
	u64				mDefaultDocids;
	int				mNumDefaultDocids;
	vector<u64>		mCrtDocids;
	vector<int>		mNumDocids;
	u64				mInitDocid;
	int				mNumVirtuals;
	int				mDocidIncValue;
	bool			mNeedVerify;

public:
	AosDocidMgrNew();
	~AosDocidMgrNew();

    bool   	start();
    bool    stop();
    bool	config(const AosXmlTagPtr &def);

	u64		nextDocid(OmnString &objid, const AosRundataPtr &rdata);
	void 	returnDocid(const u64 &docid, const AosRundataPtr &rdata);

	inline int convertObjidToVirtualId(const OmnString &objid)
	{
		aos_assert_r(mNumVirtuals > 0, -1);
		u64 vv = AosStrHashFunc(objid.data(), objid.length());
		return vv % mNumVirtuals;
	}

private:
	OmnString createDefaultObjidLocked(const AosRundataPtr &rdata);
	bool 	retrieveDftDocidsLocked(const AosRundataPtr &rdata);
	u64		nextDocidLocked(const int vid, const AosRundataPtr &rdata);
	bool	retrieveDocidsLocked(const int vid, const AosRundataPtr &rdata);
	bool	retrieveDocidsFromServerSync(
						const u32 cubicid, 
						const int vir_id,
						const AosRundataPtr &rdata);
};
#endif
