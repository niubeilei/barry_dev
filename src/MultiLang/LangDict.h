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
//
// Modification History:
// 11/30/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MultiLang_LangDict_h
#define Aos_MultiLang_LangDict_h

#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "Util/HashUtil.h"
#include <hash_map>
using namespace std;


class AosLangDict : public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosLocale::E	mLocale;
	AosStr2Str_t	mMap;
	OmnMutexPtr		mLock;
	OmnString		mCtnrObjid;

public:
	AosLangDict();
	AosLangDict(const AosLocale::E locale);

	bool config(const OmnString &dirname);
	OmnString getTerm(const OmnString &id, const AosRundataPtr &rdata);
	AosLocale::E getLocale() const {return mLocale;}

private:
	OmnString getFilename(const OmnString &dirname) const
	{
		OmnString fname = dirname;
		fname << "/Dict_" << AosLocale::toStr(mLocale);
		return fname;
	}
	int findName(const OmnString &line) const;
};

#endif
