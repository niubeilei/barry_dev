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
//	
// Modification History:
// 08/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/ObjMgr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
//#include "DocClient/DocClient.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "Rundata/RdataUtil.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ObjType.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/SiteMgr.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEServer/SeReqProc.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SMDMgr.h"
#include "Util/StrSplit.h"
#include "Util/ValueRslt.h"
#include "SEBase/SecUtil.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/AccessRcdMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"


static bool sgInitialized = false;

OmnSingletonImpl(AosObjMgrSingleton,
                 AosObjMgr,
                 AosObjMgrSelf,
                "AosObjMgr");


AosObjMgr::AosObjMgr()
:
mIsStopping(false),
mLock(OmnNew OmnMutex())
{
	if (!sgInitialized)
	{
		init();
	}
}


AosObjMgr::~AosObjMgr()
{
}


bool
AosObjMgr::init()
{
	// Initialize mUserCtnrTags[].
	mUserCtnrTags[AosObjType::eAlbum]       = AOSTAG_CTNR_ALBUM;
	mUserCtnrTags[AosObjType::eArticle]     = AOSTAG_CTNR_ATCL;
	mUserCtnrTags[AosObjType::eBlog]        = AOSTAG_CTNR_BLOG;
	mUserCtnrTags[AosObjType::eComment]     = AOSTAG_CTNR_CMT;
	mUserCtnrTags[AosObjType::eCloudCard]   = AOSTAG_CTNR_CLOUDCARD;
	mUserCtnrTags[AosObjType::eDiary]       = AOSTAG_CTNR_DIARY;
	mUserCtnrTags[AosObjType::ePublish]     = AOSTAG_CTNR_PUBLISH;
	mUserCtnrTags[AosObjType::eVpd]         = AOSTAG_CTNR_VPD;
	mUserCtnrTags[AosObjType::eImage]       = AOSTAG_CTNR_IMAGE;

	return true;
}


bool      	
AosObjMgr::start()
{
	return true;
}


bool        
AosObjMgr::stop()
{
	mIsStopping = true;
	return true;
}


bool
AosObjMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


AosXmlTagPtr	
AosObjMgr::createLoginLogCtnr(const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	OmnString objid = AosObjid::composeLoginLogCtnrObjid(siteid);
	OmnString objname = AosObjname::composeLoginLogCtnrName();
	return createSysContainer(rdata, objid, 0, objname, "");
}


AosXmlTagPtr	
AosObjMgr::createCidCtnr(const AosRundataPtr &rdata)
{
	// This function checks whether the container has
	// been created. If yes, it does nothing. Otherwise, it 
	// creates it.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	OmnString objid = AosObjid::composeCidCtnrObjid(siteid);
	OmnString objname = AosObjname::composeCidCtnrName();
	return createSysContainer(rdata, objid, AOSDOCID_CIDCTNR, objname, "");
}


AosXmlTagPtr	
AosObjMgr::createInvReadLogCtnr(const AosRundataPtr &rdata)
{
	// This function checks whether the container has
	// been created. If yes, it does nothing. Otherwise, it 
	// creates it.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	OmnString objid = AosObjid::composeInvReadLogCtnr(siteid);
	OmnString objname = AosObjname::composeInvalidReadLogName();
	return createSysContainer(rdata, objid, 0, objname, "");
}


AosXmlTagPtr	
AosObjMgr::createRootCtnr(const AosRundataPtr &rdata)
{
	// There is a root container for each site. Its objid is
	// AOSOBJID_ROOT. If the container has not been created yet, 
	// this function will create it. 
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	mLock->lock();
	OmnString objid = AosObjid::getRootCtnrObjid(siteid);
	AosXmlTagPtr rootctnr = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (rootctnr)
	{
		mLock->unlock();
		return rootctnr;
	}

	// Need to create it.
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSysRootDocid(siteid, rdata));
	OmnString cid = rdata->setCid(getSysRootCid(siteid, rdata));
	OmnString docstr = "<ctnr ";
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_PARENTC << "=\"" << objid << "\" "
		<< AOSTAG_DOCID << "=\"" << AOSDOCID_ROOTCTNR << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_CTNR_ROOT << "\" "
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\" "
		<< "/>";
	rootctnr = AosDocClientObj::getDocClient()->createRootCtnr(docstr, rdata);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);
	mLock->unlock();
	return rootctnr;	
}


OmnString
AosObjMgr::getRootCtnrObjid(const u32 siteid)
{
	return AosObjid::getRootCtnrObjid(siteid);
}


OmnString
AosObjMgr::getSysUserCtnrObjid(const AosRundataPtr &rdata)
{
	AosXmlTagPtr sysctnr = createSysUserCtnr(rdata);
	if (sysctnr)
	{
		return sysctnr->getAttrStr(AOSTAG_OBJID);
	}
	OmnAlarm << "Failed to create sysctnr!" << enderr;
	return "";
}


AosXmlTagPtr	
AosObjMgr::createPubNamespace(const AosRundataPtr &rdata)
{
	// This object is used to manage the public name space
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	OmnString objid = AosObjid::composePubNamespaceObjid(siteid);
	OmnString objname = AosObjname::composePublicNameSpace();
	return createSysContainer(rdata, objid, 0, objname, "");
}


AosXmlTagPtr
AosObjMgr::createDftContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const bool is_public,
		const OmnString &cid)
{
	// This function creates a default container as needed. If 'is_public'
	// is true, it checks whether the public default container 
	// 		AOSCTNR_DFT_DOC 
	// is there. If yes, it returns that container. If it is not there, 
	// it will create it. 
	//
	// If 'is_public' is not true, it retrieves the user's doc and checks
	// whether the corresponding container has been created. If yes, it 
	// returns it. Otherwise, it creates the container.
	
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	aos_assert_rr(doc, rdata, 0);
	if (doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD)
	{
		// It is an access record. Its parent container should be the
		// user's access record container, which means that every user has
		// a container that is for all the access records for all the user's
		// docs. The container objid is determined as:
		// 		AOSCTNR_ACCESSRECORD + "." + cid
		// u64 owner_docid = doc->getAttrU64(AOSTAG_OWNER_DOCID, AOS_INVDID);
		// if (owner_docid == AOS_INVDID)
		// {
		// 	OmnAlarm << "Missing owner docid" << enderr;
		// 	return 0;
		// }

		OmnString pctnr_objid = doc->getAttrStr(AOSTAG_PARENTC);
		aos_assert_rr(pctnr_objid != "", rdata, 0);

		// Make sure the pctnr_objid is correct. Since this is an access record, 
		// the access record's owner doc's creator should be the cloudid to use.
		u64 owner_docid = doc->getAttrU64(AOSTAG_OWNER_DOCID, AOS_INVDID);
		if (owner_docid == AOS_INVDID)
		{
			rdata->setError() << "Missing owner docid. The access record: " 
				<< doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		AosXmlTagPtr owner_doc = AosDocClientObj::getDocClient()->getDocByDocid(owner_docid, rdata);
		if (!owner_doc)
		{
			rdata->setError() << "Failed retrieving the owner doc. The access record: "
				<< doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		OmnString cloudid = owner_doc->getAttrStr(AOSTAG_CREATOR);
		if (cloudid == "")
		{
			rdata->setError() << "Found a doc that has no creator. The doc: "
				<< doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
		if (pctnr_objid != ctnr_objid)
		{
			rdata->setError() << "Found a doc whose access record's parent container"
				" is incorrect. Expecting: " << ctnr_objid 
				<< " but actual: " << pctnr_objid
				<< ". The doc: " << doc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}

		AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
			AOSOBJNAME_ACCESS_RECORD_CTNR, cloudid);
		aos_assert_rr(arcd_ctnr, rdata, 0);
		return arcd_ctnr;
	}

	if (is_public)
	{
		return createSysDftContainer(rdata, doc);
	}
	return createDftUserContainer(rdata , doc, cid);
}


AosXmlTagPtr
AosObjMgr::createSysDftContainer(const AosRundataPtr &rdata, const AosXmlTagPtr &doc)
{
	// This function checks whether the corresponding system container
	// has been created. If not, it creates it. 
	// All system containers are sub-containers of the sysctnr, which 
	// is created by "createSysUserCtnr(...)".
	// System container objid is determined based on otype. There are a few
	// special system container that should be created separated, through 
	//
	// 	"createSysContainer(rdata, objid, objname)"
	// 	
	// For instance, the lost n found container should be created separately.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	OmnString rootctnr_objid = getRootCtnrObjid(siteid);
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);

	mLock->lock();
	OmnString objid = AosObjid::composeSysCtnrObjid(otype, siteid);
	AosXmlTagPtr sysdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (sysdoc)
	{
		mLock->unlock();
		return sysdoc;
	}

	// Need to create it. Note that in order to avoid dead locking, 
	// we do not lock the class. This may cause some problems, 
	// but very, very unlikely, especially after the system run
	// for a long time, all the system containers should have 
	// been created.
	//
	// Sys container does not have parent container.
	OmnString docstr = "<sysdft ";
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSOTYPE_SYSTEM << "\" "
		<< AOSTAG_PARENTC << "=\"" << rootctnr_objid << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\" "
		<< "/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, "", "", true, false, false, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	mLock->unlock();
	aos_assert_r(ctnr, 0);
	return ctnr;
}


AosXmlTagPtr
AosObjMgr::createSysContainer(
		const AosRundataPtr &rdata, 
		const OmnString &ctnr_objid, 
		const u64 &docid,
		const OmnString &objname, 
		const OmnString &parent_ctnr_objid)
{
	// This function creates a new system container whose objid is 'ctnr_objid'.
	// The container's parent is AOSOBJID_ROOT.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	// Check whether it has already been created
	mLock->lock();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
	if (ctnr) 
	{
		if (!docid || ctnr->getAttrU64(AOSTAG_DOCID, 0) == docid)
		{
			mLock->unlock();
			return ctnr;
		}

		// It is incorrect. Remove the doc.
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosDocClientObj::getDocClient()->deleteDoc(ctnr, rdata, "", true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		ctnr = 0;
	}

	if (docid != 0)
	{
		AosXmlTagPtr ddd = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (ddd)
		{
			OmnAlarm << "Object incorrect: " << ddd->toString() << enderr;	

			// It is incorrect. Remove the doc.
			OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
			u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
			AosDocClientObj::getDocClient()->deleteDoc(ddd, rdata, "", true);
			rdata->setAppname(appname);
			rdata->setUserid(userid);
		}
	}

	OmnString pctnr = parent_ctnr_objid;
	if (pctnr == "") pctnr = getRootCtnrObjid(siteid);
	OmnString docstr = "<sysctnr ";
	docstr << AOSTAG_OBJID << "=\"" << ctnr_objid << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_SYSCTNR << "\" "
		<< AOSTAG_PARENTC << "=\"" << pctnr << "\" "
		<< AOSTAG_OBJNAME << "=\"" << objname << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
		<< "/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	//Linda 2011/08/03
	//u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	//OmnString cid = rdata->setCid(AOSCLOUDID_ROOT);
	//ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
	//		rdata, docstr, AOSCLOUDID_ROOT, "", true, false, false, false, false, true);
	u64 userid = rdata->setUserid(getSysRootDocid(siteid, rdata));
	OmnString cid = rdata->setCid(getSysRootCid(siteid, rdata));
	ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, getSysRootCid(siteid, rdata), "", true, false, true, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);
	mLock->unlock();
	aos_assert_rr(ctnr, rdata, 0); 
	return ctnr;
}


AosXmlTagPtr
AosObjMgr::createDftUserContainer(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const OmnString &cid) 
{
	// This function checks whether the corresponding user container
	// has been created. If not, it creates it. 
	//
	// User container objid is determined by doc's otype, composed by:
	// 		AosObjid::composeUserCtnrObjid(otype, cid);
	//
	// IMPORTANT!!!!!!!!!!!!!!
	// In order to prevent dead lock, this function does not lock 
	// the class. This may have some problems.
	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);

	// Chen Ding, 2013/09/10
	// aos_assert_rr(otype != "", rdata, 0);
	if (otype == "")
	{
		otype = AOSOTYPE_UNKNOWN;
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_UNKNOWN);
	}

	OmnString ctnr_objid = AosObjid::composeUserCtnrObjid(otype, rdata->getSiteid());
	OmnString ctnr_name = AosObjname::composeUserCtnrName(otype);
	return createUserContainer(rdata, ctnr_objid, ctnr_name, cid);
	
	/*
	// 2. Retrieve the corresponding user container objid
	mLock->lock();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDoc(siteid, ctnr_objid, rdata);
	if (ctnr)
	{
		// The container has already been created. 
		mLock->unlock();
		return ctnr;
	}

	// Need to create. 
	userhome_objid = AosObjid::composeUserHomeCtnrObjid(cid);
	if (userhome_objid == ctnr_objid)
	{
		// IMPORTANT: When creating a user account, it should have already created 
		// the container 'home.cid', which is the same as 'userhome_objid',  
		// such as "home.100001". 
		ctnr = AosDocClientObj::getDocClient()->getDoc(siteid, userhome_objid, rdata);
		if (!ctnr)
		{
			rdata->setError() << "User home container missing, which should never happen. "
				<< cid << ":" << userhome_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		mLock->unlock();
		return ctnr;
	}

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	OmnString docstr = "<userctnr ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_PREFIX << "_" << otype
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_PREFIX << "_" << otype 
		<< "\" " << AOSTAG_PARENTC << "=\"" << userhome_objid 
		<< "\" " << AOSTAG_OBJID << "=\"" << ctnr_objid 
		<< "\"/>";

	ctnr = AosDocServerSelf->createDocSafe1(
			rdata, docstr, cid, "", 
			false, true, false, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_rr(ctnr, rdata, false);

	OmnString new_ctnr_objid = ctnr->getAttrStr(AOSTAG_OBJID);
	if (new_ctnr_objid != ctnr_objid)
	{
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
		bool rslt = AosDocServer::getSelf()->modifyAttrStr1(rdata, 
				userdoc->getAttrU64(AOSTAG_DOCID, 0), 
				userdoc->getAttrStr(AOSTAG_OBJID), 
				aname, new_ctnr_objid, "", false, true, 
				__FILE__, __LINE__);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		aos_assert_rr(rslt, rdata, false);
	}
	return ctnr;
	*/
}


AosXmlTagPtr
AosObjMgr::createUserContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid,
		const OmnString &ctnr_name,
		const OmnString &cid)
{
	// This function creates the user private container 'ctnr_objid'
	// for the user 'userid', which cloud id is 'cid'. 
	//
	// IMPORTANT!!!!!!!!!!!!!!
	// In order to prevent dead lock, this function does not lock 
	// the class. This may have some problems.
	// 1. Check whether the container has been created
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	u64 userid = rdata->getUserid();
	aos_assert_rr(userid, rdata, 0);

	AosXmlTagPtr ctnr_doc = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
	if (ctnr_doc)
	{
		// The container has already been created. 
		return ctnr_doc;
	}

	// 2. Retrieve the user record
	AosUserAcctObjPtr user_acct = AosDocClientObj::getDocClient()->getUserAcct(userid, rdata);
	aos_assert_r(user_acct, 0);

	if (user_acct->isGuestUser()) 
	{
		AosSetError(rdata, "Access Denied");
		return 0;
	}

	OmnString userhome_objid = user_acct->getAttrStr(AOSTAG_CTNR_HOME);
	if (userhome_objid == "")
	{
		// Chen Ding, 12/20/2010
		userhome_objid = AosObjid::composeUserHomeCtnrObjid(cid);
		aos_assert_r(userhome_objid != "", 0);

		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	
		bool rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				user_acct->getDocid(0), 
				user_acct->getObjid(), 
				AOSTAG_CTNR_HOME, userhome_objid, "", 
				false, true, true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		aos_assert_r(rslt, 0);
	}

	// Ketty 2011/04/12 for SdocCreateContainer
	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);

	AosXmlTagPtr doc;
	OmnString sobjids = "";
	if(cmd)
	{
		sobjids = cmd->getAttrStr(AOSTAG_SDOCCTNR);
		cmd->removeAttr(AOSTAG_SDOCCTNR);
	}

	if(sobjids != "")
	{
		rdata->setArg1(AOSARG_CTNR_OBJID, ctnr_objid);
		rdata->setArg1(AOSARG_CTNR_SUBTYPE, userhome_objid);

		OmnStrParser1 parser(sobjids, ",");
		OmnString sdocid;
		while((sdocid = parser.nextWord()) != "")
		{
			AosSmartDocObj::procSmartdocsStatic(sdocid, rdata);
			doc  = rdata->getRetrievedDoc();
		}
	}
	else
	{
		OmnString docstr = "<userctnr ";
		OmnString objname = (ctnr_name == "")?ctnr_objid:ctnr_name;
		docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_STYPE << "=\"" << AOSSTYPE_AUTO
			<< "\" " << AOSTAG_OBJNAME << "=\"" << objname
			<< "\" " << AOSTAG_PARENTC << "=\"" << userhome_objid 
			<< "\" " << AOSTAG_OBJID << "=\"" << ctnr_objid 
			<< "\"/>";

		doc = AosDocClientObj::getDocClient()->createDocSafe1(
				rdata, docstr, cid, "", false, false, false, false, false, false);
	}

	aos_assert_r(doc, 0);
	return doc;
}


AosXmlTagPtr	
AosObjMgr::createLostFoundCtnr(const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);
	OmnString objid = AosObjid::composeLostFoundCtnrObjid(siteid);
	OmnString objname = AosObjname::composeLostFoundCtnrName();
	return createSysContainer(rdata, objid, 0, objname, "");
}


AosAccessRcdPtr
AosObjMgr::createUserAccessRecord(
		const AosRundataPtr &rdata,
		const OmnString &cloudid,
		const u64 &creator_docid) 
{
	// This function creates the user's default access record, 
	// Each access record is for a doc. This is to create the
	// access record for the user's home container. 
	
	// Check whether the access record has already been created
	u32 siteid = rdata->getSiteid();
	AosAccessRcdPtr acrd = AosGetAccessRcd(rdata, 0, creator_docid, "", false);
	if (acrd) return acrd;

	// Make sure the access record container for the user has been 
	// created. 
	OmnString cid = cloudid;
	if (cid == "") cid = AosCloudidSvr::getCloudid(creator_docid, rdata);
	aos_assert_r(cid != "", 0);

	OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
			AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	aos_assert_r(arcd_ctnr, 0);

	OmnString docstr = "<arcd";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftReadAccType()
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftDelMemAccType()
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftAddMemAccType()
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftDeleteAccType()
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftCopyAccType()
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftCreateAccType()
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftWriteAccType()
			<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << creator_docid
			<< AosSecurityMgrObj::getSecurityMgr()->getDftReadAccType()
			<< "\"/>";

	// Chen Ding, 09/28/2010
	// Creating access records should always be public. In addition, 
	// since this is to create the default access record, and when 
	// creating a doc, the doc server will check the creation permission,
	// which will lead to calling this function again. To prevent
	// the dead loop, the creator is temporarily set to the root.
	// After the doc being created, it changes the creator to the
	// requester.
	// AosXmlTagPtr doc = AosDocServerSelf->createDocSafe1(
	// 	docstr, siteid,  creator_docid, AOSAPPNAME_SYSTEM, "", 
	// 	false, errcode, errmsg, false, false, false);
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr, cloudid, "", 
		true, false, false, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_r(doc, 0);
	// Chen Ding, 2013/06/10
	// return AosAccessRcdMgr::getSelf()->getAccessRecord(doc, rdata);

	// Ken Lee, 2013/06/18
	//return AosGetAccessRecord(doc, false, rdata);
	return AosGetAccessRecord(rdata, doc->getAttrU64(AOSTAG_DOCID, 0), false);
}


AosXmlTagPtr
AosObjMgr::createSysUserCtnr(const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	// Check whether the doc has already been created
	mLock->lock();
	OmnString objid = AosObjid::getSysUserCtnrObjid(siteid);
	AosXmlTagPtr sys_ctnr = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (sys_ctnr) 
	{
		mLock->unlock();
		return sys_ctnr;
	}

	OmnString rootctnr_objid = AosObjid::getRootCtnrObjid(siteid);
	AosXmlTagPtr root_ctnr = AosDocClientObj::getDocClient()->getDocByObjid(rootctnr_objid, rdata);
	aos_assert_r(root_ctnr, 0);
	//Linda, 2011/08/03
	//OmnString root_cid = root_ctnr->getAttrStr(AOSTAG_CLOUDID);
	OmnString root_cid = getSysRootCid(siteid, rdata);
	aos_assert_r(root_cid != "", 0);
	OmnString docstr = "<sysuserctnr ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_SYSCTNR
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_PARENTC << "=\"" << rootctnr_objid 
		<< "\" " << AOSTAG_OBJID << "=\"" << objid 
		<< "\" " << AOSTAG_DOCID << "=\"" << AOSDOCID_SYSUSERCTNR 
		<< "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	sys_ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, root_cid, "", 
			true, false, true, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	mLock->unlock();
	aos_assert_rr(sys_ctnr, rdata, 0);
	return sys_ctnr;
}


AosXmlTagPtr
AosObjMgr::getSysUserCtnr1(const u32 siteid, const AosRundataPtr &rdata)
{
	// Check whether the doc has already been created
	OmnString objid = AosObjid::getSysUserCtnrObjid(siteid);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	return doc;
}


AosXmlTagPtr
AosObjMgr::getGuest(const u32 siteid, const AosRundataPtr &rdata)
{
	OmnString objid = AosObjid::composeGuestObjid(siteid);
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
}


AosAccessRcdPtr
AosObjMgr::createGuestArcd(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &guestdoc)
{
	// Make sure the access record container for the user has been 
	// created. 
	u32 siteid = rdata->getSiteid();
	u64 guest_docid = guestdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	OmnString cid = guestdoc->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid != "", 0);

	//OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	//AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
	//		AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	//aos_assert_r(arcd_ctnr, 0);

	//OmnString objid = AosObjid::createArdObjid(guest_docid);
	OmnString docstr = "<arcd ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			//<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_PUBLIC 
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PUBLIC 
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << guest_docid
			<< "\"/>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(guest_docid, doc, rdata);
	aos_assert_r(rslt, 0);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_r(doc, 0);
	return AosAccessRcdMgr::getSelf()->convertToAccessRecord(rdata, doc);
}


AosXmlTagPtr
AosObjMgr::createUserInboxCtnr(
		const AosRundataPtr &rdata,
		const OmnString &cid)
{
	u32 siteid = rdata->getSiteid();
	//ttl++;
	//the cid is a friend id.
	// Make sure the container is not created yet.
	aos_assert_r(cid != "", 0);
	OmnString objid = AosObjid::composeUserInboxCtnrObjid(cid);
	AosXmlTagPtr ctnr_doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (ctnr_doc) return ctnr_doc;

	// Retrieve the user doc
	AosXmlTagPtr user_doc = AosUserDocMgr::getSelf()->getUserDoc(siteid, cid, rdata);
	aos_assert_r(user_doc, 0);
	u64 userid = user_doc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	
	// The doc does not exist. Create it.
	OmnString docidstr;
	OmnString docstr1 = "<requests ";
	docstr1 << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_INBOX_CTNR
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_INBOX_CTNR
			<< "\" " << AOSTAG_PARENTC << "=\"" << user_doc->getAttrStr(AOSTAG_CTNR_HOME) 
			<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	ctnr_doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr1, cid, "", 
		true, false, false, false, false, false);

	aos_assert_r(ctnr_doc, 0);
	u64 ctnr_docid = ctnr_doc->getAttrU64(AOSTAG_DOCID, 0);

	// Prepare to create the access record
	OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
			AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	aos_assert_r(arcd_ctnr, 0);

	// Create the access record for the container
	//OmnString ard_objid = AosObjid::createArdObjid(ctnr_docid);
	OmnString docstr = "<arcd ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << ard_objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PUBLIC 
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			//<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << ard_objid
			<< "\"/>";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(ctnr_docid, doc, rdata);
	aos_assert_r(rslt, 0);

	rdata->setAppname(appname);
	rdata->setUserid(userid);
	return ctnr_doc;
}


AosAccessRcdPtr
AosObjMgr::createAccessRecord1(
		const AosRundataPtr &rdata,
		const OmnString &cloudid, 
		const u64 &docid)
{
	// This function creates the access record for the doc 'docid'.
	// If 'cid' is not empty, use it. Otherwise, the cid is retrieved
	// from the creator of the doc.
	u64 userid = rdata->getUserid();
	u32 siteid = rdata->getSiteid();

	aos_assert_r(docid != AOS_INVDID, 0);
	aos_assert_r(userid != AOS_INVDID, 0);

	OmnString cid = cloudid;
	if (cid == "")
	{
		cid = AosCloudidSvr::getCloudid(userid, rdata);
		aos_assert_r(cid != "", 0);
	}

	OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
			AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	aos_assert_r(arcd_ctnr, 0);

	//OmnString objid = AosObjid::createArdObjid(docid);
	OmnString docstr = "<arcd ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftReadAccType()
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftDelMemAccType()
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftAddMemAccType()
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftDeleteAccType()
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" 
			<< AosSecurityMgrObj::getSecurityMgr()->getDftCopyAccType()
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftCreateAccType()
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\""
			<< AosSecurityMgrObj::getSecurityMgr()->getDftWriteAccType()
			<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << docid 
			<< "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(docid, doc, rdata);
	aos_assert_r(rslt, 0);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_r(doc, 0);

	// Ken Lee, 2013/06/18
	//return AosGetAccessRecord(doc, false, rdata);
	return AosGetAccessRecord(rdata, doc->getAttrU64(AOSTAG_DOCID, 0), false);
}


bool
AosObjMgr::procMetaObjidOnRetObj(
		const OmnString &objid,
		const AosMetaExtension::E meta_objid_type,
		const AosRundataPtr &rdata)
{
	switch (meta_objid_type)
	{
	case AosMetaExtension::eCreateByTemplate:
		 return createObjAsNeeded(objid, rdata);
		
	default:
		 break;
	}

	return true;
}


bool
AosObjMgr::createObjAsNeeded(const OmnString &objid, const AosRundataPtr &rdata) 
{
	// The objid should be in the form:
	// 		<objid>.${cbtpl:template_objid}
	// where '<objid>' is the objid of the object to be retrieved. If
	// the object does not exist, it will use 'template_objid' to 
	// retrieve a template and use that template to create the object.
	OmnString real_objid, template_objid;
	OmnString cid = AosCloudidSvr::getCloudid(rdata->getUserid(), rdata);
	OmnString errmsg;
	bool rslt = AosObjid::separateObjidTmpl(objid, real_objid, template_objid, cid, errmsg);
	if (!rslt)
	{
		AosSetError(rdata, errmsg);
		OmnAlarm << rdata->getErrmsg() << ". Objid incorrect: " << objid << enderr;
		return false;
	}

	AosXmlTagPtr xml = AosDocClientObj::getDocClient()->getDocByObjid(real_objid, rdata);
	if (xml) 
	{
		rdata->setRetrievedDoc(xml, true);
		return true;
	}

	// The object is not in the system yet. Need to create it
	xml = AosDocClientObj::getDocClient()->createDocByTemplate1(rdata, cid, real_objid, template_objid);
	rdata->setCreatedDoc(xml, true);
	return xml;
}


AosAccessRcdPtr
AosObjMgr::createUnknownUserArcd(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &unknowndoc)
{
	// Make sure the access record container for the user has been 
	// created. 
	u32 siteid = rdata->getSiteid();

	u64 unknown_docid = unknowndoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
	OmnString cid = unknowndoc->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid != "", 0);

	//OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	
	//AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
	//		AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	//aos_assert_r(arcd_ctnr, 0);

	//OmnString objid = AosObjid::createArdObjid(unknown_docid);
	OmnString docstr = "<arcd ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			//<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_PUBLIC 
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PUBLIC 
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PUBLIC
			<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << unknown_docid
			<< "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(unknown_docid, doc, rdata);
	aos_assert_r(rslt, 0);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_r(doc, 0);
	// Chen Ding, 2013/06/10
	// return AosAccessRcdMgr::getSelf()->getAccessRecord(doc, rdata);
	
	// Ken Lee, 2013/06/18
	//return AosGetAccessRecord(doc, false, rdata);
	return AosGetAccessRecord(rdata, doc->getAttrU64(AOSTAG_DOCID, 0), false);
}


AosAccessRcdPtr
AosObjMgr::createRootArcd(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &rootdoc)
{
	// Make sure the access record container for the user has been 
	// created. 
	u32 siteid = rdata->getSiteid();
	u64 guest_docid = rootdoc->getAttrU64(AOSTAG_DOCID, AOS_INVDID);

	OmnString cid = rootdoc->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid != "", 0);

	OmnString ctnr_objid = AosObjid::composeUserArcdHomeContainerObjid(cid);
	
	AosXmlTagPtr arcd_ctnr = createUserContainer(rdata, ctnr_objid, 
			AOSOBJNAME_ACCESS_RECORD_CTNR, cid);
	aos_assert_r(arcd_ctnr, 0);

	//OmnString objid = AosObjid::createArdObjid(guest_docid);
	OmnString docstr = "<arcd ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD 
			<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_AUTO 
			<< "\" " << AOSTAG_SITEID << "=\"" << siteid
			<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ACCESS_RECORD
			<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
			//<< "\" " << AOSTAG_OBJID << "=\"" << objid
			<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AOSACTP_PRIVATE 
			<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AOSACTP_PRIVATE
			<< "\" " << AOSTAG_OWNER_DOCID << "=\"" << guest_docid
			<< "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(guest_docid, doc, rdata);
	aos_assert_r(rslt, 0);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	aos_assert_r(doc, 0);
	// return AosAccessRcdMgr::getSelf()->getAccessRecord(doc, rdata);

	// Ken Lee, 2013/06/18
	//return AosGetAccessRecord(doc, false, rdata);
	return AosGetAccessRecord(rdata, doc->getAttrU64(AOSTAG_DOCID, 0), false);
}


OmnString
AosObjMgr::getRootCloudid(const u32 siteid, const AosRundataPtr &rdata)
{
	OmnString objid = AosObjid::getRootCtnrObjid(siteid);
	AosXmlTagPtr root_ctnr = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	aos_assert_r(root_ctnr, "");
	return root_ctnr->getAttrStr(AOSTAG_CLOUDID);
}


AosXmlTagPtr
AosObjMgr::getRootCtnr(const u32 siteid, const AosRundataPtr &rdata)
{
	OmnString objid = AosObjid::getRootCtnrObjid(siteid);
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
}


AosXmlTagPtr
AosObjMgr::getSysUserCtnr(const u32 siteid, const AosRundataPtr &rdata)
{
	OmnString objid = AosObjid::getSysUserCtnrObjid(siteid);
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
}


AosXmlTagPtr	
AosObjMgr::createSystemCtnr(
		const OmnString &objid,
		const u64 &docid,
		const OmnString &stype,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	mLock->lock();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (ctnr)
	{
		mLock->unlock();
		return ctnr;
	}

	// Need to create it.
	OmnString docstr = "<ctnr ";
	OmnString root_objid = getRootCtnrObjid(siteid);
	AosXmlTagPtr root_ctnr = AosDocClientObj::getDocClient()->getDocByObjid(root_objid, rdata);
	aos_assert_rl(root_ctnr, mLock, 0);
	//OmnString root_cid = root_ctnr->getAttrStr(AOSTAG_CLOUDID);
	OmnString root_cid = getSysRootCid(siteid, rdata);
	aos_assert_rl(root_cid != "", mLock, 0);
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << stype << "\" "
		<< AOSTAG_PARENTC << "=\"" << root_objid << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\"/>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	//u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	u64 userid = rdata->setUserid(getSysRootDocid(siteid, rdata));
	ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, root_cid,
		    "", true, false, true, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	mLock->unlock();
	aos_assert_r(ctnr, 0);
	return ctnr;
}


AosXmlTagPtr
AosObjMgr::createSysRootAcct(const AosRundataPtr &rdata)
{
	// This function creates the System Root Account, which is the
	// first doc a site is to create. This doc will not have a 
	// parent container. It is purely used for creating other 
	// system docs. Once it finishes creating other system docs, 
	// this doc should not be used (normally).
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	OmnString root_objid = AosObjid::composeSysRootAcctObjid(siteid);
	AosXmlTagPtr userobj = AosDocClientObj::getDocClient()->getDocByObjid(root_objid, rdata);
	if (userobj) return userobj;

	// Create the password
	OmnString root_passwd = "";
	root_passwd << rand();				// Create the password for Root
	root_passwd << AosGetUsec();

	OmnString docstr = "<sysroot ";
	OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT
		<< "\" " << AOSTAG_PARENTC << "=\"" << AosObjid::getRootCtnrObjid(siteid)
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_CLOUDID << "=\"" << AOSCLOUDID_SYSROOT
		<< "\" " << AOSTAG_USERNAME << "=\"" << AOSUSERNAME_SYSROOT
		<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_SYSROOT
		<< "\" " << AOSTAG_SYSACCT << "=\"" << AOSSTYPE_SYSACCT
		<< "\" " << AOSTAG_SYSACCT_SIGNATURE << "=\"" << signature
		<< "\" " << AOSTAG_REALNAME << "=\"" << AOSOBJNAME_SYSROOT
		<< "\" " << AOSTAG_USERTYPE << "=\"" << AOSUSERTYPE_ROOT 
		<< "\" " << AOSTAG_OBJID << "=\"" << root_objid 
		<< "\"><" << AOSTAG_PASSWD << ">" << root_passwd 
		<< "</" << AOSTAG_PASSWD << ">"
		<< "</sysroot>";
	
	AosXmlParser parser;
	userobj = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!userobj)
	{
		rdata->setError() << "Failed to create obj (Internal error)";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Reserved the objid
	u64 docid = AOSDOCID_SYSROOT;
	bool objid_changed = false;
OmnScreen << "To bind: " << root_objid << ":" << docid << endl;
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		root_objid, docid, objid_changed, false, true, rdata);
	aos_assert_rr(!objid_changed, rdata, 0);

	// Chen Ding, 09/06/2011
	// It failed retrieving the system root doc but the objid was
	// used by someone else. This is an error. This function will
	// ignore the error. 
	// if (!rslt || docid == AOS_INVDID)
	// {
	// 	rdata->setError() << "Failed to reserved the objid!";
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return 0;
	// }

	userobj->setAttr(AOSTAG_DOCID, docid);
	rdata->setReceivedDoc(userobj, true);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, userobj, 
			AOSCLOUDID_SYSROOT, "", true, false, true, true, true, false, true);
	aos_assert_rr(doc, rdata, 0);

	//rslt = AosDocClientObj::getDocClient()->bindCloudid(AOSCLOUDID_SYSROOT, docid, rdata);
	aos_assert_rr(rslt, rdata, 0);

	return userobj;
}

AosXmlTagPtr 
AosObjMgr::createAccessRecord(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ref_doc)
{
	aos_assert_r(ref_doc, 0);
	u64 ref_docid = ref_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(ref_docid, 0);
	OmnString docstr;
	docstr<<"<arcd "<< AOSTAG_OWNER_DOCID << "=\"" << ref_docid 
		<< "\" " << AOSTAG_OTYPE << "=\"" << AOSOTYPE_ACCESS_RCD
		<< "\" " << AOSTAG_SITEID << "=\"" << rdata->getSiteid()
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSDOCSTYPE_AUTO
		<< "\" " << AOSTAG_READ_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftReadAccType()
		<< "\" " << AOSTAG_DELMEM_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftDelMemAccType()
		<< "\" " << AOSTAG_ADDMEM_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftAddMemAccType()
		<< "\" " << AOSTAG_DELETE_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftDeleteAccType()
		<< "\" " << AOSTAG_COPY_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftCopyAccType()
		<< "\" " << AOSTAG_CREATE_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftCreateAccType()
		<< "\" " << AOSTAG_WRITE_ACSTYPE << "=\"" << AosSecurityMgrObj::getSecurityMgr()->getDftWriteAccType()
		<< "\"/>";
	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	bool rslt = AosDocClientObj::getDocClient()->writeAccessDoc(ref_docid, doc, rdata);
	aos_assert_r(rslt, 0);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	AosXmlTagPtr arcd = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	aos_assert_r(arcd, 0);
	aos_assert_r(arcd->getAttrStr(AOSTAG_DOCID, "")!= "", 0);
	aos_assert_r(arcd->getAttrU64(AOSTAG_OWNER_DOCID, 0) == ref_docid , 0);
	return arcd;
}


AosXmlTagPtr
AosObjMgr::createIdGenDoc(
		const OmnString &objid,
		const bool is_public,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (ctnr) 
	{
		mLock->unlock();
		return ctnr;
	}

	OmnString docstr = "<idgen_ctnr ";
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_IDGEN << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_IDGEN << "\" "
		<< AOSTAG_PARENTC << "=\"" << AosObjid::composeIdGenCtnrObjid() << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\"/>";

	ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, rdata->getCid(), "", 
			is_public, true, false, false, true, true);
	mLock->unlock();
	aos_assert_rr(ctnr, rdata, 0); 
	return ctnr;
}


AosXmlTagPtr 
AosObjMgr::createSysLogSmartdoc(
		const AosRundataPtr &rdata,
		const OmnString &objid,
		const OmnString &ctnrname)
{
	aos_assert_r(objid != "", 0);
	aos_assert_r(ctnrname != "", 0);
	
	AosXmlTagPtr smartdoc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (smartdoc) 
	{
		return smartdoc;
	}

	OmnString docstr = "<sdoc ";
	docstr << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_SMARTDOC << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_SYSCTNR << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnrname << "\" />";
	u64 userid = rdata->setUserid(getSysRootDocid(rdata->getSiteid(), rdata));
	OmnString cid = rdata->setCid(getSysRootCid(rdata->getSiteid(), rdata));
	smartdoc = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, getSysRootCid(rdata->getSiteid(), rdata), "", 
			true, true, false, false, true, true);
	aos_assert_rr(smartdoc, rdata, 0);
	rdata->setUserid(userid);
	rdata->setCid(cid);
	return smartdoc;
}


AosXmlTagPtr 
AosObjMgr::createSysLogContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid, 
		const u64 &docid,
		const OmnString &ctnr_name)
{
	aos_assert_r(ctnr_objid != "", 0);
	
	// This function creates a new system log's container whose objid is 'ctnr_objid'.
	// The container's parent is ctnr_name.
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, 0);

	// Check whether it has already been created
	mLock->lock();
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);
	if (ctnr) 
	{
		if (!docid || ctnr->getAttrU64(AOSTAG_DOCID, 0) == docid)
		{
			mLock->unlock();
			return ctnr;
		}

		// It is incorrect. Remove the doc.
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosDocClientObj::getDocClient()->deleteDoc(ctnr, rdata, "", true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		ctnr = 0;
	}

	if (docid != 0)
	{
		AosXmlTagPtr ddd = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
		if (ddd)
		{
			OmnAlarm << "Object incorrect: " << ddd->toString() << enderr;	

			// It is incorrect. Remove the doc.
			OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
			u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
			AosDocClientObj::getDocClient()->deleteDoc(ddd, rdata, "", true);
			rdata->setAppname(appname);
			rdata->setUserid(userid);
		}
	}

	OmnString rootctnr_objid = getRootCtnrObjid(siteid);
	OmnString docstr = "<syslogctnr ";
	docstr << AOSTAG_OBJID << "=\"" << ctnr_objid << "\" "
		<< AOSTAG_DOCID << "=\"" << docid << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
		<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_SYSCTNR << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnr_name << "\" "
		<< AOSTAG_AUTOCREATED << "=\"" << "true" << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\">"
		<< "<" << AOSTAG_LOG_DEFINITION << ">"
		<< "<" << AOSTAG_LOG_INDEX << ">"
		<< "</" << AOSTAG_LOG_INDEX << ">"
		<< "</" << AOSTAG_LOG_DEFINITION << ">"
		<< "</syslogctnr>";

	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	//Linda 2011/08/03
	//u64 userid = rdata->setUserid(getSuperUserDocid(siteid, rdata));
	//OmnString cid = rdata->setCid(AOSCLOUDID_ROOT);
	//ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
	//		rdata, docstr, AOSCLOUDID_ROOT, "", true, false, false, false, false, true);
	u64 userid = rdata->setUserid(getSysRootDocid(siteid, rdata));
	OmnString cid = rdata->setCid(getSysRootCid(siteid, rdata));
	ctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, getSysRootCid(siteid, rdata), "", true, false, true, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setCid(cid);
	mLock->unlock();
	aos_assert_rr(ctnr, rdata, 0); 
	return ctnr;
}


bool
AosObjMgr::createObjsAtStartup()
{
	// This function is called when the system starts up. 
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);
	AosXmlTagPtr objs = config->getFirstChild("createobjs_at_start");
	if (!objs) return true;
	
	// The format is:
	// 	<createobjs_at_startup ...>
	// 		<obj .../>
	// 		...
	// 	</createobjs_at_startup>
	AosXmlTagPtr objdef = objs->getFirstChild();
	AosRundataPtr rdata = OmnApp::getRundata();
	// Chen Ding, 2013/02/11
	// u64 siteid = AosSiteMgr::getDftSiteid();
	u64 siteid = AosGetDftSiteId();
	u64 userid = rdata->setUserid(getSysRootDocid(siteid, rdata));
	OmnString cid = rdata->setCid(getSysRootCid(siteid, rdata));
	while (objdef)
	{
		OmnString objid = objdef->getAttrStr(AOSTAG_OBJID);
		AosXmlTagPtr doc = AosRetrieveDocByObjid(objid, false, rdata);
		if (!doc)
		{
			// The doc is not there yet. Create it.
			objdef = objdef->clone(AosMemoryCheckerArgsBegin);
			AosCreateDoc(doc, rdata);
		}
		objdef = objs->getNextChild();
	}
	rdata->setUserid(userid);
	rdata->setCid(cid);
	return true;
}

bool
AosObjMgr::createSizeIdContainer(const AosRundataPtr &rdata)
{
	//Jozhi 2014-04-16 default create sizeid doc it's a container
	AosXmlTagPtr ctnr = AosDocClientObj::getDocClient()->getDocByObjid(AOSCTNR_SIZEID, rdata);
	if (!ctnr)
	{
		OmnString str;
		str << "<sizeid "
			<< AOSTAG_OBJID << "=\"" << AOSCTNR_SIZEID << "\" " 
			<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< "/>";
		ctnr = AosCreateDoc(str, true, rdata);
	}
	aos_assert_r(ctnr, false);
	OmnScreen << "===============create sizeids container: " << ctnr->toString() << endl;
	return true;
}


bool
AosObjMgr::createSystemDocs(const AosRundataPtr &rdata)
{
	OmnString docsDir = "systemDocs";
	if (access(docsDir.data(), F_OK) == -1) return true;

	OmnScreen << "init system doc ..." << endl;
	list<OmnString> docsList;
	if (OmnFile::getFiles(docsDir, docsList, false) == -1)
	{
		return true;
	}

	for (list<OmnString>::iterator itr=docsList.begin();
			itr != docsList.end(); itr++)
	{
		OmnString fname = *itr;
		OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
		if (!file || !file->isGood())
		{
			return false;
		}
		OmnString docstr;
		file->readToString(docstr);
		AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
		aos_assert_r(doc, false);
		rdata->setReceivedDoc(doc, true);
		bool rslt = AosModifyDoc(doc, rdata);
		aos_assert_r(rslt, false);
	}
	
	return true;
}
