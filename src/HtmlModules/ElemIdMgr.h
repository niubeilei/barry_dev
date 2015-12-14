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
// 07/19/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_HtmlModules_ElemIdMgr_h
#define AOS_HtmlModules_ElemIdMgr_h

#include "Thread/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

OmnDefineSingletonClass(AosElemIdMgrSingleton,
	AosElemIdMgr,
	AosElemIdMgrSelf,
	OmnSingletonObjId::eElemIdMgr,
	"ElemIdMgr");


class AosElemIdMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBatchSize = 10,
		eIsRetrieving = 1,
		eNotDefined = 2
	};

	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	u64				mCrtId;
	int				mBatchSize;
	int				mNumIdRemaining;
	OmnString		mDirname;
	OmnString		mFilename;

public:
	AosElemIdMgr();
	~AosElemIdMgr();

	// Singleton class interface
	static AosElemIdMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);

	virtual OmnString   getSysObjName() const {return "AosElemIdMgr";}
	virtual OmnRslt     config(const OmnXmlParserPtr &def);
	virtual OmnSingletonObjId::E getSysObjId() const
			{
				return OmnSingletonObjId::eElemIdMgr;
			}

	OmnString	getNewElemId();
	OmnString	getNewElemId(const u32 siteid);

private:
};

#endif
