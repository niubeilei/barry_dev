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
// Created: 2011/02/11 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_SiteMgr_h
#define AOS_SEModules_SiteMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEModules/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Objid.h"
#include "SEUtil/ObjType.h"
#include "SEInterfaces/SiteMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/Locale.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"

using namespace std;


OmnDefineSingletonClass(AosSiteMgrSingleton,
						AosSiteMgr,
						AosSiteMgrSelf,
						OmnSingletonObjId::eSiteMgr,
						"SiteMgr");


class AosSiteMgr : virtual public AosSiteMgrObj
{
	OmnDefineRCObject;

public:
	typedef hash_map<u32, AosSitePtr> SiteMap;
	typedef hash_map<u32, AosSitePtr>::iterator SiteItr;

private:
	OmnMutexPtr		mLock;
	AosSitePtr		mDftSite;
	SiteMap 		mSites;
	OmnString		mLanguageCode;

	static u64		smDefaultSiteId;

public:
	AosSiteMgr();
	~AosSiteMgr();

    // Singleton class interface
    static AosSiteMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// Chen Ding, 2013/02/11
	// virtual u64 getDftSiteid() {return AOS_DEFAULT_SITE;}
	virtual u64 getDftSiteId() const {return smDefaultSiteId;}

	// Chen Ding, 07/29/2011
	OmnString getUnknownCloudid(const AosRundataPtr &rdata);
	OmnString getGuestCloudid(const AosRundataPtr &rdata);
	AosSitePtr getSite(const AosRundataPtr &rdata);
	OmnString getLanguageCode(const AosRundataPtr &rdata);
	AosLocale::E getLocale(const AosRundataPtr &rdata);

private:
	bool	init(const AosRundataPtr &rdata);
};
#endif

