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
// 	Created: 03/18/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_SEModules_LogMgr_h
#define AOS_SEModules_LogMgr_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Rundata.h"


#define AOSLOGCTNR_LOGIN		"zkylgctnr_login"

OmnDefineSingletonClass(AosLogMgrSingleton,
						AosLogMgr,
						AosLogMgrSelf,
						OmnSingletonObjId::eLogMgr,
						"LogMgr");


class AosLogMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	bool		mIsStopping;

	enum
	{
		eDftPagesize = 20
	};

public:
	AosLogMgr();
	~AosLogMgr();

    //
    // Singleton class interface
    //
    static AosLogMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

    bool    start(const OmnString &dirname, 
				const OmnString &fname);

	// bool 	addLoginLogEntry(
	// 			const OmnString &siteid,
	// 			const AosSeLogType logtype,
	// 			const OmnString &logid,
	// 			const OmnString &contents);
	bool 	addLogEntry(const AosXmlTagPtr &logdoc, const AosRundataPtr &rdata);
	bool	getLog(
				const OmnString &siteid,
				const OmnString &logtype,
				const OmnString &logid, 
				const int pagesize,
				const bool reverse,
				OmnString &contents, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
	AosXmlTagPtr getLogEntry(const u64 &docid);

	bool
	rebuildLogEntry(
		const AosXmlTagPtr &logdoc, 
		const u32 seqno, 
		const u32 offset,
		const AosRundataPtr &rdata); 

	bool rebuildLogEntry(
			const AosXmlTagPtr &logdoc,
			const AosRundataPtr &rdata);

	bool
	addLogEntry(
		const OmnString &siteid, 
		const OmnString &logtype,
		const OmnString &logid, 
		const OmnString &docstr,
		const AosRundataPtr &rdata,
		const u32 logtime = 0);

	static void	
	createLogIILName(
		OmnString &iilname, 
		const OmnString &siteid,
		const OmnString &logid)
	{
		iilname = AOSZTG_LOG;
		iilname << siteid << ":" << logid;
	}

	static void	
	createLogIILName(
		OmnString &iilname, 
		const OmnString &siteid,
		const OmnString &logtype,
		const OmnString &logid)
	{
		iilname = AOSZTG_LOG;
		if (logtype == "")
		{
			iilname << siteid << "_" << logid;
		}
		else
		{
			iilname << siteid << ":" << logtype << "_" << logid;
		}
	}

	static OmnString constructLoginCtnr(const OmnString &user_ctnr_objid)
	{
		OmnString ctnr = AOSLOGCTNR_LOGIN;
		ctnr << user_ctnr_objid;
		return ctnr;
	}
		
	bool
	addCreateCtnrEntry(
		const OmnString &siteid,
		const OmnString &creator_id, 
		const OmnString &ctnr_objid, 
		const OmnString &msg);

	bool
	addSystemBackupLogEntry(
		const OmnString &siteid,
		const OmnString &type,
		const OmnString &fname,
		const u32 &starttime,
		const u32 &endtime,
		const OmnString &status, 
		const OmnString &command, 
		const AosRundataPtr &rdata);
	
	bool addInvalidReadEntry(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc);

	bool
	logWhoVisited(
		const OmnString &siteid,
		const AosXmlTagPtr &xml, 
		const u64 &userid);
	
	bool
	createWhoVisitedLogEntry(
		const AosRundataPtr &rdata,
		u32 &seqno,
		u64 &offset);

	u64 
	composeLogEntryId(u64 seqno, u64 offset)
	{
		u64 docid = 0;
		docid = (((u64)offset) << 32) + seqno;
		return docid;
	}
};
#endif
#endif
