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
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtilServer_IdGenFileMgr_h
#define AOS_SEUtilServer_IdGenFileMgr_h

#include "Thread/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"

OmnDefineSingletonClass(AosIdGenFileMgrSingleton,
	AosIdGenFileMgr,
	AosIdGenFileMgrSelf,
	OmnSingletonObjId::eIdGenFileMgr,
	"IdGenFileMgr");



class AosIdGenFileMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{         
		eDftStartDocid = 5000,
		eDftStartIILId = 1000,
		eDftStartImgId = 1000,
		eDftDocidBlocksize = 10,
		eDftIILIdBlocksize = 10,
		eDftImgIdBlocksize = 10,

		eRecordStart =10000,
		eRecordSize = 100000000,

		eRecordBodyStart =32,
		eMaxDocSize = 100000
	};

	OmnMutexPtr		mLock;
	OmnFilePtr 		mFile;
	OmnString 		mFilePath;
	const static OmnString sgFileName;

public:
	AosIdGenFileMgr();
	~AosIdGenFileMgr();

	// Singleton class interface
	static AosIdGenFileMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	bool writeRecord(const int id, const OmnString &date, bool needLock);
	AosXmlTagPtr readRecord(const int id);
private:
};

#endif
