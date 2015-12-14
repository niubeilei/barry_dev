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
// 11/30/2011	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MULTILANG_LANGDICTMGR_h
#define AOS_MULTILANG_LANGDICTMGR_h

#include "MultiLang/Ptrs.h"
#include "MultiLang/LangDict.h"
#include "MultiLang/DictUtil.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/Locale.h"
#include <vector>
using namespace std;

#ifndef AOSDICTERM
#define AOSDICTERM(id, rdata) AosLangDictMgr::getSelf()->getTerm(id, rdata)
#endif

class OmnFile;
OmnDefineSingletonClass(AosLangDictMgrSingleton,
						AosLangDictMgr,
						AosLangDictMgrSelf,
						OmnSingletonObjId::eLangDictMgr,
						"LangDictMgr");

class AosLangDictMgr : virtual public AosLangDictObj
{
	OmnDefineRCObject;

private:
	AosLangDictPtr			mDictionaries[AosLocale::eMax];
	OmnMutexPtr				mLock;
	AosDocClientObjPtr		mDocClient;
	AosQueryObjPtr			mQueryObj;
	OmnString				mDirname;

public:
	AosLangDictMgr();
	~AosLangDictMgr();

    // Singleton class interface
    static AosLangDictMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool 		config(const AosXmlTagPtr &def);

	virtual OmnString getTerm(  const OmnString &id, 
						AosRundata *rdata);
	u64 getMember( 		const OmnString &container_objid, 
						const OmnString &id_name, 
						const OmnString &id_value,
						bool &is_unique,
						AosRundata *rdata);

private:
	AosLangDictPtr getDictionary(const AosLocale::E locale);
};

#endif
