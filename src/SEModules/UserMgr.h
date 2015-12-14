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
// 01/06/2011	Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_UserMgr_h
#define AOS_SEModules_UserMgr_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"



OmnDefineSingletonClass(AosUserMgrSingleton,
						AosUserMgr,
						AosUserMgrSelf,
						OmnSingletonObjId::eUserMgr,
						"UserMgr");

class AosUserMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr         mLock;
	
	static OmnString	mUserBaseDir;

public:
	AosUserMgr();
	~AosUserMgr();

    // Singleton class interface
    static AosUserMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool
	createUserDomain(
		const AosXmlTagPtr &doc,
		const AosXmlTagPtr root,
		const u32 siteid,
		const u64 &userid,
		AosXmlRc &errcode,
		OmnString &errmsg);

	bool createUserAccount(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &udoc,
		OmnString &resp);

	
	OmnString
	getNewCid(const u32 siteid, const AosRundataPtr &rdata);
	
	static OmnString
	getUserBaseDir(
		const u32 siteid, 
		const u64 &docid);

	AosXmlTagPtr
	getUserCtnr(
		const u32 siteid,
		const AosXmlTagPtr &userobj, 
		OmnString &ctnr_objid,
		const u64 &userid,
		const AosRundataPtr &rdata);

	bool createHomeFolder(
		const AosRundataPtr &rdata,
		const OmnString &usr_cid,
		const AosXmlTagPtr &userobj, 
		OmnString &pctnr_objid);

	bool
	createUserDirs(
		const u32 siteid,
		const u64 &ctnr_docid, 
		const AosXmlTagPtr &userobj);

	bool
	createHomeVpd1(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		OmnString &hpvpd,
		const OmnString &user_cid,
		const AosXmlTagPtr &userobj,
		AosXmlRc &errcode,
		OmnString &errmsg);

	bool
	createUserCtnr(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_aname,
		const OmnString &ctnr_name,
		const OmnString &subtype,
		const OmnString &objname,
		OmnString &ctnr_objid,
		const OmnString &userhome_objid ,
		const AosXmlTagPtr &userdoc, 
		const OmnString &cid); 

	bool
	createUserObj(
		const AosXmlTagPtr &root,
		const u32 siteid,
		const AosXmlTagPtr &userobj, 
		const u64 &userid, 
		const OmnString &cid,
		const OmnString &username,
		AosXmlRc &errcode, 
		OmnString &errmsg);

	void
	addCreateAcctEntry(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		const OmnString &username,
		const AosXmlTagPtr &userdoc);

	OmnString	getDftHpvpdBySex1(
		const AosRundataPtr &rdata,
		const OmnString &user_cid,
		const AosXmlTagPtr &xml, 
		const AosXmlTagPtr &ctnrxml);
	
	OmnString
	checkCreateNewHpvpd1(
		const AosRundataPtr &rdata,
		const OmnString &hpvpd, 
		const AosXmlTagPtr &ctnrxml, 
		const OmnString &user_cid);
};
#endif

