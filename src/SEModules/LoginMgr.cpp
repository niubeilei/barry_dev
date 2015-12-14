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
// 	User accounts are managed through a container. All objects in the 
// 	container are user records. There is one attribute in all the objects
// 	that is used as the 'username'. If not specified, the default 
// 	AOSTAG_USERNAME is assumed. When checking the
// 	logins, we need to know the container, the user name, and the
// 	passwod. This means that we need to find the record:
// 		AOSZTG_SITEID + siteid and
// 		AOSZTG_USERNAME + username
//
// Modification History:
// 01/31/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEModules/LoginMgr.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "SEInterfaces/DocClientObj.h"
#include "MultiLang/LangTermIds.h"
#include "Porting/GetTime.h"
#include "Rundata/RdataUtil.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/Docid.h"
#include "SEUtil/Objid.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/SiteMgr.h"
#include "SEModules/ImgProc.h"
#include "SEModules/DftVpdPolicy.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SmartDoc/SMDMgr.h"
#include "SEServer/SeReqProc.h"
#include "Security/Session.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "Util/StrSplit.h"
#include "Util/ValueRslt.h"
#include "SEBase/SecUtil.h"
//#include "Python/Pyemail.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/AccessRcd.h"
#include "SEInterfaces/IILClientObj.h"
#include "API/AosApi.h"
#include "IdGen/U64IdGen.h"


OmnMutexPtr	AosLoginMgr::mLock = OmnNew OmnMutex();

static OmnString 		sgCidGenName = "cidmgr";
static OmnString		sgDftCidPrefix = "";
static OmnString		sgCidPrefix = sgDftCidPrefix;
static AosU64IdGenPtr	sgCidMgr;
OmnString		AosLoginMgr::mUserBaseDir;


// Chen Ding, 11/20/2010
// Temporary
// const int sgMaxEmails = 10;
// static OmnString 	sgEmails[sgMaxEmails];
// static int			sgCrtEmail = 0;

const OmnString sgDftUserBaseDir = "User";
const OmnString AOSCONFIG_CID_PREFIX = "cid_prefix";

AosLoginMgr::AosLoginMgr(const AosXmlTagPtr &config, const bool)
{
	// Ketty 2013/05/16
	//mUserBaseDir = config->getAttrStr(AOSCONFIG_USER_BASEDIR, sgDftUserBaseDir);
	OmnString base_dir = config->getAttrStr(AOSCONFIG_USER_BASEDIR, sgDftUserBaseDir);
	mUserBaseDir = OmnApp::getAppBaseDir();
	mUserBaseDir << base_dir;
	aos_assert(mUserBaseDir != "");

	if (mUserBaseDir.data()[mUserBaseDir.length()-1] == '/')
	{
		mUserBaseDir = mUserBaseDir.substr(0, mUserBaseDir.length()-1);
	}
	sgCidPrefix = config->getAttrStr(AOSCONFIG_CID_PREFIX, sgCidPrefix);
}

bool
AosLoginMgr::checkLogin(
		OmnString &errmsg,
		AosXmlTagPtr &userdoc,
		const OmnString &cid,
		const OmnString &pwd,
		const OmnString &ctnr,
		const AosRundataPtr &rdata)
{
	//cid may be username or cloudid
	if (cid == "")
	{
		errmsg = "missing cloudid";	
		return false;
	}
	if (pwd == "")
	{
		errmsg = "missing password";
		return false;
	}
	if (ctnr == "")
	{
		errmsg = "missing user container";
		return false;
	}
	u32 siteid = rdata->getSiteid();
	userdoc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
	if (!userdoc)
	{
		bool duplicate = false;
		u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr, AOSTAG_USERNAME, 
				cid, AOSTAG_OTYPE, AOSOTYPE_USERACCT, duplicate, rdata);

		if (docid)
		{
			userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
		}

		if (!userdoc)
		{
			docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr, AOSTAG_CLOUDID, 
					cid, AOSTAG_OTYPE, AOSOTYPE_USERACCT, duplicate, rdata);
			if (docid)
			{
				userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
			}
		}
	}

	if (!userdoc)
	{
		errmsg = "user not found";
		return false;
	}

	OmnString username = userdoc->getAttrStr(AOSTAG_USERNAME);
	OmnString cloudid = userdoc->getAttrStr(AOSTAG_CLOUDID);

	int level = 0;
	AosXmlRc errcode = eAosXmlInt_General;
	bool rslt = verifyPassword(siteid, ctnr, cloudid, username, pwd, userdoc, level, errcode, errmsg, rdata); 
	if (!rslt)
	{
		errmsg = "access denied";
		return false;
	}
	return true;
}


bool
AosLoginMgr::checkLogin(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &cmd,
		OmnString &hpvpd, 
		OmnString &workvpd,
		OmnString &publicvpd, 
		OmnString &familyvpd)
{
	// 'cmd' is an XML:
	// 	<cmd container="xxx"
	// 		AOSTAG_CLOUDID="xxx"
	// 		AOSTAG_LEVEL="xxx"
	// 		AOSTAG_USERNAME="xxx">
	// 		<AOSTAG_PASSWD>
	// 		
	u32 siteid = rdata->getSiteid();
	AosXmlRc errcode = eAosXmlInt_General;
	OmnString errmsg;

	// This function checks whether the login credentials
	// are correct. If 'ctnr_objid' is defined, the user information
	// is defined in that container. Otherwise, it is defined 
	// at the site level. 'siteid' is mandatory. 
	OmnString ctnr_objid= cmd->getAttrStr("container");
	if (ctnr_objid== "")
	{
		AosSetErrorU(rdata, "missing_container") << enderr;
		return false;
	}

	// Check whether there is the CloudID
	OmnString cid = cmd->getAttrStr(AOSTAG_CLOUDID);
	OmnString username = cmd->getAttrStr(AOSTAG_USERNAME);
	if (cid == "")
		cid = username;
	if (cid == "")
	{
		OmnAlarm << "missing cid" << enderr;
		/*
		bool duplicate = false;
		u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr_objid, 
			AOSTAG_CLOUDID, cid, 
			AOSTAG_OTYPE, AOSOTYPE_USERACCT,
			duplicate, rdata);
		userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
		*/
	}
	userdoc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);

	if (!userdoc)
	{
		if (username == "")
		{
			errmsg = "Missing username!";
			addLoginEntry(siteid, ctnr_objid, AOSLOGIN_MISS_UNAME, errmsg, rdata);
			AosSetErrorU(rdata, "missing_username") << enderr;
			return false;
		}

		bool duplicate = false;
		//u64 docid = AosIILClientSelf->getCtnrMemDocid(siteid, ctnr_objid, 
		u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr_objid, 
			AOSTAG_USERNAME, username, 
			AOSTAG_OTYPE, AOSOTYPE_USERACCT,
			duplicate, rdata);

		if (docid)
		{
			userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
		}

		if (!userdoc)
		{
			//docid = AosIILClientSelf->getCtnrMemDocid(siteid, ctnr_objid, 
			docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr_objid, 
				AOSTAG_CLOUDID, username, 
				AOSTAG_OTYPE, AOSOTYPE_USERACCT,
				duplicate, rdata);
			if (docid)
			{
				userdoc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
			}
		}
	}

	if (!userdoc)
	{
		errmsg = "User not found!";
		addLoginEntry(siteid, ctnr_objid, cid, username, 0, 
				AOSLOGIN_USER_NOTFOUND, errmsg, rdata);
		AosSetErrorU(rdata, "user_not_found") << enderr;
		return false;
	}

	username = userdoc->getAttrStr(AOSTAG_USERNAME);
	cid = userdoc->getAttrStr(AOSTAG_CLOUDID);
//	if (cid == "")
//	{
//		errmsg = "Missing Cloud ID";
//		addLoginEntry(siteid, ctnr_objid, cid, username, 0, 
//				AOSLOGIN_MISS_CID, errmsg);
//		return false;
//	}

	if (username == "") username = userdoc->getAttrStr(AOSTAG_USERNAME);

	AosXmlTagPtr passwdnode = cmd->getFirstChild(AOSTAG_PASSWD);
	OmnString passwd;
	if (!passwdnode || (passwd = passwdnode->getNodeText()) == "")
	{
		errmsg = "Missing password!";
		// OmnAlarm << errmsg << enderr;
		addLoginEntry(siteid, ctnr_objid, cid, username, 0, 
				AOSLOGIN_MISS_PASSWORD, errmsg, rdata);
		AosSetErrorU(rdata, "missing_password") << enderr;
		return false;
	}

	int level = cmd->getAttrInt(AOSTAG_LEVEL, 0);
	if (!verifyPassword(siteid, ctnr_objid, 
			cid, username, passwd, userdoc, level, errcode, errmsg, rdata)) 
	{
		addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
			AOSLOGIN_PASSWD_NOT_MATCH, errmsg, rdata);
		return false;
	}


	if (ctnr_objid != userdoc->getAttrStr(AOSTAG_PARENTC, ""))
	{
		AosSetErrorU(rdata, "missing_ctnr") << enderr;
		return false;
	}

	OmnString logentry = "<login ";
	logentry << AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid << "_log"
		<< "\" " << AOSTAG_USERNAME << "=\"" << userdoc->getAttrStr(AOSTAG_USERNAME)
		<< "\" " << AOSTAG_USERID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID)
		<< "\" " << AOSTAG_CLOUDID << "=\"" << cid
		<< "\" " << AOSTAG_CTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale())
		<< "\" " << AOSTAG_STATUS << "=\"";

	// LogId: it is in the form: AOSLOGID_LOGIN + "_" + ctnr_objid 
	OmnString logid = ctnr_objid;
	logid << "_login";

	bool exist;
	OmnString status = userdoc->getAttrStr1(AOSTAG_USERSTATUS, exist);
	if (status != AOSUSRSTATUS_ACTIVE)
	{
		errmsg = "Account is not active!";
		logentry << "denied\"/>";
		addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
				AOSLOGIN_NOT_ACTIVATED, errmsg, rdata);
		AosSetErrorU(rdata, "accout_not_active") << enderr;
		return false;
	}
	
	//// Password is encoded in the subtag:
	//// 	<...>
	//// 		<zky_passwd>xxx</zky_passwd>
	//// 		...
	//// 	</...>
	//AosXmlTagPtr pwdnode = userdoc->getFirstChild(AOSTAG_PASSWD);
	//if (!pwdnode)
	//{
	//	errmsg = "Internal error: no password found in user record!";
	//	OmnAlarm << errmsg << enderr;
	//	logentry << "internal error: missing password\"/>";
	//	addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
	//		AOSERROR_INTERNAL, errmsg, rdata);
	//	rdata->setError(errcode) << errmsg;
	//	return false;
	//}
	//
	//OmnString pwd = pwdnode->getNodeText();
	//if (pwd != passwd)
	//{
	//	errmsg = "Password mismatch!";
	//	logentry << "password mismatch\"/>";
	//	addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
	//			AOSLOGIN_PASSWD_NOT_MATCH, errmsg, rdata);
	//	rdata->setError(errcode) << errmsg;
	//	return false;;
	//}

	logentry << "ok\"/>";
	addLoginEntry(siteid, ctnr_objid, cid, username, userdoc,
			AOSLOGIN_LOGIN_SUCCESS, "", rdata);

	// Retrieve the container
	AosXmlTagPtr ctnr_doc = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);

	// Check whether it needs to create a default vpd for the user.
	hpvpd = userdoc->getAttrStr(AOSTAG_HPVPD);
	workvpd = userdoc->getAttrStr(AOSTAG_WORK_VPD);
	publicvpd = userdoc->getAttrStr(AOSTAG_PUBLIC_VPD);
	familyvpd = userdoc->getAttrStr(AOSTAG_FAMILY_VPD);
	rdata->setCid(cid);
	rdata->setOk();
	return true;
}


OmnString
AosLoginMgr::getDftHpvpdBySex1(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def_tag,
		const AosXmlTagPtr &userdoc, 
		const AosXmlTagPtr &ctnrxml)
{
	// It retrieves the vpd name based on sex
	OmnString sex = userdoc->getAttrStr(AOSTAG_SEX);
	if (sex == "") return "";
	
	// The container should be in the form:
	// 	<container ...>
	// 		<zky_dftvpds>
	// 			<record sex="xxx" vpdname="xxx"/>
	// 			<record sex="xxx" vpdname="xxx"/>
	// 			...
	// 		</zky_dftvpds>
	// 		...
	// 	</container>
	// AosXmlTagPtr vpds = ctnrxml->getFirstChild(AOSTAG_DFTVPDS);
	aos_assert_rr(def_tag, rdata, "");
	AosXmlTagPtr vpds = def_tag->getFirstChild();
	if (vpds)
	{
		AosXmlTagPtr record = vpds->getFirstChild();
		while (record)
		{
			if (sex == record->getAttrStr("sex"))
			{
				return record->getAttrStr("vpdname");
				// return checkCreateNewHpvpd(rdata, hpvpd, ctnrxml, user_cid);
			}
			record = vpds->getNextChild();
		}
	}

	return "";
	// OmnString hpvpd = ctnrxml->getAttrStr(AOSTAG_DFTHPVPD);
	// return checkCreateNewHpvpd(rdata, hpvpd, ctnrxml, user_cid);
}


OmnString
AosLoginMgr::getUserRealname(
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &uname, 
		const AosRundataPtr &rdata)
{
	bool duplicate;
	//u64 docid = AosIILClientSelf->getCtnrMemDocid(siteid, ctnr_objid, 
	u64 docid = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, ctnr_objid, 
		AOSTAG_USERNAME, uname, duplicate, rdata);
	AosXmlTagPtr xml = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata); 
	if (!xml) return "";
	bool exist;
	return xml->getAttrStr1(AOSTAG_REALNAME, exist);
}


OmnString
AosLoginMgr::checkCreateNewHpvpd(
		const AosRundataPtr &rdata,
		const OmnString &hpvpd, 
		const AosXmlTagPtr &ctnr_doc, 
		const OmnString &user_cid)
{
	// This function checks whether it needs to clone the vpd 
	// identified by 'hpvpd'. If 'hpvpd' is empty, it does nothing.
	// Otherwise, it checks whether 'ctnr_doc' wants to create 
	// an instance. If no, it does nothing. Otherwise, it checks
	// whether the vpd 'hpvpd' + '_' + username exists. If yes, 
	// it does nothing. Otherwise, it will clone the vpd and
	// set the objid of the newly created vpd 'hpvpd' + '_' + username.
	u32 siteid = rdata->getSiteid();
	if (hpvpd == "") return "";
	
	// The container defines a default homepage VPD. Check whether
	// an instance should be created for it.
	if (ctnr_doc->getAttrStr(AOSTAG_NEW_DFTHPVPD) != "true")
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
		AosXmlTagPtr cloned_doc = AosDocClientObj::getDocClient()->cloneDoc(rdata, user_cid, hpvpd, oid, true);
		if (!cloned_doc)
		{
			return hpvpd;
		}
		return cloned_doc->getAttrStr(AOSTAG_OBJID);
	}
			
	// Already exist. No need to create.
	return oid;
}


bool 
AosLoginMgr::createUserAccount(
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
	AosSessionObjPtr sdoc = rdata->getSession();
	AosXmlTagPtr root = rdata->getReceivedDoc();
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
			rdata->setError() << "Missing the user object!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	userobj->setAttr(AOSTAG_SITEID, siteid);

	OmnString sdoc_emailobjid = userobj->getAttrStr(AOSTAG_SMARTDOCS_EMAIL, ""); 
	userobj->removeAttr(AOSTAG_SMARTDOCS_EMAIL);

	bool is_send_email = userobj->getAttrBool(AOSTAG_SENDMAIL_FLAG, true);
	userobj->removeAttr(AOSTAG_SENDMAIL_FLAG);

	// Get user container. 
	OmnString usrctnr_objid;
	AosXmlTagPtr userctnr = getUserCtnr(rdata, userobj, usrctnr_objid);
	if (!userctnr)
	{
		rdata->setError() << "Missing user account container!";
		return false;
	}
	userobj->setAttr(AOSTAG_CTNR_USER, userobj->getAttrStr(AOSTAG_PARENTC));

	// Check whether it is allowed
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateUserAcct(userctnr, rdata))
	{
		// Cannot create, permission denied
		return false;
	}

	// if (!sdoc)
	// {
	// 	errmsg = "Create user account denied!";
	// 	OmnAlarm << errmsg << enderr;
	// 	return false;
	// }

	if (sdoc) userid = sdoc->getUserid();
	// if (!userid)
	// {
	// 	errmsg = "Create user account denied (no userid)!";
	// 	OmnAlarm << errmsg << enderr;
	// 	return false;
	// }

	// Chen Ding, 03/29/2012
	// OmnString cid = userobj->getAttrStr(AOSTAG_CLOUDID);
	// if (cid == "")
	// {
	// Create CloudID
	OmnString cid = getNewCid(siteid, rdata);
	aos_assert_r(cid != "", false);
	userobj->setAttr(AOSTAG_CLOUDID, cid);

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
		rdata->setError() << "Missing password!";
		return false;
	}
	
	//Linda 2010/10/16
	// Check emails
	OmnString ismail =  userobj->getAttrStr("ismail");	
	userobj->removeAttr("ismail");
	OmnString email;
	if (ismail == "" || ismail == "true")
	{	
		email = userobj->getAttrStr(AOSTAG_EMAIL);
		if (email != "")
		{
			// Verify the email.
			OmnString email2 = userobj->getAttrStr(AOSTAG_EMAIL1);
			if (email2 != "")
			{
				if (email != email2)
				{
					rdata->setError() << "Email mismatch";
					return false;
				}
				userobj->removeNode(AOSTAG_EMAIL1, false, false);
			}
		}


		// Chen Ding, 11/20/2010
		// tmp
		// int idx = sgCrtEmail-1;
		// if (idx < 0) idx = sgMaxEmails-1;
		// for (int i=0; i<sgMaxEmails; i++)
		// {
		// 	if (email == sgEmails[idx])
		// 	{
		// 		// The email is not unique
		// 		mLock->unlock();
		// 		errcode = eAosXmlInt_Ok;
		// 		return true;
		// 	}
		// 	idx--;
		// 	if (idx < 0) idx = sgMaxEmails-1;
		// }
	}

	mLock->lock();

	// Check whether the user already exists.
	bool duplicate;
	OmnString errmsg;
	//u64 id = AosIILClientSelf->getCtnrMemDocid(siteid, usrctnr_objid, 
	u64 id = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, usrctnr_objid, 
		AOSTAG_USERNAME, username, duplicate, rdata);
	AosXmlTagPtr xml;
	if (id)
	{
		xml = AosDocClientObj::getDocClient()->getDocByDocid(id, rdata); 
	}
	if (xml) 
	{
		mLock->unlock();
		rdata->setError() << eAosXmlInt_General << ". User already exists!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Chen Ding, 09/29/2010
	// Check whether the cid has been used by others.
	// Chen Ding, 03/29/2012
	// We will not check cid anymore
	// id = AosIILClientObj::getIILClient()->getCtnrMemDocid(siteid, usrctnr_objid, 
	//	AOSTAG_CLOUDID, cid, duplicate, rdata);
	//AosXmlTagPtr xml1;
	// if (id)
	// {
	// 	xml1 = AosDocClientObj::getDocClient()->getDocByDocid(id, rdata); 
	// }
	// 
	// if (xml1)
	// {
	// 	// The cid has been used by someone else
	// 	 mLock->unlock();
	// 	 rdata->setError() << "Cloud ID was used by someone else: " << cid;
	// 	 return false;
	// }

	// Check whether the requester has the permission
	// felicia, 2015/08/31 for bug JIMODB-626
	//if (!!AosSecurityMgrObj::getSecurityMgr()->checkCreateUserAcct(userctnr, rdata))
	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateUserAcct(userctnr, rdata))
	{
	 	// Not allowed
	 	mLock->unlock();
	 	return false;
	}

	// Create the objid
	OmnString objid = AosObjid::composeUserAcctObjid(cid);

	// Reserved the objid
	u64 docid = 0;
	bool objid_changed;
	OmnScreen << "To bind: " << objid << ":" << docid << endl;
	// bool rslt = AosDocServer::getSelf()->reserveObjid1(rdata, objid, docid, true);
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		objid, docid, objid_changed, true, false, rdata);
	if (userid == 0)
	{
		//--Ketty
		rdata->setUserid(docid);
		userid = docid;
	}

	// Create user home folder
	AosXmlTagPtr ddd = createHomeFolder3(rdata, cid);
	if (!ddd)
	{
		mLock->unlock();
		return false;
	}
	userobj->setAttr(AOSTAG_CTNR_HOME, ddd->getAttrStr(AOSTAG_OBJID));
	OmnString home_objid = ddd->getAttrStr(AOSTAG_OBJID);

	OmnString realname = userobj->getAttrStr(AOSTAG_REALNAME);
	if (realname == "") userobj->setAttr(AOSTAG_REALNAME, cid);

	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(docid != AOS_INVDID, mLock, false);
	userobj->setAttr(AOSTAG_OBJID, objid);
	userobj->setAttr(AOSTAG_DOCID, docid);

	//rslt = AosDocClientObj::getDocClient()->bindCloudid(cid, docid, rdata);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();

	createUserDirs(userctnr->getAttrU64(AOSTAG_DOCID, 0), userobj, rdata);

	// Create user home folder
	// OmnString uhobjid;
	// if (!createUserHomefolder1(userobj, userid, cid, uhobjid, errcode, errmsg))
	// {
	// 	mLock->unlock();
	// 	return false;
	// }

	// Create the home vpd, if needed
	vector<OmnString> vpdnames, anames;
	createDefaultVpds(anames, vpdnames, userobj, userctnr, rdata);
	// OmnString hpvpd = createHomeVpd1(AOSTAG_HPVPD, 
	// 		AOSTAG_HOMEVPD_POLICY, userobj, userctnr, rdata);
	// OmnString wordvpd_name = createHomeVpd1(AOSTAG_WORD_VPD, 
	// 		AOSTAG_WORKVPD_POLICY, userobj, userctnr, rdata);
	// OmnString publicvpd_name = createHomeVpd1(AOSTAG_PUBLIC_VPD, 
	// 		AOSTAG_PUBLICVPD_POLICY, userobj, userctnr, rdata);
	// OmnString familyvpd_name = createHomeVpd1(AOSTAG_FAMILY_VPD, 
	// 		AOSTAG_FAMILYVPD_POLICY, userobj, userctnr, rdata);

	// Create the user account
	aos_assert_r(rslt, false);

	// Create user folders
	OmnString blog_objid, album_objid, cmt_objid, rtcl_objid; 
	OmnString publish_objid, diary_objid; 
	
	createUserCtnr(rdata, AOSTAG_CTNR_BLOG,  AOSOBJIDRES_USER_BLOG,  AOSSTYPE_HMFD_USRBLOG, 
			AosObjname::composeBlogCtnrName(), blog_objid, home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_ALBUM, AOSOBJIDRES_USER_ALBUM, AOSSTYPE_HMFD_USRALBUM,
			AosObjname::composeAlbumCtnrName(), album_objid, home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_CMT,   AOSOBJIDRES_USER_CMT,   AOSSTYPE_HMFD_USRCMT,
			AosObjname::composeCommentCtnrName(), cmt_objid, home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_ATCL,  AOSOBJIDRES_USER_ATCL,  AOSSTYPE_HMFD_USRATCL,
			AosObjname::composeArticleCtnrName(), rtcl_objid, home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_PUBLISH,  AOSOBJIDRES_USER_PUBLISH,  
			AOSSTYPE_HMFD_PUBLISH, AosObjname::composePublishCtnrName(), publish_objid, 
			home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_DIARY, AOSOBJIDRES_USER_DIARY, AOSSTYPE_HMFD_DIARY, 
			AosObjname::composeDiaryCtnrName(), diary_objid, home_objid, userobj, cid, false);

	// 01/05/2011, Ken
	OmnString mblog_objid;
	createUserCtnr(rdata, AOSTAG_CTNR_MBLOG, AOSOBJIDRES_USER_MBLOG, AOSSTYPE_HMFD_USRMBLOG, 
			AosObjname::composeMblogCtnrName(), mblog_objid, home_objid, userobj, cid, false);
	
	// 01/05/2011, Ken
	OmnString lostfound_objid;
	createUserCtnr(rdata, AOSTAG_CTNR_LOSTFOUND, AOSOBJIDRES_USER_LOSTFOUND, 
			AOSSTYPE_HMFD_USRLOSTFOUND, AosObjname::composeLostFoundCtnrName(), 
			lostfound_objid, home_objid, userobj, cid, false);

	// 2011/12/14, Chen Ding
	OmnString vpd_ctnr_objid, image_ctnr_objid, data_ctnr_objid;
	createUserCtnr(rdata, AOSTAG_CTNR_VPD, AOSOBJIDRES_USER_VPD, AOSSTYPE_HMFD_VPD, 
			AosObjname::composeVpdCtnrName(), vpd_ctnr_objid, home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_IMAGE, AOSOBJIDRES_USER_IMAGE, AOSSTYPE_HMFD_IMAGE, 
			AOSOBJNAME_IMAGE_CTNR, image_ctnr_objid, 
			home_objid, userobj, cid, false);
	createUserCtnr(rdata, AOSTAG_CTNR_DATA, AOSOBJIDRES_USER_DATA, AOSSTYPE_HMFD_DATA, 
			AOSOBJNAME_USERDATA, data_ctnr_objid, 
			home_objid, userobj, cid, false);
	
	rslt = createUserObj(rdata, userobj, cid, username);

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
	resp = "<Contents ";
	resp << AOSTAG_USERNAME << "=\"" << username << "\" "
		<< AOSTAG_PARENTC << "=\"" << usrctnr_objid << "\" "
		<< AOSTAG_OBJID << "=\"" << userobj->getAttrStr(AOSTAG_OBJID) << "\" "
		<< AOSTAG_CTNR_HOME << "=\"" << home_objid << "\" "
		<< AOSTAG_CTNR_BLOG << "=\"" << blog_objid << "\" "
		<< AOSTAG_CTNR_ALBUM << "=\"" << album_objid << "\" "
		<< AOSTAG_CTNR_CMT << "=\"" << cmt_objid << "\" "
		<< AOSTAG_CTNR_ATCL << "=\"" << rtcl_objid << "\" "
		<< AOSTAG_CTNR_PUBLISH << "=\"" << publish_objid << "\" "
		<< AOSTAG_CTNR_VPD << "=\"" << vpd_ctnr_objid << "\" "
		<< AOSTAG_CTNR_IMAGE << "=\"" << image_ctnr_objid << "\" "
		<< AOSTAG_CTNR_MBLOG << "=\"" << mblog_objid << "\" "
		<< AOSTAG_CTNR_LOSTFOUND << "=\"" << lostfound_objid << "\" "
		<< AOSTAG_CTNR_DATA << "=\"" << data_ctnr_objid << "\" "
		<< AOSTAG_CTNR_DIARY << "=\"" << diary_objid << "\" ";

	for (u32 i=0; i<anames.size(); i++)
	{
		if (anames[i] != "" && vpdnames[i] != "")
		{
			resp << anames[i] << "=\"" << vpdnames[i] << "\" ";
		}
	}

	OmnString userrole = userobj->getAttrStr(AOSTAG_USER_ROLE);
	if (userrole != "") resp << AOSTAG_USER_ROLE << "=\"" << userrole << "\"";
	OmnString accttype = userobj->getAttrStr(AOSTAG_ACCT_TYPE);
	if (accttype != "") resp << AOSTAG_ACCT_TYPE << "=\"" << accttype << "\"";
	resp << "/>";

	// Need to send email
	//Linda 2010/10/16
	rdata->setOk();
	if (email != "" && is_send_email)
	{
		OmnString subject, contents, emailstr;
		if (sdoc_emailobjid != "")
		{
			aos_assert_r(rdata, false);
			rdata->setSourceDoc(userobj, false);
			AosSmartDocObj::procSmartdocsStatic(sdoc_emailobjid, rdata);
			emailstr = rdata->getResults();
			rdata->setResults("");
			if (!rdata->isOk())
			{
			    emailstr = "";
			    rdata->setOk();
			}
			rdata->setSourceDoc(0, false);
		}
		if (emailstr == "")
		{
			OmnString subject = "云语言会员注册";
			OmnString contents = "欢迎您加入云语言社区.\n    您的用户�?";
			contents << username <<"  云号:"
	    		     << cid << "  密码:"
	       	 	 <<passwd <<"\n    点击激�? http://www.zykie.com";

			emailstr="<sendemail ";
			emailstr << "rmailaddr =\"" << email <<"\" "
	    	     << "subject =\"" << subject <<"\" "
		 		 << "contents =\"" << contents <<"\" />";

		}
		aos_assert_r(emailstr!="", false);
		AosXmlParser parser;
		AosXmlTagPtr emailxml = parser.parse(emailstr, "" AosMemoryCheckerArgs);
		//AosPyemail::getSelf()->sendmail(emailxml, rdata);
	}
	return true;
}


bool
AosLoginMgr::verifyPassword(
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &cid,
		const OmnString &username,
		const OmnString &passwd,
		const AosXmlTagPtr &userdoc, 
		const int level,
		AosXmlRc &errcode, 
		OmnString &errmsg,
		const AosRundataPtr &rdata)
{
	// Password is encoded in the subtag:
	// 	<...>
	// 		<zky_passwd__n>xxx</zky_passwd__n>
	// 		or
	// 		<zky_passwd__n1>xxx</zky_passwd__n1>
	// 		...
	// 	</...>
	AosXmlTagPtr pwdnode;
	if (level > 0)
	{
		OmnString nodename;
		nodename << AOSTAG_PASSWD << level;
		pwdnode = userdoc->getFirstChild(nodename);
	}
	else
	{
		pwdnode = userdoc->getFirstChild(AOSTAG_PASSWD);
	}

	if (!pwdnode)
	{
		errcode = eAosXmlInt_General;
		errmsg = "No password found!";
		addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
 			AOSLOGIN_NOPASSWD, errmsg, rdata);
		return false; 
	}

	OmnString pwd = pwdnode->getNodeText();
	if (pwd != passwd)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Password mismatch!";
		addLoginEntry(siteid, ctnr_objid, cid, username, userdoc, 
			AOSLOGIN_MISSING_PASSWORD, errmsg, rdata);
		return false;
	}

	return true;
}


bool
AosLoginMgr::addLoginEntry(
		const u32 siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &status, 
		const OmnString &errmsg, 
		const AosRundataPtr &rdata)
{
	OmnString logentry = "<login ";
	logentry << AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG 
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSLOGTYPE_LOGIN 
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_LOGTYPE << "=\"" << AOSLOGTYPE_LOGIN
		<< "\" " << AOSTAG_LOGID << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_CTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale())
		<< "\" " << AOSTAG_STATUS << "=\"" << status << "\">"
		<< errmsg << "</login>";
OmnScreen << "Log: " << logentry << endl;
	AosXmlParser parser;
	AosXmlTagPtr logdoc = parser.parse(logentry, "" AosMemoryCheckerArgs);
	aos_assert_r(logdoc, false);
	return AosSeLogClientObj::getLogClient()->addLoginLog(logdoc, rdata);
}


bool
AosLoginMgr::addLoginEntry(
		const u32 siteid,
		const OmnString &ctnr_objid, 
		const OmnString &cloudid,
		const OmnString &username,
		const AosXmlTagPtr &userdoc,
		const OmnString &status, 
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	// 	<login
	// 		AOSTAG_OTYPE=AOSOTYPE_LOG
	// 		AOSTAG_STYPE=AOSLOGTYPE_LOGIN
	// 		AOSTAG_SITEID="xxx"
	// 		AOSTAG_PUB_CONTAINER="xxx"
	// 		AOSTAG_USERNAME="xxx"
	// 		AOSTAG_USERID="xxx"
	// 		AOSTAG_CTIME="xxx"
	// 		AOSTAG_STATUS=status>msg</login>
	OmnString logentry = "<login ";
	logentry << AOSTAG_OTYPE << "=\"" << AOSOTYPE_LOG 
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSLOGTYPE_LOGIN 
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_PARENTC << "=\"" <<ctnr_objid 
		<< "\" " << AOSTAG_LOGTYPE << "=\"" << AOSLOGTYPE_LOGIN
		<< "\" " << AOSTAG_CLOUDID << "=\"" << cloudid
		<< "\" " << AOSTAG_USERNAME << "=\"" << username;
	if (userdoc) 
	{
		logentry << "\" " << AOSTAG_USERID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID)
			<< "\" " << AOSTAG_DOCID << "=\"" << userdoc->getAttrStr(AOSTAG_DOCID);
	}
	logentry << "\" " << AOSTAG_CTIME << "=\"" << OmnGetTime(AosLocale::getDftLocale())
		<< "\" " << AOSTAG_STATUS << "=\"" << status << "\">"
		<< msg << "</login>";
OmnScreen << "Log: " << logentry << endl;
	AosXmlParser parser;
	AosXmlTagPtr logdoc = parser.parse(logentry, "" AosMemoryCheckerArgs);
	aos_assert_r(logdoc, false);
	return AosSeLogClientObj::getLogClient()->addLoginLog(logdoc, rdata);
}


void
AosLoginMgr::addCreateAcctEntry(
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


AosXmlTagPtr
AosLoginMgr::getUserCtnr(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userobj, 
		OmnString &ctnr_objid)
{
	// It retrieves/creates the user container. The user container objid
	// is specified by the attribute AOSTAG_PARENTC. Or in other
	// word, if a container is created for managing user accounts, that
	// container must be a public container. If the user doc did not 
	// specify AOSTAG_PARENTC, it is an error.
	//
	// IMPORTANT: In the current implementations, there shall be one and 
	// only one parent container. Additional containers may be added 
	// later on.
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->getUserid();
	OmnString errmsg;

	ctnr_objid = userobj->getPrimaryContainer();
	if (ctnr_objid == "") 
	{
		errmsg = "Missing user account container. User objid: ";
		errmsg << userobj->getAttrStr(AOSTAG_OBJID) 
			<< ". Docid: " << userobj->getAttrStr(AOSTAG_DOCID);
		OmnAlarm << errmsg << enderr;
		AosSetErrorU(rdata, "missing_user_acct_ctnr") << ": " << errmsg << enderr;
		return 0;
	}

	//AosXmlTagPtr usrctnr = AosDocServerSelf->getDoc(siteid, ctnr_objid);	
	AosXmlTagPtr usrctnr = AosDocClientObj::getDocClient()->getDocByObjid(ctnr_objid, rdata);	
	if (!usrctnr)
	{
		if (userid == 0) return 0;

		// The container does not exist yet. Normally this is an error. 
		// In the current implementation, we will create it.
		OmnString docstr = "<ctnr ";
		docstr << AOSTAG_OBJID << "=\"" << ctnr_objid << "\" " 
			<< AOSTAG_SITEID << "=\"" << siteid << "\" "
			<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_USRACCT_CTNR << "\" "
			<< AOSTAG_PARENTC << "=\"" << AOSDFTCONTAINER_USER << "\" ";
		docstr << "/>";

		//--Ketty
		//usrctnr = AosDocServerSelf->createDocSafe1(
		//	docstr, siteid, userid, AOSAPPNAME_SYSTEM, "", "", true, false,
		//	errcode, errmsg, false, false, true, true, ttl);
		usrctnr = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, "", "", true, false,
		    false, false, true, true);
		if (!usrctnr)
		{
			errmsg = "Failed to create user container: ";
			errmsg << ctnr_objid;
			OmnAlarm << errmsg << enderr;
			AosSetErrorU(rdata, "failed_create_user_ctnr") << ": " << ctnr_objid << enderr;
			return 0;
		}
		ctnr_objid = usrctnr->getAttrStr(AOSTAG_OBJID);

		// Log it
		// AosLogMgr::getSelf()->addCreateCtnrEntry(siteid, userid, ctnr_objid, "");

		// Create the image directory for the container
		createUserBaseDir(siteid, usrctnr->getAttrU64(AOSTAG_DOCID, 0));
	}
	userobj->setAttr(AOSTAG_PARENTC, ctnr_objid);
	rdata->setOk();
	return usrctnr;
}


OmnString
AosLoginMgr::createUserBaseDir(
		const u32 siteid, 
		const u64 &docid)
{
	aos_assert_r(docid != AOS_INVDID, "");
	OmnString dir = mUserBaseDir;
	aos_assert_r(dir != "", "");
	dir << "/userData/" << siteid;
	OmnString cmd = "mkdir -p ";
	cmd << dir;
	system(cmd.data());

	return dir;
}


AosXmlTagPtr
AosLoginMgr::createHomeFolder3(
		const AosRundataPtr &rdata,
		const OmnString &usr_cid)
{
	u32 siteid = rdata->getSiteid();	
	aos_assert_rr(siteid != 0, rdata, 0);

	AosObjMgr::getSelf()->createCidCtnr(rdata);

	// Creates user's home folder, whose name is:
	// 	zcid_cidnnn
	OmnString objid = AosObjid::composeUserHomeCtnrObjid(usr_cid);
	
	// Create the parent folder
	OmnString docstr = "<ctnr ";
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid 
		<< "\" " << AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_USER_PFOLDER
		<< "\" " << AOSTAG_OBJID << "=\"" << objid 
		<< "\" " << AOSTAG_CTNR_PUBLIC << "=\"" << "true"
		<< "\" " << AOSTAG_PARENTC << "=\"" << AosObjid::composeCidCtnrObjid(siteid) 
		<< "\"/>";
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
		rdata, docstr, usr_cid, "", 
		true, false, false, false, true, true);
	if (!doc) return 0;

	createUserBaseDir(siteid, doc->getAttrU64(AOSTAG_DOCID, 0));
	return doc;
}


bool
AosLoginMgr::addOnlineEntry(
		const u32 siteid, 
		const OmnString &user_container, 
		const OmnString &realname, 
		const OmnString &userid,
		const AosRundataPtr &rdata)
{
	// Online users are managed by an IIL. Its word is:
	// 	[AOSZTG_ONLINE + "_" + siteid + "_" + user_container, username, docid]
	// This is a string iil. 
	OmnString iilname = constructOnlineIILName(siteid, user_container);
	//bool rslt = AosIILClient::getSelf()->addStrValueDoc(
	bool rslt = AosAddStrValueDoc(
			iilname, realname, atoll(userid.data()), false, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLoginMgr::removeOnlineEntry(
		const u32 siteid, 
		const OmnString &user_container, 
		const OmnString &realname, 
		const OmnString &userid,
		const AosRundataPtr &rdata)
{
	// Online users are managed by an IIL. Its word is:
	// 	[AOSZTG_ONLINE + "_" + siteid + "_" + user_container, username, docid]
	// This is a string iil. 
	OmnString iilname = constructOnlineIILName(siteid, user_container);
	bool rslt = AosRemoveStrValueDoc(
		iilname, realname, atoll(userid.data()), rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosLoginMgr::retrieveOnlineEntries(
		const u32 startidx,
		const u32 psize)
{
	// This function retrieves the online entries from the start 'start_idx',
	// for the given number of entries 'psize'. If 'fnames' is not empty,
	// it will retrieve the attributes specified in 'fnames'.
	// OmnString iilname = constructOnlineIILName(siteid, user_container);
	OmnNotImplementedYet;
	return false;
}


bool
AosLoginMgr::createUserDirs(
		const u64 &ctnr_docid, 
		const AosXmlTagPtr &userobj, 
		const AosRundataPtr &rdata)
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
	const OmnString userbase = createUserBaseDir(rdata->getSiteid(), ctnr_docid);
	OmnString rootdir = userbase;
	aos_assert_rr(rootdir != "", rdata, false);
	OmnString cid = userobj->getAttrStr(AOSTAG_CLOUDID);
	aos_assert_rr(cid != "", rdata, false);
	OmnString syscmd = "mkdir -p ";

	//The home directory should be in the form:
	//user_data_base/siteid/cloudid/home
	// Create the home directory
	rootdir << "/" << cid << "/home";
	syscmd << rootdir;
	system(syscmd.data());
	int length = mUserBaseDir.length();
	OmnString dir;
	dir = rootdir.substr(length+1);

	userobj->setAttr(AOSTAG_USER_HOMEDIR, dir);

	// Create the image directory
	syscmd = "mkdir -p ";
	// Phnix, 2011/01/23
	// The image directory should be in the form:
	// 	user_data_base/siteid/cloudid/images
	rootdir = userbase; 
	rootdir << "/" << cid << "/images";
	syscmd << rootdir;
	dir = rootdir.substr(length+1);
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_IMGDIR, dir);

	// Create the resource directory
	// Phnix, 2011/01/23
	// The resource directoryshould be in the form:
	// 	user_data_base/siteid/cloudid/rsc
	syscmd = "mkdir -p ";
	rootdir = userbase;
	rootdir << "/" << cid << "/rsc";
	syscmd << rootdir;
	dir = rootdir.substr(length+1);
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_RSCDIR, dir);

	// Create Email directory
	// Brian Zhang 08/03/2011
	// The form :
	// 	user_data_base/siteid/cloudid/email
	syscmd = "mkdir -p ";
	rootdir = userbase;
	rootdir << "/" << cid << "/email";
	mUserEmailDir = rootdir;
	syscmd << rootdir;
	dir = rootdir.substr(length+1);
	system(syscmd.data());
	userobj->setAttr(AOSTAG_USER_EMLDIR, dir);
	rdata->setOk();
	return true;
}


bool
AosLoginMgr::createUserObj(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &userobj, 
		const OmnString &cid,
		const OmnString &username)
{
	u32 siteid = rdata->getSiteid();
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

	/*
	// Below are some hard coded attributes:
	// 	zky_id is the same as AOSTAG_USERNAME
	// 	zky_objnm is the same as AOSTAG_REALNAME
	// 	zky_xjid = "32050021095" + AOSTAG_USERNAME
	OmnString usrnm = userobj->getAttrStr(AOSTAG_USERNAME);
	OmnString nmprefix = userobj->getAttrStr(AOSTAG_UNAMEPREFIX);
	if (nmprefix != "")
	{
		usrnm = usrnm.substr(nmprefix.length(), 0);
	}

	OmnString rnm = userobj->getAttrStr(AOSTAG_REALNAME);
	OmnString xjid = "32050021095";
	xjid << usrnm;
	userobj->setAttr("zky_id", usrnm);
	userobj->setAttr(AOSTAG_OBJNAME, rnm);
	userobj->setAttr("zky_xjid", xjid);
	// End of the hard code
	*/

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

	// Ketty run SmartDoc 2011/05/06
	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	OmnString sobjids;
	if(cmd && (sobjids=cmd->getAttrStr(AOSTAG_PREV_SDOCOBJID)) != "" )
	{
		OmnStrParser1 parser(sobjids, ",");
		OmnString sdocid;
		while((sdocid = parser.nextWord()) != "")
		{
			AosSmartDocObj::procSmartdocsStatic(sdocid, rdata);
		}
	}
	// Ketty End

	rdata->setReceivedDoc(userobj, true);
	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(
		rdata, userobj, cid, "", 
		true, false, true, true, true, true, true);
	aos_assert_r(dd, false);
	rdata->setOk();
	return true;
}


bool
AosLoginMgr::createUserCtnr(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_aname,
		const OmnString &ctnr_name,
		const OmnString &subtype,
		const OmnString &objname,
		OmnString &ctnr_objid,
		const OmnString &userhome_objid ,
		const AosXmlTagPtr &userdoc, 
		const OmnString &cid, 
		const bool checkCreation) 
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
	u32 siteid = userdoc->getAttrU32(AOSTAG_SITEID,0);
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
	//AosXmlTagPtr doc = AosDocServerSelf->createDocSafe1(
	//		docstr, siteid, requester_id, cid, AOSAPPNAME_SYSTEM, "", 
	//		true, true, errcode, errmsg, false, false, false, true, ttl);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, cid, "", true, checkCreation, false, false, false, true);
	aos_assert_r(doc, false);
	ctnr_objid = doc->getAttrStr(AOSTAG_OBJID);
	userdoc->setAttr(ctnr_aname, ctnr_objid);
	rdata->setOk();
	return true;
}


OmnString
AosLoginMgr::getNewCid(const u32 siteid, const AosRundataPtr &rdata)
{
	return AosSeIdGenMgr::getSelf()->nextCid(siteid, rdata);
}


AosXmlTagPtr
AosLoginMgr::createSuperUser(const AosRundataPtr &rdata)
{
	// This function creates the super user account for the site 'siteid'.
	// It assumes the object has not been created yet.
	// if (OmnApp::getAppConfig()->getAttrStr(AOSTAG_ALLOW_CRTSPUSR) == "true")
	// {
	// 	errmsg = "Server not allowed to create super user!";
	// 	return false;
	// }

	// OmnString server_passwd = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_SYSPASSWD);
	// if (requester_passwd != server_passwd)
	// {
	// 	errmsg = "Password not match!";
	// 	return false;
	// }

	u32 siteid = rdata->getSiteid();
	OmnString root_objid = AosObjid::composeRootAcctObjid(siteid);
	u64 docid = AOSDOCID_ROOT;
	AosXmlTagPtr userobj = AosDocClientObj::getDocClient()->getDocByObjid(root_objid, rdata);
	if (userobj)
	{
		// Check whether its docid is correct
		if (userobj->getAttrU64(AOSTAG_DOCID, 0) == docid)
		{
			rdata->setOk();
			return userobj;
		}

		// This is incorrect. Need to remove the doc
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosDocClientObj::getDocClient()->deleteDoc(userobj, rdata, "", true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		userobj = 0;
	}

	userobj = AosDocClientObj::getDocClient()->getDocByDocid(AOSDOCID_ROOT, rdata); 
	if (userobj) 
	{
		// The root account has been created.
		return userobj;
	}

	AosXmlTagPtr sysctnr = AosObjMgr::getSelf()->getSysUserCtnr(siteid, rdata);
if (!sysctnr)
	OmnMark;
	aos_assert_r(sysctnr, 0);

	OmnString root_passwd = "";
	root_passwd << "12345";				// Create the password for Root

	// Check whether AOSUSERNAME_ROOT has been created
	OmnString ctnr_objid = sysctnr->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(ctnr_objid != "", 0);
	
	OmnString cid = AOSCLOUDID_ROOT;
	aos_assert_r(cid != "", 0);

	OmnString docstr = "<root ";
	OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_CLOUDID << "=\"" << cid
		<< "\" " << AOSTAG_USERNAME << "=\"" << AOSUSERNAME_ROOT
		<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_ROOT
		<< "\" " << AOSTAG_REALNAME << "=\"" << AOSOBJNAME_ROOT
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_SYSACCT << "=\"" << AOSSTYPE_SYSACCT 
		<< "\" " << AOSTAG_SYSACCT_SIGNATURE << "=\"" << signature 
		<< "\" " << AOSTAG_USERTYPE << "=\"" << AOSUSERTYPE_ROOT 
		<< "\" " << AOSTAG_OBJID << "=\"" << root_objid 
		<< "\"><" << AOSTAG_PASSWD << ">" << root_passwd 
		<< "</" << AOSTAG_PASSWD << ">"
		<< "</root>";
	
	AosXmlParser parser;
	userobj = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!userobj)
	{
		rdata->setError() << "Failed to create obj (Internal error)";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Reserved the objid
	bool objid_changed = false;
OmnScreen << "To bind: " << root_objid << ":" << docid << endl;
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		root_objid, docid, objid_changed, false, true, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Failed to reserved the objid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	// Bind the cid
	//rslt = AosDocClientObj::getDocClient()->bindCloudid(cid, docid, rdata);
	aos_assert_r(rslt, 0);

	// Create user parent folder
	AosXmlTagPtr ddd = createHomeFolder3(rdata, cid);
	if (!ddd)
	{
		rdata->setError();
		return 0;
	}

	userobj->setAttr(AOSTAG_CTNR_HOME, ddd->getAttrStr(AOSTAG_OBJID));

	userobj->setAttr(AOSTAG_DOCID, docid);
	rslt = createUserObj(rdata, userobj, cid, cid);
	if (!rslt) 
	{
		return 0;
	}

	// Log the action
	addCreateAcctEntry(siteid, ctnr_objid, cid, userobj);
	return userobj;
}


bool 
AosLoginMgr::createGuestUser(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &userobj)
{
	u32 siteid = rdata->getSiteid();
	OmnString objid = AosObjid::composeGuestObjid(siteid);
	userobj = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (userobj)
	{
		// Check whether its docid is correct
		if (userobj->getAttrU64(AOSTAG_DOCID, 0) == AOSDOCID_GUEST)
		{
			rdata->setOk();
			return true;
		}

		// This is incorrect. Need to remove the doc
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosDocClientObj::getDocClient()->deleteDoc(userobj, rdata, "", true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		userobj = 0;
	}

	userobj = AosDocClientObj::getDocClient()->getDocByDocid(AOSDOCID_GUEST, rdata);
	if (userobj) 
	{
		// Already created
		rdata->setOk();
		return true;
	}

	OmnString errmsg;

	AosXmlTagPtr sysctnr = AosObjMgr::getSelf()->getSysUserCtnr(siteid, rdata);
	aos_assert_r(sysctnr, false);

	// Create the password
	OmnString passwd;
	passwd << rand();

	// Check whether it has been created
	OmnString ctnr_objid = sysctnr->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(ctnr_objid != "", false);

	OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
	OmnString docstr = "<guest ";
	// OmnString cid = getNewCid(siteid, rdata);
	OmnString cid = AOSCLOUDID_GUEST;
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_CLOUDID << "=\"" << cid
		<< "\" " << AOSTAG_USERNAME << "=\"" << objid
		<< "\" " << AOSTAG_OBJNAME << "=\"" << AOSOBJNAME_GUEST
		<< "\" " << AOSTAG_REALNAME << "=\"" << AOSOBJNAME_GUEST
		<< "\" " << AOSTAG_SYSACCT << "=\"" << AOSSTYPE_SYSACCT 
		<< "\" " << AOSTAG_SYSACCT_SIGNATURE << "=\"" << signature 
		<< "\" " << AOSTAG_USERSTATUS << "=\"" << AOSUSRSTATUS_ACTIVE
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_USERTYPE << "=\"" << AOSUSERTYPE_GUEST
		<< "\" " << AOSTAG_OBJID << "=\"" << objid 
		<< "\"><" << AOSTAG_PASSWD << ">" << passwd 
		<< "</" << AOSTAG_PASSWD << ">"
		<< "</guest>";
	
	AosXmlParser parser;
	userobj = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!userobj)
	{
		errmsg = "Failed to create obj (Internal error)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return false;
	}

	// Chen Ding, 12/06/2010
	// Reserved the objid
	u64 docid = AOSDOCID_GUEST;
	bool objid_changed = false;
OmnScreen << "To bind: " << objid << ":" << docid << endl;
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		objid, docid, objid_changed, false, true, rdata);
	aos_assert_r(rslt, false);
	userobj->setAttr(AOSTAG_DOCID, docid);

	// Bind the cid
	//rslt = AosDocClientObj::getDocClient()->bindCloudid(cid, docid, rdata);
	aos_assert_r(rslt, false);

	// Create user parent folder
	AosLoginMgrObjPtr loginmgr_obj = AosLoginMgrObj::getLoginMgr();
	aos_assert_r(loginmgr_obj, false);
	AosXmlTagPtr ddd = loginmgr_obj->createHomeFolder3(rdata, cid);
	if (!ddd)
	{
		rdata->setError();
		return false;
	}
	userobj->setAttr(AOSTAG_CTNR_HOME, ddd->getAttrStr(AOSTAG_OBJID));

	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	AosXmlTagPtr received_doc = rdata->setReceivedDoc(userobj, false);
	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(
		rdata, userobj, cid, "", true, false, true, true, true, false, true);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	rdata->setReceivedDoc(received_doc, false);
	aos_assert_r(dd, false);

	userid = rdata->setUserid(docid);
	AosAccessRcdPtr arcd = AosObjMgr::getSelf()->createGuestArcd(rdata, userobj);
	aos_assert_r(arcd, false);
	rdata->setUserid(userid);
	rdata->setOk();
	return true;
}


AosXmlTagPtr
AosLoginMgr::getGuestDoc(const u32 siteid, const AosRundataPtr &rdata)
{
	OmnString objid = AosObjid::composeGuestObjid(siteid);
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
}


AosXmlTagPtr 
AosLoginMgr::createUnknownUser(const AosRundataPtr &rdata)
{
	// Check whether it has been created
	u32 siteid = rdata->getSiteid();
	OmnString objid = AOSOBJIDPRE_UNKNOWN;
	AosXmlTagPtr userobj = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (userobj)
	{
		// Check whether its docid is correct
		if (userobj->getAttrU64(AOSTAG_DOCID, 0) == AOSDOCID_UNKNOW)
		{
			rdata->setOk();
			return userobj;
		}

		// This is incorrect. Need to remove the doc
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		AosDocClientObj::getDocClient()->deleteDoc(userobj, rdata, "", true);
		rdata->setAppname(appname);
		rdata->setUserid(userid);
		userobj = 0;
	}

	userobj = AosDocClientObj::getDocClient()->getDocByDocid(AOSDOCID_UNKNOW, rdata);
	if (userobj) 
	{
		// Already created
		return userobj;
	}

	OmnString errmsg;

	mLock->lock();
	AosXmlTagPtr sysctnr = AosObjMgr::getSelf()->createSysUserCtnr(rdata);
	aos_assert_rl(sysctnr, mLock, 0);

	// Create the password
	OmnString passwd;
	passwd << rand();

	OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
	OmnString ctnr_objid = sysctnr->getAttrStr(AOSTAG_OBJID);
	aos_assert_rl(ctnr_objid != "", mLock, 0);
	OmnString docstr = "<unknown ";
	// OmnString cid = getNewCid(siteid);
	OmnString cid = AOSCLOUDID_UNKNOWN;
	docstr << AOSTAG_OTYPE << "=\"" << AOSOTYPE_USERACCT
		<< "\" " << AOSTAG_SITEID << "=\"" << siteid
		<< "\" " << AOSTAG_CLOUDID << "=\"" << cid
		<< "\" " << AOSTAG_USERNAME << "=\"" << objid
		<< "\" " << AOSTAG_OBJNAME << "=\"" <<  AOSOBJNAME_UNKNOWN
		<< "\" " << AOSTAG_REALNAME << "=\"" << AOSOBJNAME_UNKNOWN
		<< "\" " << AOSTAG_SYSACCT << "=\"" << AOSSTYPE_SYSACCT 
		<< "\" " << AOSTAG_SYSACCT_SIGNATURE << "=\"" << signature 
		<< "\" " << AOSTAG_USERSTATUS << "=\"" << AOSUSRSTATUS_ACTIVE
		<< "\" " << AOSTAG_PARENTC << "=\"" << ctnr_objid
		<< "\" " << AOSTAG_USERTYPE << "=\"" << AOSUSERTYPE_UNKNOWN
		<< "\" " << AOSTAG_OBJID << "=\"" << objid 
		<< "\"><" << AOSTAG_PASSWD << ">" << passwd 
		<< "</" << AOSTAG_PASSWD << ">"
		<< "</unknown>";
	
	AosXmlParser parser;
	userobj = parser.parse(docstr, "" AosMemoryCheckerArgs);
	if (!userobj)
	{
		mLock->unlock();
		errmsg = "Failed to create obj (Internal error)";
		OmnAlarm << errmsg << enderr;
		rdata->setError() << errmsg;
		return 0;
	}

	u64 docid = AOSDOCID_UNKNOW;
	bool objid_changed = false;
OmnScreen << "To bind: " << objid << ":" << docid << endl;
	bool rslt = AosDocClientObj::getDocClient()->bindObjid(
		objid, docid, objid_changed, true, true, rdata);
	if (!rslt)
	{
		mLock->unlock();
		OmnAlarm << "Failed to reserveObjid: " << objid << enderr;
		rdata->setError() << errmsg;
		return 0;
	}
	userobj->setAttr(AOSTAG_DOCID, docid);

	// Create user parent folder
	AosXmlTagPtr ddd = createHomeFolder3(rdata, cid);
	if (!ddd)
	{
		mLock->unlock();
		rdata->setError();
		return 0;
	}
	userobj->setAttr(AOSTAG_CTNR_HOME, ddd->getAttrStr(AOSTAG_OBJID));

	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	AosXmlTagPtr received_doc = rdata->setReceivedDoc(userobj, false);
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	AosXmlTagPtr dd = AosDocClientObj::getDocClient()->createDocSafe3(
		rdata, userobj, cid, "", true, false, true, true, true, false, true);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	rdata->setReceivedDoc(received_doc, false);
	aos_assert_rl(dd, mLock, 0);

	// Bind the cid
	//rslt = AosDocClientObj::getDocClient()->bindCloudid(cid, docid, rdata);
	aos_assert_rl(rslt, mLock, 0);

	userid = rdata->setUserid(docid);
	AosAccessRcdPtr arcd = AosObjMgr::getSelf()->createUnknownUserArcd(rdata, userobj);
	aos_assert_rl(arcd, mLock, 0);
	rdata->setUserid(userid);
	mLock->unlock();
	return userobj;
}


bool
AosLoginMgr::createDefaultVpds(
		vector<OmnString> &anames,
		vector<OmnString> &vpdnames,
		const AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata)
{
	// This function creates all the default vpds. Default vpds are defined
	// by the container:
	// 	<container ...>
	// 		<defaultvpds>
	// 			<dftvpd .../>
	// 			<dftvpd .../>
	// 			...
	// 		</defaultvpds>
	// 		...
	// 	</container>
	aos_assert_rr(userdoc, rdata, false);
	aos_assert_rr(ctnr_doc, rdata, false);
	vpdnames.clear();
	anames.clear();

	AosXmlTagPtr tags = ctnr_doc->getFirstChild("zky_defaultvpds");
	if (!tags)
	{
		// No default vpds are defined.
		return true;
	}

	AosXmlTagPtr tag = tags->getFirstChild();
	while (tag)
	{
		OmnString vpdname = createHomeVpd1(tag, userdoc, ctnr_doc, rdata);
		if (vpdname != "")
		{
			OmnString aname = tag->getAttrStr(AOSTAG_ATTRNAME);
			if (aname == "")
			{
				OmnAlarm << "Attribute is empty: " << ctnr_doc->toString() << enderr;
			}
			else
			{
				anames.push_back(aname);
				vpdnames.push_back(vpdname);
			}
		}
		tag = tags->getNextChild();
	}

	return true;
}


OmnString
AosLoginMgr::createHomeVpd1(
		const AosXmlTagPtr &def_tag,
		const AosXmlTagPtr &userdoc,
		const AosXmlTagPtr &ctnr_doc,
		const AosRundataPtr &rdata)
{
	// This function determines the home VPD. It checks whether it was
	// already created. If yes, it should be able to retrieve the vpd name
	// from 'userdoc' by the name 'aname'. If yes, it returns the VPD name.
	// Otherwise, how the vpd is created is determined by the tag named 
	// 'tagname' in its parent container.
	// 	<container ...>
	// 		<tagname policy="xxx"/>
	// 	</container>
	// If it is not defined, it defaults to the default policy.
	// 1. If the requested vpd is already in 'aname', just return it.
	// 2. Otherwise, it retrieves the vpd. After that, it checks whether it needs
	//    to create a copy of it. If yes, it creates the copy and then sets the
	//    vpd name to 'aname'.
	if (!def_tag) return "";
	aos_assert_rr(userdoc, rdata, "");
	OmnString aname = def_tag->getAttrStr(AOSTAG_ATTRNAME);
	aos_assert_rr(aname != "", rdata, "");
	aos_assert_rr(ctnr_doc, rdata, "");

	AosDftVpdPolicy::E vpd_policy = AosDftVpdPolicy::toEnum(
			def_tag->getAttrStr("vpd_policy")); 

	OmnString vpdname;
	switch (vpd_policy)
	{
	case AosDftVpdPolicy::eBySex:
		 // The VPD is determined by sex. 
		 // 		<zky_dftvpds>
		 // 			<record sex="xxx" vpdname="xxx"/>
		 // 			<record sex="xxx" vpdname="xxx"/>
		 // 			...
		 // 		</zky_dftvpds>
		 // 		...
		 vpdname = getDftHpvpdBySex1(rdata, def_tag, userdoc, ctnr_doc);
		 break;

	default:
		 OmnAlarm << "Unrecognized default vpd policy: " 
			 << def_tag->getAttrStr("vpd_policy") << enderr;
		 break;
	}

	// Make sure it got the vpdname. If not, retrieve the default one. 
	// If failed, raise alarm and return.
	if (vpdname == "") 
	{
		AosSetError(rdata, AOSLT_FAILED_DETERMINE_DFTVPD);
		OmnAlarm << rdata->getErrmsg() << ". " << aname << enderr;
		return "";
	}
	
	// Retrieve the VPD
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	u32 siteid = rdata->getSiteid();
	u64 uid = rdata->setUserid(AosObjMgr::getSysRootDocid(siteid, rdata));
	AosXmlTagPtr vpd = AosDocClientObj::getDocClient()->getDocByObjid(vpdname, rdata);
	if (!vpd)
	{
		AosSetError(rdata, AOSLT_FAILED_RETRIEVE_DFTVPD);
		OmnAlarm << rdata->getErrmsg() << ". " << vpdname << enderr;
		return "";
	}

	// Check whether it needs to clone it.
	if (def_tag->getAttrBool("need_clone", true))
	{
		// It does need to clone. 
		OmnString newname = composeNewVpdname(vpdname, def_tag, rdata);
		aos_assert_rr(newname != "", rdata, "");
		vpd->setAttr(AOSTAG_OBJID, newname);
		vpd->setAttr(AOSTAG_PARENTC, userdoc->getAttrStr(AOSTAG_CTNR_VPD));

		AosXmlTagPtr newvpd = AosDocClientObj::getDocClient()->createDocSafe3(rdata, 
			vpd, rdata->getCid(), "", true, false, false, false, true, false, true);
		aos_assert_rr(newvpd, rdata, "");
		vpdname = newvpd->getAttrStr(AOSTAG_OBJID);
	}

	// Modify the userdoc
	userdoc->setAttr(aname, vpdname), 

	rdata->setAppname(appname);
	rdata->setUserid(uid);
	rdata->setOk();
	return vpdname;
}


OmnString
AosLoginMgr::composeNewVpdname(
		const OmnString &vpdname,
		const AosXmlTagPtr &def_tag, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(def_tag, rdata, "");
	OmnString name_policy = def_tag->getAttrStr("name_policy");
	const char data = (name_policy.data())[0];
	OmnString name;
	switch (data)
	{
	case '1':
		 // AOSTAG_VPDNAME_PREFIX + "_" + 'vpdname' + cid
		 // This is the default.
		 break;

	case '2':
		 // Vpdname base from def_tag + cid
		 name = def_tag->getAttrStr("vpdname_base");
		 if (name != "")
		 {
			 return AosObjid::compose(name, rdata->getCid());
		 }
		 break;

	default:
		 break;
	}

	name = AOSTAG_VPDNAME_PREFIX;
	name << "_" << vpdname;
	return AosObjid::compose(name, rdata->getCid());
}


