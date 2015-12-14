////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (u32, docid) and is sorted
// based on the u32 value. 
//
// Modification History:
// 01/31/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_LoginMgr_h
#define AOS_SEModules_LoginMgr_h

#include "IdGen/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosLoginMgr : virtual public AosLoginMgrObj
{
	OmnDefineRCObject;

private:
	static OmnMutexPtr	mLock;
	static OmnString	mUserBaseDir;
	static OmnString	mUserDataDir;	// Phnix, 2011/01/23
	OmnString 			mUserEmailDir;

public:
	AosLoginMgr(const AosXmlTagPtr &config, const bool);
	~AosLoginMgr() {}

	bool	checkLogin(
				OmnString &errmsg,
				AosXmlTagPtr &userdoc,
				const OmnString &cid,
				const OmnString &pwd,
				const OmnString &ctnr,
				const AosRundataPtr &rdata);

	bool	checkLogin(
				const AosRundataPtr &rdata,
				AosXmlTagPtr &userdoc,
				const AosXmlTagPtr &cmd,
				OmnString &hpvpd, 
				OmnString &workvpd,
				OmnString &publicvpd,
				OmnString &familyvpd);

	// OmnString getEmailDir(){return mUserEmailDir;};

	bool 	createUser(
				const u32 siteid,
				const AosXmlTagPtr &userdoc,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool 	createUserAccount(
		const AosRundataPtr &rdata,	
		const AosXmlTagPtr &udoc,
		OmnString &resp);

	OmnString getUserRealname(
				const u32 siteid,
				const OmnString &container,
				const OmnString &uname, 
				const AosRundataPtr &rdata);
	OmnString	checkCreateNewHpvpd(
			const AosRundataPtr &rdata,
			const OmnString &hpvpd, 
			const AosXmlTagPtr &ctnrxml, 
			const OmnString &user_cid);

	OmnString getDftHpvpdBySex1(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &def_tag,
				const AosXmlTagPtr &userdoc, 
				const AosXmlTagPtr &ctnrxml);

	virtual AosXmlTagPtr createHomeFolder3(
		const AosRundataPtr &rdata,
		const OmnString &usr_cid);

	static OmnString createUserBaseDir(
		const u32 siteid, 
		const u64 &docid);

	bool createUserDirs(
		const u64 &ctnr_docid, 
		const AosXmlTagPtr &userobj, 
		const AosRundataPtr &rdata);

	virtual AosXmlTagPtr createSuperUser(const AosRundataPtr &rdata);

	static bool createGuestUser(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &userobj);

	virtual AosXmlTagPtr createUnknownUser(const AosRundataPtr &rdata);

	static bool createUserCtnr(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_aname,
		const OmnString &ctnr_name,
		const OmnString &subtype,
		const OmnString &objname,
		OmnString &ctnr_objid,
		const OmnString &userhome_objid ,
		const AosXmlTagPtr &userdoc, 
		const OmnString &cid, 
		const bool checkCreation); 

	static AosXmlTagPtr getGuestDoc(const u32 siteid, const AosRundataPtr &rdata);

	static OmnString getRequesterUserDomain(const AosRundataPtr &rdata);
	static u64 getUserid(const OmnString &username, const AosRundataPtr &rdata);

private:
	OmnString createHomeVpd1(
		const AosXmlTagPtr &def_tag,
		const AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata);

	bool verifyPassword(
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &cid,
		const OmnString &username,
		const OmnString &passwd,
		const AosXmlTagPtr &userdoc, 
		const int level,
		AosXmlRc &errcode, 
		OmnString &errmsg,
		const AosRundataPtr &rdata);

	bool addLoginEntry(
		const u32 siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &actid, 
		const OmnString &errmsg,
		const AosRundataPtr &rdata);

	bool addLoginEntry(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		const OmnString &cid,
		const OmnString &username,
		const AosXmlTagPtr &userdoc,
		const OmnString &status, 
		const OmnString &msg,
		const AosRundataPtr &rdata);

	static void addCreateAcctEntry(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		const OmnString &username,
		const AosXmlTagPtr &userdoc);

	AosXmlTagPtr getUserCtnr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userobj, 
		OmnString &ctnr_objid);

	bool getParentFolder1(
		const u32 siteid,
		const u64 &userid,
		const OmnString &cid,
		const AosXmlTagPtr &userobj,
		const AosXmlTagPtr &userctnr,
		OmnString &pctnr_objid,
		AosXmlRc &errcode, 
		OmnString &errmsg);

	bool addOnlineEntry(
		const u32 siteid, 
		const OmnString &user_container, 
		const OmnString &realname, 
		const OmnString &userid,
		const AosRundataPtr &rdata);

	bool removeOnlineEntry(
		const u32 siteid, 
		const OmnString &user_container, 
		const OmnString &realname, 
		const OmnString &userid,
		const AosRundataPtr &rdata);

	bool retrieveOnlineEntries(
		const u32 startidx,
		const u32 psize);

	inline OmnString constructOnlineIILName(
		const u32 siteid,
		const OmnString &container)
	{
		return "";
	}

	static bool createUserObj(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userobj, 
		const OmnString &cid,
		const OmnString &username);

	static OmnString getNewCid(const u32 siteid, const AosRundataPtr &rdata);

private:
	OmnString composeNewVpdname(
		const OmnString &vpdname,
		const AosXmlTagPtr &def_tag, 
		const AosRundataPtr &rdata);

	bool createDefaultVpds(
		vector<OmnString> &anames,
		vector<OmnString> &vpdnames,
		const AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata);
};
#endif

