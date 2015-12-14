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
// 	Created: 05/28/2013 by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_RawFilesMgr_h
#define AOS_DocFileMgr_RawFilesMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "DocFileMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <map>
using namespace std;

OmnDefineSingletonClass(AosRawFilesMgrSingleton,
						AosRawFilesMgr,
						AosRawFilesMgrSelf,
						OmnSingletonObjId::eRawFilesMgr,
						"RawFilesMgr");

class AosRawFilesMgr : public OmnRCObject  
{
	OmnDefineRCObject;

private:
	
	OmnMutexPtr						mLock;
	map<u16, AosRawFilesPtr>		mFiles;
	u16								mRawFilesSeqno;
	int								mLoopIndex;

	AosRawFilesMgr();
	~AosRawFilesMgr();

public:
    // Singleton class interface
    static AosRawFilesMgr* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);


	bool saveFile(const AosXmlTagPtr &doc, const AosBuffPtr &buff, const AosRundataPtr &rdata);
	AosBuffPtr getFile(const AosXmlTagPtr &doc, const u64 &id, const AosRundataPtr &rdata);

private:
	bool initRawFiles();
};
#endif

