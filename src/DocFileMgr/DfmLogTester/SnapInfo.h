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
	u32			mSnapId;
	map<u64, AosDocInfoPtr> mDocInfo;

public:
	AosSnapInfo(const u32 snap_id, map<u64, AosDocInfoPtr> &doc_infos);
	AosSnapInfo();
	~AosSnapInfo();
	
	u32		getSnapId(){ return mSnapId; };
	void	removeDocInfo();
	u32		getDocNum(){ return mDocInfo.size(); };
	map<u64, AosDocInfoPtr>  getDocInfo(){ return mDocInfo;};
	
	bool 	serializeFrom(const AosBuffPtr &buff);
	bool 	serializeTo(const AosBuffPtr &buff);

};

#endif
