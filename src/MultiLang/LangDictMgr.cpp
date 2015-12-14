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
// 11/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MultiLang/LangDictMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangDict.h"
#include "MultiLang/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosLangDictMgrSingleton,
                 AosLangDictMgr,
                 AosLangDictMgrSelf,
                "AosLangDictMgr");

// const OmnString sgDictTermCtnr = "zky_dict_terms";

AosLangDictMgr::AosLangDictMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosLangDictMgr::~AosLangDictMgr()
{
}


bool      	
AosLangDictMgr::start()
{
	return true;
}


bool        
AosLangDictMgr::stop()
{
	return true;
}


bool
AosLangDictMgr::config(const AosXmlTagPtr &def)
{
	aos_assert_r(def, false);
	AosXmlTagPtr conf = def->getFirstChild("dictionary");
	if (!conf)
	{
		mDirname = ".";
		return true;
	}

	mDirname = conf->getAttrStr("dirname");
	if (mDirname == "") mDirname = ".";
	return true;
}


OmnString 
AosLangDictMgr::getTerm(const OmnString &id, AosRundata* rdata)
{
	AosLocale::E locale = rdata->getLocale();
	AosLangDictPtr dict = getDictionary(locale);
	if (dict)
	{
		return dict->getTerm(id, rdata);
	}

	return id;
}


AosLangDictPtr
AosLangDictMgr::getDictionary(const AosLocale::E locale)
{
	aos_assert_r(AosLocale::isValid(locale), 0);
	mLock->lock();
	AosLangDictPtr dict = mDictionaries[locale];
	if (dict)
	{
		mLock->unlock();
		return dict;
	}

	// It has not been created yet. Create it now.
	if (mDirname == "")
	{
		// The class has not been configured yet or not configured correctl.
		mLock->unlock();
		return 0;
	}

	dict = OmnNew AosLangDict(locale);
	mDictionaries[locale] = dict;
	mLock->unlock();
	dict->config(mDirname);
	return dict;
}


u64 
AosLangDictMgr::getMember(
		const OmnString &container_objid, 
		const OmnString &id_name, 
		const OmnString &id_value,
		bool &is_unique,
		AosRundata *rdata)
{
	if (!mQueryObj) mQueryObj = AosQueryObj::getQueryObj();
	aos_assert_rr(mQueryObj, rdata, 0);
	// if (!mQueryObj)
	// {
	// 	OmnAlarm << "mQueryObj is null" << enderr;
	// 	return 0;
	// }

	return mQueryObj->getMember(container_objid, id_name, id_value, is_unique, rdata);
}

