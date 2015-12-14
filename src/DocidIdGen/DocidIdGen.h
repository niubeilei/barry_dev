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
#ifndef AOS_DocidIdGen_DocidIdGen_h
#define AOS_DocidIdGen_DocidIdGen_h

#include "Debug/Debug.h"
#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"

OmnDefineSingletonClass(AosDocidIdGenSingleton,
	AosDocidIdGen,
	AosDocidIdGenSelf,
	OmnSingletonObjId::eDocidIdGen,
	"DocidIdGen");

#define AOSIDGEN_DOCID				"docid"

#define AOSCIDPREFIX 				""

class AosDocidIdGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxTries = 5000000,
		eMaxInvalidTries = 10000,
		eMaxValidationTries = 10000

	};

	AosU64IdGenPtr	mDocidIdGen;

public:
	AosDocidIdGen();
	~AosDocidIdGen();

	// Singleton class interface
	static AosDocidIdGen*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def) {return true;}

	bool setDocid(const char *id, const char *blocksize);
	u64	nextDocid(const AosRundataPtr &rdata); 

	bool modifyDef(
		const AosU64IdGenPtr &idgen, 
		const char *idstr, 
		const char *blocksize);
	bool verifyDocid(
		const AosXmlTagPtr &def, 
		const u64 &start_iilid, 
		const AosRundataPtr &rdata);
};

#endif
