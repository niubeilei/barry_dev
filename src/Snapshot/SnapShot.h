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
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Snapshot_SnapShot_h
#define AOS_Snapshot_SnapShot_h

#include "DocFileMgr/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/TransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "Util/Ptrs.h"

class AosSnapShot : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eNorm = 0,
		eCommit,
		eRollback,
		eRemoveSnap,
		eAddSnap
	};

private:
	bool				mShowLog;	

public:
	AosSnapShot();
	~AosSnapShot();

	virtual bool addEntry(
			const AosRundataPtr &rdata,
			const AosDfmDocPtr &doc,
			vector<AosTransId> &trans_ids) = 0;

	virtual AosDfmDocPtr readDoc(
			const AosRundataPtr &rdata,
			const u64 &docid) = 0;

	virtual bool rollBack(const AosRundataPtr &rdata) = 0;

	virtual bool removeSnapShot(const AosRundataPtr &rdata) = 0;

	virtual bool commit(const AosRundataPtr &rdata, const AosDocFileMgrObjPtr &docFileMgr) = 0;

	virtual Status getStatus() = 0;

	virtual bool clearData() = 0;
	
	virtual bool recoverCommit(const u32 siteid) = 0;

	virtual bool rollBackFinishTransId(const AosRundataPtr &rdata) = 0;

	virtual bool recoverRollBack(const u32 siteid) = 0;
};
#endif
