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
#ifndef AOS_IILUtil_IILDfmDocs_h
#define AOS_IILUtil_IILDfmDocs_h

#include "alarm_c/alarm.h"
#include "DfmUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"

struct AosIILDfmDocs : virtual public OmnRCObject 
{
	OmnDefineRCObject;


	typedef map<u64, u64> map_t;
	typedef map<u64, u64> ::iterator mapitr_t;

	enum
	{
		eDftBuffSize = 500000 //500K
	};

private:
	bool					mShowLog;
	OmnMutexPtr				mLock;
	AosBuffPtr				mBuff;
	map_t					mIndex;
	AosDfmDocPtr			mTempDoc;
	u64						mRootIILId;
int	mNum;

public:
	AosIILDfmDocs(const u64 &root_iilid, const bool show_log);

	~AosIILDfmDocs();

	bool addEntry(
			const AosRundataPtr &rdata,
			const AosDfmDocPtr &doc,
			vector<AosTransId> &trans_ids);

	bool commit(
			const AosRundataPtr &rdata, 
			const AosDocFileMgrObjPtr &doc_mgr);

	AosDfmDocPtr readDoc(
			const AosRundataPtr &rdata,
			const u64 &docid);

	u64 getRootIILID() {return mRootIILId;}
};
#endif
