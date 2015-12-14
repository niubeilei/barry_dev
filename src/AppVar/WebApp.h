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
// Modification History:
// 07/15/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AppVar_WebApp_h
#define Aos_AppVar_WebApp_h

#include "SearchEngine/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "AppVar/Ptrs.h"
#include "Rundata/Rundata.h"
#include <map>
#include <set>
using namespace std;

class AosWebApp : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr 						mLock;
	const OmnString 					mUrl;
	OmnString							mAppid;
	map<OmnString, map<OmnString,AosXmlTagPtr> >  mMap;
	OmnString							mDefualtDataFormat;
	OmnString							mLanguage;
	AosWebAppPtr						mBaseApp;
	set<OmnString>                   	mSet;
	OmnString                           mIsdel;

	static OmnMutexPtr		smLock;
	static map<OmnString, AosWebAppPtr> 	smApps;

public:
	AosWebApp(const OmnString &url, const AosRundataPtr &rdata);
	~AosWebApp();

	bool  init(
			const OmnString &appid,
			const AosRundataPtr &rdata);
	bool  sanityCheckBeforeAdding();
	bool  buildMap(
			const AosXmlTagPtr &maps,
			const OmnString &vpd_objid,
			const AosRundataPtr &rdata);
	void  comreplace(
			map<OmnString, AosXmlTagPtr> &map1,
			AosXmlTagPtr &vpd);
	bool  appConvert(
			AosXmlTagPtr &gic, 
			const AosXmlTagPtr &obj, 
			const AosRundataPtr &rdata);
	inline OmnString createKey(
			const OmnString &type,
			const OmnString &vpd_objid,
			const OmnString &gic)
	{
		OmnString key = type;
		key << "_" << vpd_objid  << "_" << gic;
		OmnScreen << "=====  " << key << "  =====" << endl;
		return key;
	}

	static AosWebAppPtr getApp(const OmnString &url, const AosRundataPtr &rdata);
	static bool resetAppData(const OmnString &url, const AosRundataPtr &rdata);
	static bool deleteApp(
			const OmnString &url, 
			const OmnString &delapp,
			const AosRundataPtr &rdata);
	static bool resetApp(const OmnString &url, const AosRundataPtr &rdata);
};
#endif
