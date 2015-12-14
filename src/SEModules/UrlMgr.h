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
#ifndef AOS_SEModules_UrlMgr_h
#define AOS_SEModules_UrlMgr_h

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
#include <vector>
using namespace std;



OmnDefineSingletonClass(AosUrlMgrSingleton,
						AosUrlMgr,
						AosUrlMgrSelf,
						OmnSingletonObjId::eUrlMgr,
						"UrlMgr");


class AosUrlMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxParms = 10
	};

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

	bool overrideUrl(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &objdef);

	bool resolveUrl(
		const OmnString &url, 
		const OmnString &full_url,
		const OmnString &query_str,
		OmnString &vpdname,
		OmnString &objname,
		OmnString &svpd, 
		AosXmlTagPtr &url_doc,
		const AosRundataPtr &rdata);

	OmnString decodeUrl(const OmnString &url);

	bool	createSysUrl(const AosRundataPtr &rdata);

private:
	bool parseUrl(
		const OmnString &url, 
		vector<OmnString> &names,
		vector<OmnString> &values);

	bool checkHomes(
		const AosXmlTagPtr &urldoc, 
		const OmnString &url_home, 
		OmnString &vpdname, 
		OmnString &objname,
		OmnString &svpd, 
		const AosRundataPtr &rdata);
};
#endif

