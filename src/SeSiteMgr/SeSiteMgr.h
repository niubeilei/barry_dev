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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeSiteMgr_SeSiteMgr_h
#define AOS_SeSiteMgr_SeSiteMgr_h

#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlUtil/Ptrs.h"



OmnDefineSingletonClass(AosSeSiteMgrSingleton,
						AosSeSiteMgr,
						AosSeSiteMgrSelf,
						OmnSingletonObjId::eSeSiteMgr,
						"SeSiteMgr");

#define AOSSITE_DEFAULT_SITEID				"100"

class AosSeSiteMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	int				mSiteLoop;
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnString		mDftSite;

public:
	AosSeSiteMgr();
	~AosSeSiteMgr();

    // Singleton class interface
    static AosSeSiteMgr *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool		resetSiteLoop();
	OmnString	nextSite();
	OmnString	getDefaultSite() const {return mDftSite;}
};
#endif
