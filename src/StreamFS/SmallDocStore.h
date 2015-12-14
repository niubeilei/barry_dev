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
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamFS_SmallDocStore_h
#define AOS_StreamFS_SmallDocStore_h

#include "SEInterfaces/SmallDocStoreObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "StreamFS/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "Util/TransId.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

#include <map>



OmnDefineSingletonClass(AosSmallDocStoreSingleton,
						AosSmallDocStore,
						AosSmallDocStoreSelf,
						OmnSingletonObjId::eSmallDocStore,
						"SmallDocStore");

class AosSmallDocStore : public AosSmallDocStoreObj
{
	OmnDefineRCObject;
	
private:
	OmnMutexPtr							mLock;
	OmnCondVarPtr						mCondVar;
	map<u64, AosSmallDocContainerPtr> 	mCntrMap;

public:
	AosSmallDocStore();
	virtual ~AosSmallDocStore();

    // Singleton class interface
    static AosSmallDocStore* 	getSelf();
    virtual bool      			start() {return true;};
    virtual bool    		    stop() {return true;};
    virtual bool				config(const AosXmlTagPtr &def) {return true;}

public:
	virtual bool retrieveDoc(
					const AosRundataPtr &rdata, 
					const u64 docid,
					AosDataRecordObjPtr &doc);

	AosBuffPtr   retrieveDoc(
					const AosRundataPtr &rdata, 
					const u64 &docid, 
					const u32 &entryNums);

	virtual bool addDocs(
					const u64 group_id, 
					const u32 entryNums,
					const AosBuffPtr &contents,
					const AosTransId &trans_id,
					const AosRundataPtr &rdata);

	virtual bool ageOutDocs(
					const AosRundataPtr &rdata,
					const u64 group_id,
					const AosTransId &trans_id);

	u64 		getRealDocid(
					const u64 smallDocid, 
					const u32 entryNums);

private:
	AosSmallDocContainerPtr getContainer(
					const AosRundataPtr &rdata, 
					const u64 group_id);

};
#endif
