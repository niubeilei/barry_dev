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
// 2015/04/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_Tester_LocalSnapshot_h
#define Aos_BlobSE_Tester_LocalSnapshot_h 

#include "BlobSE/Tester/Ptrs.h"
#include "BlobSE/Tester/LocalDoc.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#include <map>
using namespace std;


class AosLocalSnapshot: public OmnRCObject
{
	OmnDefineRCObject;

private:
	//OmnMutexPtr	mLock;
	u32			mSnapId;
	u64			mMaxDocid;
	map<u64, AosLocalDoc *> mDocs;

public:
	AosLocalSnapshot(
			const u32 snap_id, 
			const u64 max_docid);
	~AosLocalSnapshot();
	
	u32		getSnapId(){ return mSnapId; };
	u32		getDocNum(){ return mDocs.size(); };

	map<u64, AosLocalDoc *> & getDocs(){ return mDocs; };
	void	cleanDocs(){ mDocs.clear(); };

	AosLocalDoc * addDoc(const u64 docid);
	AosLocalDoc * randModifyDoc();
	AosLocalDoc * randDeleteDoc();

	u64 	randPickDocid();
	AosLocalDoc * readDoc(const u64 docid);

	bool 	merge(const AosLocalSnapshotPtr &snap);

};

#endif
