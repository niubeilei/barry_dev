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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_LoginMgrObj_h
#define Aos_SEInterfaces_LoginMgrObj_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosLoginMgrObj : virtual public OmnRCObject
{
private:
	static AosLoginMgrObjPtr smLoginMgr;

public:
	virtual AosXmlTagPtr createSuperUser(const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr createHomeFolder3(
						 const AosRundataPtr &rdata, 
						 const OmnString &usr_cid) = 0;
	virtual AosXmlTagPtr createUnknownUser(const AosRundataPtr &rdata) = 0;
	virtual bool 	createUserAccount(
						 const AosRundataPtr &rdata,	
						 const AosXmlTagPtr &udoc,
						 OmnString &resp) = 0;
	virtual bool createUserDirs(
						 const u64 &ctnr_docid, 
						 const AosXmlTagPtr &userobj, 
						 const AosRundataPtr &rdata) = 0;
	virtual bool checkLogin(
						 const AosRundataPtr &rdata,
						 AosXmlTagPtr &userdoc,
						 const AosXmlTagPtr &cmd,
						 OmnString &hpvpd, 
						 OmnString &workvpd,
						 OmnString &publicvpd,
						 OmnString &familyvpd) = 0;
	virtual bool checkLogin(
						OmnString &errmsg,
						AosXmlTagPtr &userdoc,
						const OmnString &cid,
						const OmnString &pwd,
						const OmnString &ctnr,
						const AosRundataPtr &rdata) = 0;


	static AosLoginMgrObjPtr getLoginMgr() {return smLoginMgr;}
	static void setLoginMgrObj(const AosLoginMgrObjPtr &d) {smLoginMgr = d;}
};

inline AosXmlTagPtr AosLoginMgr_CreateSuperUser(const AosRundataPtr &rdata)
{
	AosLoginMgrObjPtr loginmgr = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr, 0);
	return loginmgr->createSuperUser(rdata);
}

inline bool AosCreateUserAccount(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &userdoc, 
		OmnString &resp)
{
	AosLoginMgrObjPtr loginmgr = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr, 0);
	return loginmgr->createUserAccount(rdata, userdoc, resp);
}

inline bool AosCreateUserDirs(
		const u64 &ctnr_docid, 
		const AosXmlTagPtr &userobj, 
		const AosRundataPtr &rdata)
{
	AosLoginMgrObjPtr loginmgr = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr, 0);
	return loginmgr->createUserDirs(ctnr_docid, userobj, rdata);
}

inline bool AosCheckLogin(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &cmd,
		OmnString &hpvpd, 
		OmnString &workvpd,
		OmnString &publicvpd,
		OmnString &familyvpd)
{
	AosLoginMgrObjPtr loginmgr = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr, 0);
	return loginmgr->checkLogin(rdata, userdoc, cmd, hpvpd, workvpd, publicvpd, familyvpd);
}

inline bool AosCheckLogin(
		OmnString &errmsg,
		AosXmlTagPtr &userdoc,
		const OmnString &cid,
		const OmnString &pwd,
		const OmnString &ctnr,
		const AosRundataPtr &rdata)
{
	AosLoginMgrObjPtr loginmgr = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr, 0);
	return loginmgr->checkLogin(errmsg, userdoc, cid, pwd, ctnr, rdata);

}


#endif
