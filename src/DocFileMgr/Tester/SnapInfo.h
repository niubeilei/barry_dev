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
// Modification History:
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocFileMgr_Tester_SnapInfo_h
#define Aos_DocFileMgr_Tester_SnapInfo_h 

#include "Thread/Ptrs.h"
#include "DocFileMgr/Tester/Ptrs.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include "Thread/Mutex.h"

#include <set>
#include <vector>
using namespace std;


class AosSnapInfo : public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr	mLock;
	AosDfmInfoPtr mDfmInfo;
	u32			mSnapId;
	map<u64, AosDocInfoPtr> mDocs;
	set<u64>	mDeletedDocs;

public:
	AosSnapInfo(const u32 snap_id, const AosDfmInfoPtr &dfm);
	AosSnapInfo();
	~AosSnapInfo();
	
	u32		getSnapId(){ return mSnapId; };
	u32		getDocNum(){ return mDocs.size(); };

	map<u64, AosDocInfoPtr> & getDocs(){ return mDocs; };
	set<u64> & getDeletedDocs(){ return mDeletedDocs; };

	AosDocInfoPtr randReadDoc();
	AosDocInfoPtr addDoc(const u64 docid);
	AosDocInfoPtr randModifyDoc();
	AosDocInfoPtr randDeleteDoc();
	bool 	merge(const AosSnapInfoPtr &snap);

	bool 	serializeFrom(const AosBuffPtr &buff);
	bool 	serializeTo(const AosBuffPtr &buff);

};

#endif
