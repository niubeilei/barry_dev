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
// 03/18/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/UserMgr.h"

#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Objname.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "Python/Pyemail.h"
#include "SEModules/ObjMgr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"
#include "Rundata/Rundata.h"


const int sgMaxEmails = 10;
static OmnString sgEmails[sgMaxEmails];
static int sgCrtEmail = 0;
OmnString AosUserMgr::mUserBaseDir;

OmnSingletonImpl(AosUserMgrSingleton,
                 AosUserMgr,
                 AosUserMgrSelf,
                "AosUserMgr");

AosUserMgr::AosUserMgr()
{
}


AosUserMgr::~AosUserMgr()
{
}


bool      	
AosUserMgr::start()
{
	return true;
}


bool        
AosUserMgr::stop()
{
	return true;
}


bool
AosUserMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool
AosUserMgr::createUserDomain(
		const AosXmlTagPtr &doc,
		const AosXmlTagPtr root,
		const u32 siteid,
		const u64 &userid, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	/*
	// This function creates a new user domain. 
	
	// 1. All user domain OTYPE must be AOSOTYPE_USERDOMAIN
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERDOMAIN);

	// 2. Create the doc
	AosXmlDocPtr header;
	int ttl = 0;
	bool rslt = AosDocClient::getSelf()->createDoc1(siteid, AOSAPPNAME_SYSTEM, userid, root, 0, 
			"", doc, true, header, 0, 0, errcode, errmsg, false, ttl);
	if (!rslt)
	{
		return false;
	}

	// 3. Create the admin container for the domain
	OmnString docstr = "<ctnr ";
	
	//?????????????????
	//AosObjid::composeCtnrAdminObjid is not exist
	//need to change~~~~
	//?????????????????
	docstr << AOSTAG_OBJID << "=\"" << AosObjid::composeCtnrAdminObjid(
			doc->getAttrStr(AOSTAG_DOCID)) << "\"" 
		   << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERDOMAIN_ADMIN << "\""
		   << AOSTAG_USER_DOMAIN_OBJID << "=\"" << doc->getAttrStr(AOSTAG_OBJID) << "\""
		   << " />";
	
	ttl = 0;
	AosXmlTagPtr admin_doc = AosDocClient::getSelf()->createDocSafe1(
			docstr, siteid, userid, AOSAPPNAME_SYSTEM, "", "", true, 
			false, errcode, errmsg, false, false, false, true, ttl);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


OmnString
AosUserMgr::getNewCid(const u32 siteid, const AosRundataPtr &rdata)
{
	return AosSeIdGenMgr::getSelf()->nextCid(siteid, rdata);
}


bool
AosUserMgr::createUserAccount(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &udoc,
		OmnString &resp)
{
	// This function creates a new user account based on the information
	// in 'root', which is in the form:
	// 	<req ...>
	// 		<objdef>
	// 			<user zky_uname="xxx" zky_usrctnr="xxx">
	// 				<passwd><![CDATA[xxx]]></passwd>
	// 				<folders pctnrs="xxx" homefolder="xxx"/>
	// 			</user>
	// 		</objdef>
	// 	</req>
	//
	// User account contents are organized as:
	// 	zcid (a container)
	// 		zcid_cidnnnn		(a cloud id)
	// 			nnn				(the user's resource)
	// 			nnn				(the user's resource)
	// 			...
	// 		zcid_cidnnnn		(another cloud id)
	// 			nnn				(the user's resource)
	// 			nnn				(the user's resource)
	// 			...
	//
	// It will do the following:
	// 1. If zkycloudid is not created yet, it will create it.
	// 2. Create the container cidnnnn
	// 3. Create all the standard sub-containers under 'cidnnnn'
	// 4. Create the user object
	// 5. Log
	// 
	// Upon success, it will return the following:
	// 	<resp username="xxx"
	// 		  hpvpd="xxx"
	// 		  zky_homctnr="xxx"
	// 		  user_objid="xxx"
	// 		  userblog_ctnr="xxx"
	// 		  useralbum_ctnr="xxx"
	// 		  usercmt_ctnr="xxx"
	// 		  />
	u32 siteid = rdata->getSiteid();
	OmnString appname = rdata->getAppname();
	AosSessionObjPtr sdoc = rdata->getSession();
	AosXmlTagPtr root = rdata->getReceivedDoc();
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;
	
	
	u64 userid = 0;

	// Retrieve the user account object
	AosXmlTagPtr userobj = udoc;
	if (!userobj) 
	{
		if (root)
		{
			userobj = root->getFirstChild("objdef");
		}

		if (!userobj || !(userobj = userobj->getFirstChild()))
		{
			AosSetErrorU(rdata, "missing_user_obj") << enderr;
			return false;
		}
	}
	userobj->setAttr(AOSTAG_SITEID, siteid);

	// Get user container. 
	OmnString usrctnr_objid;
	AosXmlTagPtr userctnr = getUserCtnr(siteid, userobj, usrctnr_objid,
			userid, rdata);
	if (!userctnr)
	{
		AosSetErrorU(rdata, "missing_user_acct_ctnr") << enderr;
		return false;
	}
	userobj->setAttr(AOSTAG_CTNR_USER, userobj->getAttrStr(AOSTAG_PARENTC));

	// Check whether it is allowed
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateUserAcct(userctnr, rdata))
	{
		// Cannot create, permission denied
		return false;
	}

	if (sdoc) userid = sdoc->getUserid();

	OmnString cid = userobj->getAttrStr(AOSTAG_CLOUDID);
	if (cid == "")
	{
		// Create CloudID
		cid = getNewCid(siteid, rdata);
		aos_assert_r(cid != "", false);
		userobj->setAttr(AOSTAG_CLOUDID, cid);
	}

	// Check username
	OmnString username = userobj->getAttrStr(AOSTAG_USERNAME);
	if (username == "") username = cid;
	OmnString unameprefix = userobj->getAttrStr(AOSTAG_UNAMEPREFIX);
	if (unameprefix != "")
	{
		unameprefix << username;
		username = unameprefix;
	}
	// Chen Ding, 09/29/2010
	userobj->setAttr(AOSTAG_USERNAME, username);
 
	// Check password
	bool exist;
	OmnString passwd_path = AOSTAG_PASSWD;
	passwd_path << "/_$text";
	OmnString passwd = userobj->xpathQuery(passwd_path, exist, "");
	if (passwd == "")
	{
		errmsg = "Missing password!";
		return false;
		rdata->setError() << errmsg;
	}
	
	//Linda 2010/10/16
	// Check emails
	OmnString email = userobj->getAttrStr(AOSTAG_EMAIL);
	if (email != "")
	{
		// Verify the email.
		OmnString email2 = userobj->getAttrStr(AOSTAG_EMAIL1);
		if (email2 != "")
		{
			if (email != email2)
			{
				errmsg = "Email mismatch";
				return false;
				rdata->setError() << errmsg;
			}
			userobj->removeNode(AOSTAG_EMAIL1, false, false);
		}
	}

	mLock->lock();

	// Chen Ding, 11/20/2010
	// tmp
	int idx = sgCrtEmail-1;
	if (idx < 0) idx = sgMaxEmails-1;
	for (int i=0; i<sgMaxEmails; i++)
	{
		if (email == sgEmails[idx])
		{
			// The email is not unique
			mLock->unlock();
			errcode = eAosXmlInt_Ok;
			rdata->setOk();
			return true;
		}
		idx--;
		if (idx < 0) idx = sgMaxEmails-1;
	}

	// Check whether the user already exists.
	bool duplicate;
	//u64 id = AosIILClientSelf->getCtnrMemDocid(siteid, usrctnr_objid, AOSTAG_USERNAME, username, duplicate, rdata);
	u64 id = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, usrctnr_objid, AOSTAG_USERNAME, username, duplicate, rdata);
	
	AosXmlTagPtr xml = AosDocClientObj::getDocClient()->getDocByDocid(id, rdata);
	if (xml) 
	{
		mLock->unlock();
		errcode = eAosXmlInt_General;
		errmsg = "User already exists!";
		rdata->setError() << errmsg;
		return false;
	}

	// Chen Ding, 09/29/2010
	// Check whether the cid has been used by others.
	//id = AosIILClientSelf->getCtnrMemDocid(siteid, usrctnr_objid, AOSTAG_CLOUDID, cid, duplicate, rdata);
	id = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, usrctnr_objid, AOSTAG_CLOUDID, cid, duplicate, rdata);
	AosXmlTagPtr xml1 = AosDocClientObj::getDocClient()->getDocByDocid(id, rdata); 
	if (xml1)
	{
		// The cid has been used by someone else
		 errmsg = "Cloud ID was used by someone else: ";
		 errmsg << cid;
		 rdata->setError() << errmsg;
		 return false;
	}

	// Check whether the requester has the permission
	//--Ketty 
	//if (!AosSecurityMgrSelf->checkCreateUserAcct(siteid, appname, 
	//	sdoc, userctnr, errcode, errmsg))
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateUserAcct(userctnr, rdata))
	{
	 	// Not allowed
	 	mLock->unlock();
	 	return false;
	}

	// Create the objid
	OmnString objid = AosObjid::compose(AOSTAG_USERPREFIX, cid);

	// Reserved the objid
	u64 docid = 0;
	//bool rslt = AosDocClient::getSelf()->reserveObjid1(rdata, objid, docid, true);
	bool objid_changed = false;
OmnScreen << "To bind: " << objid << ":" << docid << endl;
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		objid, docid, objid_changed, true, false, rdata);
	if (userid == 0) userid = docid;

	// Create user home folder
	OmnString home_objid;
	//--Ketty
	//if (!createHomeFolder(siteid, userid, cid, userobj, home_objid, errcode, errmsg)) 
	if (!createHomeFolder(rdata, cid, userobj, home_objid)) 
	{
		mLock->unlock();
		return false;
	}

	OmnString realname = userobj->getAttrStr(AOSTAG_REALNAME);
	if (realname == "") userobj->setAttr(AOSTAG_REALNAME, cid);


	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(docid != AOS_INVDID, mLock, false);
	userobj->setAttr(AOSTAG_OBJID, objid);
	userobj->setAttr(AOSTAG_DOCID, docid);

	rslt = AosDocClientObj::getDocClient()->bindCloudid(cid, docid, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	createUserDirs(siteid, userctnr->getAttrU64(AOSTAG_DOCID, 0), userobj);

	// Create user home folder
	// OmnString uhobjid;
	// if (!createUserHomefolder1(userobj, userid, cid, uhobjid, errcode, errmsg))
	// {
	// 	mLock->unlock();
	// 	return false;
	// }

	// Create the home vpd, if needed
	OmnString hpvpd;
	createHomeVpd1(siteid, usrctnr_objid, hpvpd, cid, userobj, errcode, errmsg);

	// Create the user account
	aos_assert_r(rslt, false);

	// Create user folders
	OmnString blog_objid, album_objid, cmt_objid, rtcl_objid; 
	OmnString publish_objid, diary_objid;

	createUserCtnr(rdata, AOSTAG_CTNR_BLOG,  AOSOBJIDRES_USER_BLOG,  AOSSTYPE_HMFD_USRBLOG, 
			AosObjname::composeBlogCtnrName(), blog_objid, home_objid, userobj, cid);
	createUserCtnr(rdata, AOSTAG_CTNR_ALBUM, AOSOBJIDRES_USER_ALBUM, AOSSTYPE_HMFD_USRALBUM,
			AosObjname::composeAlbumCtnrName(), album_objid, home_objid, userobj, cid);
	createUserCtnr(rdata, AOSTAG_CTNR_CMT,   AOSOBJIDRES_USER_CMT,   AOSSTYPE_HMFD_USRCMT,
			AosObjname::composeCommentCtnrName(), cmt_objid, home_objid, userobj, cid);
	createUserCtnr(rdata, AOSTAG_CTNR_ATCL,  AOSOBJIDRES_USER_ATCL,  AOSSTYPE_HMFD_USRATCL,
			AosObjname::composeArticleCtnrName(), rtcl_objid, home_objid, userobj, cid);
	createUserCtnr(rdata, AOSTAG_CTNR_PUBLISH,  AOSOBJIDRES_USER_PUBLISH,  AOSSTYPE_HMFD_PUBLISH, 
			AosObjname::composePublishCtnrName(), publish_objid, home_objid, userobj, cid);
	createUserCtnr(rdata, AOSTAG_CTNR_DIARY, AOSOBJIDRES_USER_DIARY, AOSSTYPE_HMFD_DIARY, 
			AosObjname::composeDiaryCtnrName(), diary_objid, home_objid, userobj, cid);
	OmnString mblog_objid;
	createUserCtnr(rdata, AOSTAG_CTNR_MBLOG, AOSOBJIDRES_USER_MBLOG, AOSSTYPE_HMFD_USRMBLOG, 
			AosObjname::composeMblogCtnrName(), mblog_objid, home_objid, userobj, cid);

	rslt = createUserObj(root, siteid, userobj, userid, cid, username, errcode, errmsg);

	// Log the action
	addCreateAcctEntry(siteid, usrctnr_objid, username, userobj);

	// 	<resp username="xxx"
	// 		  hpvpd="xxx"
	// 		  zky_homctnr="xxx"
	// 		  objid="xxx"
	// 		  userblog_ctnr="xxx"
	// 		  useralbum_ctnr="xxx"
	// 		  usercmt_ctnr="xxx"
	// 		  />
	errcode = eAosXmlInt_Ok;
	resp = "<Contents ";
	resp << AOSTAG_USERNAME << "=\"" << username << "\" "
		<< AOSTAG_PARENTC << "=\"" << usrctnr_objid << "\" "
		<< AOSTAG_OBJID << "=\"" << userobj->getAttrStr(AOSTAG_OBJID) << "\" "
		<< AOSTAG_CTNR_HOME << "=\"" << home_objid << "\" "
		<< AOSTAG_CTNR_BLOG << "=\"" << blog_objid << "\" "
		<< AOSTAG_CTNR_ALBUM << "=\"" << album_objid << "\" "
		<< AOSTAG_CTNR_CMT << "=\"" << cmt_objid << "\" "
		<< AOSTAG_CTNR_MBLOG << "=\"" << mblog_objid << "\" "	// Ken, 01/05/2011
		<< AOSTAG_CTNR_ATCL << "=\"" << rtcl_objid << "\" "
		<< AOSTAG_CTNR_PUBLISH << "=\"" << publish_objid << "\" "
		<< AOSTAG_CTNR_DIARY << "=\"" << diary_objid << "\" ";
	if (hpvpd != "") resp << AOSTAG_HPVPD << "=\"" << hpvpd << "\" ";
	OmnString userrole = userobj->getAttrStr(AOSTAG_USER_ROLE);
	if (userrole != "") resp << AOSTAG_USER_ROLE << "=\"" << userrole << "\"";
	OmnString accttype = userobj->getAttrStr(AOSTAG_ACCT_TYPE);
	if (accttype != "") resp << AOSTAG_ACCT_TYPE << "=\"" << accttype << "\"";
	resp << "/>";

	// Need to send email
	//Linda 2010/10/16
	rdata->setOk();
	if (email!="")
	{
		OmnString subject = "äºè¯­è¨ä¼åæ³¨å";
		OmnString contents = "æ¬¢è¿æ¨å å¥äºè¯­è¨ç¤¾åº.\n    æ¨çç¨æ·å?";
		contents << username <<"  äºå·:"
	    	     << cid << "  å¯ç :"
	        	 <<passwd <<"\n    ç¹å»æ¿æ´? http://www.zykie.com";
		AosPyemail pc;
		OmnString emailstr="<sendemail ";
		emailstr << "rmailaddr =\"" << email <<"\" "
	    	     << "subject =\"" << subject <<"\" "
		 		 << "contents =\"" << contents <<"\" />";
		AosXmlParser parser;
		AosXmlTagPtr emailxml = parser.parse(emailstr, "" AosMemoryCheckerArgs);
		pc.sendmail(emailxml, rdata);
	}
	return true;
}


AosXmlTagPtr
AosUserMgr::getUserCtnr(
		const u32 siteid,
		const AosXmlTagPtr &userobj, 
		OmnString &ctnr_objid,
		const u64 &userid,
		const AosRundataPtr &rdata)
{
	// It retrieves/creates the user container. The user container objid
	// is specified by the attribute AOSTAG_PARENTC. Or in other
	// word, if a container is created for managing user accounts, that
	// container must be a public container. If the user doc did not 
	// specify AOSTAG_PARENTC, it is an error.
	ctnr_objid = userobj->getPrimaryContainer();
	if (ctnr_objid == "") 
	{
		rdata->setError() << "Missing user account container. User objid: "
			<< userobj->getAttrStr(AOSTAG_OBJID) 
			<< ". Docid: " << userobj->getAttrStr(AOSTAG_DOCID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);	
}


bool
AosUserMgr::createHomeFolder(
		const AosRundataPtr &rdata,
		const OmnString &usr_cid,
		const AosXmlTagPtr &userobj, 
		OmnString &pctnr_objid)
{
	u32 siteid = rdata->getSiteid();
	//--Ketty
	//AosObjMgr::getSelf()->createCidCtnr(siteid);
	AosObjMgr::getSelf()->createCidCtnr(rdata);

	// Creates user's home folder, whose name is:
	// 	zcid_cidnnn
	// Chen Ding, 12/20/2010
	// pctnr_objid = AosObjid::composeCidRootCtnrObjid(usr_cid);
	pctnr_objid = AosObjid::composeUserHomeCtnrObjid(usr_cid);
	
	// Create the parent folder
	OmnString docstr = "<ctnr ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid 
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_USER_PFOLDER
		<< "\" " << AOSTAG_OBJID << "=\"" << pctnr_objid 
		<< "\" " << AOSTAG_CTNR_PUBLIC << "=\"" << "true"
		<< "\" " << AOSTAG_PARENTC << "=\"" << AOSCTNR_CID
		<< "\"/>";
	//--Ketty
	//AosXmlTagPtr doc = AosDocServerSelf->createDocSafe1(
	//	docstr, siteid, userid, AOSAPPNAME_SYSTEM, "", "", 
	//	true, false, errcode, errmsg, false, false, true, true, ttl);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr, "", "", true, false, false, false, true, true);
	if (!doc) return false;
	pctnr_objid = doc->getAttrStr(AOSTAG_OBJID);

	// Create the directory
	// OmnString cmd = "mkdir ";
	// cmd << mUserBaseDir << "/" << siteid << "/" 
	//	<< doc->getAttrStr(AOSTAG_DOCID, 0);
	//system(cmd);
	getUserBaseDir(siteid, doc->getAttrU64(AOSTAG_DOCID, 0));
	userobj->setAttr(AOSTAG_CTNR_HOME, pctnr_objid);
	rdata->setOk();
	return true;
}


bool
AosUserMgr::createUserDirs(
		const u32 siteid,
		const u64 &ctnr_docid, 
		const AosXmlTagPtr &userobj)
{
	// The following directories are created:
	// 	<root>/<container_docid>/<user_docid>/home
	// 	<root>/<container_docid>/<user_docid>/images
	//
	// Create the root dirrectory
	// OmnString rootdir = mUserBaseDir;
	// rootdir << "/" << siteid << "/" << usrctnr->getAttrStr(AOSTAG_DOCID) << "/" 
	// 	<< userobj->getAttrStr(AOSTAG_DOCID);
	// OmnString syscmd = "mkdir ";
	// syscmd << rootdir;
	// system(syscmd);
	OmnString rootdir = getUserBaseDir(siteid, ctnr_docid);
	OmnString cid = userobj->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_r(cid != "", false);
	rootdir << "/" << cid;
	OmnString syscmd = "mkdir ";
	syscmd << rootdir;
	system(syscmd.data());

	// Create the home directory
	syscmd = "mkdir ";
	OmnString dir = rootdir;
	dir << "/home";
	syscmd << dir;
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_HOMEDIR, dir);

	// Create the image directory
	syscmd = "mkdir ";
	dir = rootdir;
	dir << "/images";
	syscmd << dir;
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_IMGDIR, dir);

	// Create the resource directory
	syscmd = "mkdir ";
	dir = rootdir;
	dir << "/rsc";
	syscmd << dir;
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_RSCDIR, dir);
	return true;	
}


bool
AosUserMgr::createHomeVpd1(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		OmnString &hpvpd,
		const OmnString &user_cid,
		const AosXmlTagPtr &userobj,
		AosXmlRc &errcode,
		OmnString &errmsg)
{
	/*
	// Check whether it needs to create a default vpd for the user.
	hpvpd = "";
	AosXmlTagPtr ctnrxml = AosDocServerSelf->getDoc(siteid, ctnr_objid);
	if (!ctnrxml)
	{
		// This should not happen!!!
		errmsg = "Internal error!";
		OmnAlarm << "Failed to retrieve the container: "
			<< ctnr_objid << enderr;
		return false;
	}

	OmnString dftvpd_policy = ctnrxml->getAttrStr(AOSTAG_DFTVPD_POLIGY);
	if (dftvpd_policy == "")
	{
		// Check whether a template has been specified by the object.
		OmnString vpdtmpl = userobj->getAttrStr(AOSTAG_VPDTMPLNAME);
		if (vpdtmpl == "") return true;

		// Create the vpd
		AosXmlTagPtr vpd = AosDocServer::getSelf()->getDoc(siteid, vpdtmpl);
		aos_assert_r(vpd, false);

		OmnString cid = userobj->getAttrStr(AOSTAG_CLOUDID);
		if (cid == "") cid = userobj->getAttrStr(AOSTAG_USERNAME);

		OmnString namebase = userobj->getAttrStr(AOSTAG_VPD_NAMEBASE);
		if (namebase == "") namebase = vpdtmpl;
		hpvpd = AosObjid::compose(namebase, cid);
		vpd->setAttr(AOSTAG_OBJID, hpvpd);
		AosXmlRc errcode;
		OmnString errmsg;
		int ttl = 0;
		AosXmlTagPtr cloned_doc = AosDocServerSelf->cloneDoc(siteid, AOSAPPNAME_SYSTEM, 
				userobj->getAttrU64(AOSTAG_DOCID, AOS_INVDID), cid, vpdtmpl, 
				hpvpd, true, errcode, errmsg, ttl);
		if (!cloned_doc)
		{
			OmnAlarm << "Failed to clone!" << enderr;
			return false;
		}
		hpvpd = cloned_doc->getAttrStr(AOSTAG_OBJID);
		userobj->setAttr(AOSTAG_HPVPD, hpvpd);
		return true;
	}

	if (dftvpd_policy == "by_sex")
	{
		hpvpd = getDftHpvpdBySex1(siteid, userobj->getAttrU64(AOSTAG_DOCID, AOS_INVDID), 
				user_cid, userobj, ctnrxml);
		if (hpvpd != "")
		{
			// A new vpd has been associated with the user. Need to
			// store it into the user's data. 
			userobj->setAttr(AOSTAG_HPVPD, hpvpd);
		}
		return true;
	}

	OmnNotImplementedYet;
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


//-Ketty
/*bool
AosUserMgr::createUserCtnr(
		const OmnString &ctnr_aname,
		const OmnString &ctnr_name,
		const OmnString &subtype,
		const OmnString &objname,
		OmnString &ctnr_objid,
		const OmnString &userhome_objid ,
		const AosXmlTagPtr &userdoc, 
		const OmnString &cid, 
		const u64 &requester_id)*/
bool
AosUserMgr::createUserCtnr(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_aname,
		const OmnString &ctnr_name,
		const OmnString &subtype,
		const OmnString &objname,
		OmnString &ctnr_objid,
		const OmnString &userhome_objid ,
		const AosXmlTagPtr &userdoc, 
		const OmnString &cid) 
{
	// Create the user's subfolder. Each sub-folder is defined by an object, 
	// which is to be created by this function. The sub-folder is a private
	// folder, which means that its objid must be in the form:
	// 		AOSZTG_PRICONTAINER + "_" + <user_docid> + "_" + <folder name>
	// The sub-folder's parent container is 'userhome_objid', which should
	// be treated as a public folder. 
	//
	ctnr_objid = AosObjid::compose(ctnr_name, cid);
	OmnString docstr = "<userfd ";
	u32 siteid = userdoc->getAttrU32(AOSTAG_SITEID, 0);
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << subtype
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_OBJNAME << "=\"" << objname
		<< "\" " << AOSTAG_PARENTC << "=\"" << userhome_objid 
		<< "\" " << AOSTAG_OBJID << "=\"" << ctnr_objid 
		<< "\"/>";

	//AosXmlRc errcode; 
	//OmnString errmsg;
	//--Ketty
	/*AosXmlTagPtr doc = AosDocServerSelf->createDocSafe1(
			docstr, siteid, requester_id, cid, AOSAPPNAME_SYSTEM, "",
			true, true, errcode, errmsg, false, false, false, true, ttl);*/
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, cid, "",
			true, true, false, false, false, true);
	aos_assert_r(doc, false);
	ctnr_objid = doc->getAttrStr(AOSTAG_OBJID);
	userdoc->setAttr(ctnr_aname, ctnr_objid);
	return true;
}


bool
AosUserMgr::createUserObj(
		const AosXmlTagPtr &root,
		const u32 siteid,
		const AosXmlTagPtr &userobj, 
		const u64 &userid, 
		const OmnString &cid,
		const OmnString &username,
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	/*
	// Remove the extra password
	userobj->removeNode("zky_passwd1", false, false);
	userobj->removeNode("passwd1", false, false);
	userobj->setAttr(AOSTAG_CTNR_PUBLIC, "true");

	// If AOSTAG_OBJNAME is empty, it uses AOSTAG_REALNAME. If there is
	// no AOSTAG_REALNAME, use AOSTAG_USERNAME
	OmnString objnm = userobj->getAttrStr(AOSTAG_OBJNAME);
	if (objnm == "")
	{
		objnm = userobj->getAttrStr(AOSTAG_REALNAME);
		if (objnm == "") objnm = username;
	}

	// Removing the unwanted attributes
	userobj->removeAttr(AOSTAG_VPDTMPLNAME, false, true);
	userobj->removeAttr(AOSTAG_VPD_NAMEBASE, false, true);
	userobj->removeAttr(AOSTAG_UNAMEPREFIX, false, true);

	userobj->setAttr(AOSTAG_OBJNAME, objnm);
	userobj->setAttr(AOSTAG_SITEID, siteid);
	userobj->setAttr(AOSTAG_OTYPE, AOSOTYPE_USERACCT);
	OmnString acct_status = userobj->getAttrStr(AOSTAG_USERSTATUS);
	if (acct_status == "") userobj->setAttr(AOSTAG_USERSTATUS, AOSUSRSTATUS_ACTIVE);

	OmnString usertype = userobj->getAttrStr(AOSTAG_USERTYPE);
	if (usertype == "") userobj->setAttr(AOSTAG_USERTYPE, AOSUSERTYPE_NORM);

	int ttl = 0;
	bool rslt = AosDocServerSelf->createDocSafe1(
		root, userobj, siteid, userid, cid, AOSAPPNAME_SYSTEM, "", 
		true, false, errcode, errmsg, true, true, true, true, ttl);
	aos_assert_r(rslt, false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


void
AosUserMgr::addCreateAcctEntry(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		const OmnString &username,
		const AosXmlTagPtr &userdoc)
{
	// 	<login
	// 		AOSTAG_OTYPE=AOSOTYPE_LOG
	// 		AOSTAG_STYPE=AOSLOGTYPE_CREATE_USER
	// 		AOSTAG_SITEID="xxx"
	// 		AOSTAG_USERNAME="xxx"
	// 		AOSTAG_USERID="xxx"
	// 		AOSTAG_CTIME="yyy"
	// 		AOSTAG_STATUS=status>msg</login>
	OmnString logentry = "<ctacct ";
	logentry << AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSLOGTYPE_CREATE_USER
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_USERNAME << "=\"" << username;
	if (userdoc)
	{
		logentry << "\" " << AOSTAG_USEROBJID << "=\"" << userdoc->getAttrStr(AOSTAG_OBJID)
			<< "\" " << AOSTAG_USERID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID);
	}
	logentry << "\" " << AOSTAG_CTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale()) 
		<< "\"/>";
}


OmnString
AosUserMgr::getUserBaseDir(
		const u32 siteid, 
		const u64 &docid)
{
	aos_assert_r(docid != AOS_INVDID, "");
	OmnString dir = mUserBaseDir;
	dir << "/" << siteid;
	OmnString cmd = "mkdir ";
	cmd << dir;
	system(cmd.data());

	dir << "/" << docid;
	cmd = "mkdir ";
	cmd << dir;
	system(cmd.data());
	return dir;
}


//--Ketty
/*OmnString
AosUserMgr::getDftHpvpdBySex1(
		const u32 siteid,
		const u64 &user_docid,
		const OmnString &user_cid,
		const AosXmlTagPtr &xml, 
		const AosXmlTagPtr &ctnrxml)*/
OmnString
AosUserMgr::getDftHpvpdBySex1(
		const AosRundataPtr &rdata,
		const OmnString &user_cid,
		const AosXmlTagPtr &xml, 
		const AosXmlTagPtr &ctnrxml)
{
	// Homepage is required and it is created based on user sex.
	OmnString sex = xml->getAttrStr(AOSTAG_SEX);
	if (sex != "") 
	{
		// The container should be in the form:
		// 	<container ...>
		// 		<zky_dftvpds>
		// 			<record sex="xxx" vpdname="xxx"/>
		// 			<record sex="xxx" vpdname="xxx"/>
		// 			...
		// 		</zky_dftvpds>
		// 		...
		// 	</container>
		AosXmlTagPtr vpds = ctnrxml->getFirstChild(AOSTAG_DFTVPDS);
		if (vpds)
		{
			AosXmlTagPtr record = vpds->getFirstChild();
			while (record)
			{
				if (sex == record->getAttrStr("sex"))
				{
					OmnString hpvpd = record->getAttrStr("vpdname");
					return checkCreateNewHpvpd1(rdata, hpvpd, ctnrxml, user_cid);
				}

				record = vpds->getNextChild();
			}
		}
	}

	OmnString hpvpd = ctnrxml->getAttrStr(AOSTAG_DFTHPVPD);
	return checkCreateNewHpvpd1(rdata, hpvpd, ctnrxml, user_cid);
}


//--Ketty
/*OmnString
AosUserMgr::checkCreateNewHpvpd1(
		const u32 siteid,
		const u64 &user_docid,
		const OmnString &hpvpd, 
		const AosXmlTagPtr &ctnrxml, 
		const OmnString &user_cid)*/
OmnString
AosUserMgr::checkCreateNewHpvpd1(
		const AosRundataPtr &rdata,
		const OmnString &hpvpd, 
		const AosXmlTagPtr &ctnrxml, 
		const OmnString &user_cid)
{
	// This function checks whether it needs to clone the vpd 
	// identified by 'hpvpd'. If 'hpvpd' is empty, it does nothing.
	// Otherwise, it checks whether 'ctnrxml' wants to create 
	// an instance. If no, it does nothing. Otherwise, it checks
	// whether the vpd 'hpvpd' + '_' + username exists. If yes, 
	// it does nothing. Otherwise, it will clone the vpd and
	// set the objid of the newly created vpd 'hpvpd' + '_' + username.
	u32 siteid = rdata->getSiteid();

	if (hpvpd == "") return "";
	
	// The container defines a default homepage VPD. Check whether
	// an instance should be created for it.
	if (ctnrxml->getAttrStr(AOSTAG_NEW_DFTHPVPD) != "true")
	{
		// No need to create new instance. 
		return hpvpd;
	}

	// It does need to create a new instance. Check whether
	// the new instance has been created. The new instance 
	// objid should be:
	OmnString oid = AosObjid::compose(AOSOBJIDRES_HOMEVPD, user_cid);
	if (AosDocClientObj::getDocClient()->doesObjidExist(siteid, oid, rdata) == AOS_INVDID)
	{
		// It does not exist. Create one.
		//AosXmlRc errcode;
		//OmnString errmsg;
		//--Ketty
		//AosXmlTagPtr cloned_doc = AosDocServerSelf->cloneDoc(siteid, AOSAPPNAME_SYSTEM, 
		//		user_docid, user_cid, hpvpd, oid, true, errcode, errmsg, ttl);
		AosXmlTagPtr cloned_doc = AosDocClientObj::getDocClient()->cloneDoc(rdata,
				user_cid, hpvpd, oid, true);
		if (!cloned_doc)
		{
			return hpvpd;
		}
		return cloned_doc->getAttrStr(AOSTAG_OBJID);
	}
			
	// Already exist. No need to create.
	return oid;
}
