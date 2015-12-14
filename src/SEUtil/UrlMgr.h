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
// 	Created: 07/29/2010 by James Kong
////////////////////////////////////////////////////////////////////////////
#if 0
This file is moved to src/SEModules. Chen Ding, 12/22/2011




#ifndef AOS_SEUtil_UrlMgr_h
#define AOS_SEUtil_UrlMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Security/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"


OmnDefineSingletonClass(AosUrlMgrSingleton,
						AosUrlMgr,
						AosUrlMgrSelf,
						OmnSingletonObjId::eUrlMgr,
						"UrlMgr");


class AosUrlMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	bool		mIsStopping;

public:
	AosUrlMgr();
	~AosUrlMgr();

    // Singleton class interface
    static AosUrlMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool createUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef,
		bool &url_used); 

	//--Ketty
	/*bool	
	overrideUrl(
		const AosXmlTagPtr &root,
		const OmnString &siteid, 
		const AosSessionPtr &sdoc, 
		const AosXmlTagPtr &objdef,
		AosXmlRc &errcode, 
		OmnString &errmsg);*/
	bool overrideUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef);

	bool
	resolveUrl(
		const OmnString &siteid,
		const OmnString &url, 
		OmnString &vpdname,
		OmnString &objname,
		OmnString &svpd, 
		const AosRundataPtr &rdata);

};
#endif

#endif
