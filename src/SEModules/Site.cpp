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
#include "SEModules/Site.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/UrlMgr.h"
#include "SEUtil/Objname.h"
#include "SEBase/SecUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"
#include "API/AosApi.h"
#include "XmlUtil/AccessRcd.h"

AosSite::AosSite(const u32 siteid, const AosRundataPtr &rdata)
:
mDftLocale(AosLocale::getDftLocale())
{
	mSiteid = siteid;
	if (mSiteid == 0) 
	{
		OmnString errmsg = "Missing siteid";
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}
	rdata->setSiteid(mSiteid);

	// if (!init(rdata))
	// {
	// 	OmnString errmsg = "Failed creating the site: ";
	// 	errmsg << mSiteid;
	// 	OmnExcept e(__FILE__, __LINE__, errmsg);
	// 	throw e;
	// }
}


AosSite::AosSite(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
{
	mSiteid = config->getAttrU32(AOSTAG_SITEID,0);
	if (mSiteid == 0)
	{
		OmnString errmsg = "Missing siteid";
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}

	// if (flag) 
	// {
	// 	if (!init(rdata))
	// 	{
	// 		OmnString errmsg = "Failed creating the site: ";
	// 		errmsg << mSiteid;
	// 		OmnExcept e(__FILE__, __LINE__, errmsg);
	// 		throw e;
	// 	}
	// }
}


AosSite::~AosSite()
{
}


bool
AosSite::init(const AosRundataPtr &rdata)
{
	// The system container structure is as follows:
	// 	Root Container:
	// 		Lost and Found Container
	//
	// 	Root Container:
	// 	 	AOSOBJID_SESSIONMGR;
	// 	 	AOSOBJID_DEFAULT_CTNR;
	// 	 	AOSOBJID_USER;
	// 	 	AOSOBJID_ALBUM;
	// 	 	AOSOBJID_ARTICLE;
	// 	 	AOSOBJID_BLOG;
	// 	 	AOSOBJID_COMMENT;
	// 	 	AOSOBJID_DIARY;
	// 	 	AOSOBJID_CLOUDCARD;
	// 	 	AOSOBJID_USERGROUP;
	// 	 	AOSOBJID_IMAGE;
	// 	 	AOSOBJID_LOG;
	// 	 	AOSOBJID_LOGINLOG;
	// 	 	AOSOBJID_PUBLISH;
	// 	 	AOSOBJID_USERROLE;
	// 	 	AOSOBJID_SYS_BACKLOG;
	// 	 	AOSOBJID_USERACCT;
	// 	 	AOSOBJID_VERSION;
	// 	 	AOSOBJID_VPD;
	// 	 	Login Log Container
	// 	 	Invalid Read Log Container
	// 		Cloudid Container
	// 		Public Namespace
	// 		Lost and Found
	// 		System User Container
	// 			Root Account
	// 			System Admin Account (not implemented yet)
	// 			Guest Account
	// 			Unknown User Account
	//
	// It makes sure the following objects are created:
	// 	1. Root Container 
	// 	2. System user container
	// 	3. super user account
	// 	4. CloudID Container
	// 	5. System Default Container
	// 	6. System Container
	// 	7. System Default User Container
	// 	8. Guest User
	// 	9. Lost and Found Container
	// 	10. Dictoinary Container
	//
	
	//if (AosDocClient::getSelf()->isRepairing()) return true;

	// 1. Create the root container. This is the container for all 
	//    docs for the site.
	u32 siteid_back = rdata->setSiteid(mSiteid);
	AosXmlTagPtr rootctnr = AosObjMgr::getSelf()->createRootCtnr(rdata);
	rdata->setSiteid(siteid_back);
	aos_assert_r(rootctnr, false);

	// 2. Create the sysroot. This is the first object a site creates.
	//    This is a root user account. The system will use this account
	//    to create the following system-level objects.
	AosXmlTagPtr sysroot = AosObjMgr::getSelf()->createSysRootAcct(rdata);
	if (!sysroot)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingSysRoot);
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		AosUserAcctObj::verifySysAcct(sysroot, rdata);
	}

	// 3. Create the system user container. This is the container for 
	//    all the system user account docs.
	AosXmlTagPtr sys_userctnr = AosObjMgr::getSelf()->createSysUserCtnr(rdata);
	aos_assert_r(sys_userctnr, false);

	// 4. Create the cid container
	AosXmlTagPtr cid_ctnr = AosObjMgr::getSelf()->createCidCtnr(rdata);
	aos_assert_r(cid_ctnr, false);

	// 5. Create User Domain Container
	OmnString objid = AosObjid::composeUserDomainCtnrObjid(mSiteid);
	AosXmlTagPtr user_domain = AosObjMgr::getSelf()->createSystemCtnr(
			objid, AOSDOCID_USRDOMAINCTNR, AOSSTYPE_USERDOMAIN, rdata);
	aos_assert_r(user_domain, false);

	// 6. Create IDGen Container
	objid = AosObjid::composeIdGenCtnrObjid();
	AosXmlTagPtr idgen_container = AosObjMgr::getSelf()->createSystemCtnr(
			objid, AOSDOCID_IDGEN_CONTAINER, AOSSTYPE_IDGEN_CONTAINER, rdata);
	aos_assert_r(idgen_container, false);

	// Create System Monitor Container
	AosObjMgr::getSelf()->createSysContainer(rdata, 
			AOSCTNR_SYSMONITOR, AOSDOCID_MONITORLOGCTNR, AOSOBJNAME_SYSMONITOR_CONTAINER, "");

	// Create System Temporary Doc Container
	AosXmlTagPtr tmpobj_ctnr = AosObjMgr::getSelf()->createSysContainer(rdata, 
			AOSCTNR_TEMPOBJ, AOSDOCID_TEMPDOCCTNR, AOSOBJNAME_TEMPOBJ_CONTAINER, "");

	// Create lost+found Doc Container
	AosObjMgr::getSelf()->createSysContainer(rdata, 
			AOSOBJIDPRE_LOSTaFOUND, AOSDOCID_LOSTFOUND_CONTAINER, 
			AOSOBJNAME_LOSTFOUND_CTNR, "");
	
	// Create system log  Container
	AosObjMgr::getSelf()->createSysLogContainer(rdata, 
			AOSCTNR_SYSLOGCTNR, AOSDOCID_SYSLOG_CONTAINER, 
			rootctnr->getAttrStr(AOSTAG_PARENTC));

	// Create system virtual file Container
	AosObjMgr::getSelf()->createSysContainer(rdata, 
			AOSCTNR_VIRTUALFILECTNR, AOSDOCID_VIRTUALFILE_CONTAINER, "", "");

	if (AosDocClientObj::getDocClient()->isRepairing()) return true;
	// Create the super user account
	AosLoginMgrObjPtr loginmgr_obj = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr_obj, false);
	AosXmlTagPtr root_acct = loginmgr_obj->createSuperUser(rdata);
	if (!root_acct)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingRootAcct);
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		AosUserAcctObj::verifySysAcct(root_acct, rdata);
	}

	// Create Guest User Account
	AosXmlTagPtr guestdoc;
	bool rslt = AosLoginMgr::createGuestUser(rdata, guestdoc);
	if (!rslt || !guestdoc)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingGuestUser);
		OmnAlarm << rdata->getErrmsg() << enderr;
	}
	else
	{
		AosUserAcctObj::verifySysAcct(guestdoc, rdata);
	}

	// Create Unknown User Account
	AosXmlTagPtr unknown_user = loginmgr_obj->createUnknownUser(rdata);
	if (!unknown_user)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingUnknownUser);
		OmnAlarm << rdata->getErrmsg() << ". Failed creating Unknown User!" << enderr;
	}
	else
	{
		AosUserAcctObj::verifySysAcct(unknown_user, rdata);
	}
	
	// Create system log smartdoc
	AosObjMgr::getSelf()->createSysLogSmartdoc(rdata, 
			AOSCTNR_SYSLOGSMARTDOC,AOSCTNR_SYSLOGCTNR);


	// create system Url 
	AosUrlMgr::getSelf()->createSysUrl(rdata);

	// Create Dictionary Container
	AosObjMgr::getSelf()->createSysContainer(rdata, 
			AOSRSVEDOBJID_DICT_PARENT, AOSDOCID_DICT_PARENT, AOSOBJNAME_DICT_PARENT, "");
	
	// Create all the dictionary containers
	for (u32 i=AosLocale::eInvalid+1; i<AosLocale::eMax; i++)
	{
		u64 docid = AOSDOCID_DICT_START + i;
		if (docid > AOSDOCID_DICT_END)
		{
			OmnAlarm << "Too many dictionaries: " << docid << enderr;
		}
		else
		{
			OmnString ctnr_objid = AosObjid::composeDictCtnrObjid((AosLocale::E)i);
			OmnString objname = AOSOBJNAME_DICTCTNR;
			AosObjMgr::getSelf()->createSysContainer(
					rdata, ctnr_objid, docid, AOSOBJNAME_DICTCTNR, AOSRSVEDOBJID_DICT_PARENT);
		}
	}
	rslt = AosObjMgr::getSelf()->createSizeIdContainer(rdata);
	aos_assert_r(rslt, false);

	rslt = AosObjMgr::getSelf()->createSystemDocs(rdata);
	aos_assert_r(rslt, false);

	//create job id doc
	OmnString dp_str = "";                                                                                  
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << AOSTAG_JOBOBJID << "\">";
	dp_str << "</jimodataproc>";
	AosXmlTagPtr dp_xml = AosStr2Xml(rdata.getPtr(), dp_str AosMemoryCheckerArgs);
	aos_assert_r(dp_xml, false);
	dp_xml->setAttr("zky_job_id", 1);
	dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	AosXmlTagPtr xml = AosCreateDoc(dp_xml->toString(), true, rdata);
	return true;
}


// Chen Ding, 07/29/2011
OmnString
AosSite::getUnknownCloudid(const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return "";
}


// Chen Ding, 07/29/2011
OmnString
AosSite::getGuestCloudid(const AosRundataPtr &rdata) const
{
	OmnNotImplementedYet;
	return "";
}

