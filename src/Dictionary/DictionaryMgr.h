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
// 	Created: 13/04/2011 by Lynch
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Dictionary_DictionaryMgr_h
#define AOS_Dictionary_DictionaryMgr_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "SEUtil/Ptrs.h"
#include "Dictionary/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"


OmnDefineSingletonClass(AosDictionaryMgrSingleton,
						AosDictionaryMgr,
						AosDictionaryMgrSelf,
						OmnSingletonObjId::eDictionaryMgr,
						"DictionaryMgr");

class AosDictionaryMgr : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	AosDictionaryPtr 		mDftDictionary;

public:
	AosDictionaryMgr();
	~AosDictionaryMgr();

    // Singleton class interface
    static AosDictionaryMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);

	// AosDictionaryMgr Interface
	const OmnString resolveTerm(
			const u64 &dictId, 
			const OmnString &languageCode, 
			const AosRundataPtr &rdata);

private:
};
#endif

