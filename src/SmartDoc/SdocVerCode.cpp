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
// 04/14/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SmartDoc/SdocVerCode.h"

#include "Actions/ActSeqno.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/SMDMgr.h"
#include "Util/String.h"
#include "Python/Pyemail.h"
#include "SEUtilServer/UserDocMgr.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEServer/SeReqProc.h"
#include "Rundata/Rundata.h"
#include "SEModules/LoginMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosSdocVerCode::AosSdocVerCode(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_VERCODE, AosSdocId::eVerCode, flag)
{
}


AosSdocVerCode::~AosSdocVerCode()
{
}


bool
AosSdocVerCode::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//Zky3233, Linda, 2011/04/18
 	// <smartdoc 
	//	zky_sdoctp="vercode"  //this is sdoc type important!
	//	zky_siteid="100"
	//	zky_otype="zky_smtdoc">
	//		<config EncryptType="SH1|MD5" sendmethod="shortmessage|email">
	//		</config>
	//</smartdoc>
	//
	AosXmlTagPtr request = rdata->getReceivedDoc();

	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!request)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString capcha= AosGetVersionUID();
	aos_assert_r(capcha!="", false);

	AosXmlTagPtr config = sdoc->getFirstChild("config");
	if (!config)
	{
		rdata->setError() <<"Missing Config!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString encryptType = config->getAttrStr("EncryptType", "MD5"); 

	OmnString id;
	if (encryptType=="SH1")
	{
		//SH1
		OmnShouldNeverComeHere;
		return false;
	}
	else
	{
		//MD5
		id = AosMD5Encrypt(capcha);
	}
	aos_assert_r(id!="", false);
	
	OmnString sendmethod = config->getAttrStr("sendmethod", "email");
	bool rslt;
	rdata->setVerificationCode(capcha);
	if (sendmethod == "shortmessage")
	{
		rslt = sendShortMethod();
		if (!rslt) 
		{
			rdata->setError() << "Faild send Short Method!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else
	{
		rslt = sendEmailMethod(id, request, rdata);
		if (!rslt) 
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	rdata->setOk();
	return true;
}


bool
AosSdocVerCode::sendEmailMethod(
			const OmnString &id, 
			const AosXmlTagPtr &request, 
			const AosRundataPtr &rdata)
{
	OmnString subject;
	OmnString contents;
	AosUserAcctObjPtr user_acct = rdata->getRequesterAcct();
	aos_assert_rr(user_acct, rdata, false);
	AosXmlTagPtr userdoc = user_acct->getDoc();
	aos_assert_rr(userdoc, rdata, false);
	OmnString str;

	AosXmlTagPtr rootchild  = request->getFirstChild();
	if (!rootchild)
	{
		rdata->setError()<< "Request incorrect!" ;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}
	
	AosXmlTagPtr objdef = rootchild->getFirstChild("objdef");
	if (!objdef)
	{
		rdata->setError()<< "Missing objdef!" ;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}

	OmnString sdoc_objid = objdef->getAttrStr(AOSTAG_SMARTDOCS_EMAIL);
	OmnString email = objdef->getAttrStr(AOSTAG_EMAIL);
	if (sdoc_objid != "")
	{
		AosXmlTagPtr xml = userdoc->clone(AosMemoryCheckerArgsBegin);
		xml->setAttr("UID", id);
		if (email!="") xml->setAttr(AOSTAG_EMAIL, email);
		rdata->setSourceDoc(xml, true);

		// Chen Ding, 11/28/2012
		// AosSMDMgr::procSmartdocs(sdoc_objid, rdata);
		AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
		if (!rdata->isOk())
		{
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		str = rdata->getResults();
		rdata->setResults("");
		rdata->setError();
	}
	else
	{
		if (email =="") 
		{
			email= userdoc->getAttrStr(AOSTAG_EMAIL);
			if (email == "")
			{
				rdata->setError() << "Missing Email Address!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}

		subject = "邮箱验证";
		contents = "您的邮箱验证码是:";
		contents<<id <<"，请您将此验证码输入到校验框内，继续完成操作.";
		str << "<sendemail  rmailaddr =\"" << email << "\" "
			<< "subject =\"" << subject << "\" "
			<< "contents =\"" << contents << "\" />";
	}

	aos_assert_r(str!="", false);
	AosPyemail pc;
	AosXmlParser parser;
	AosXmlTagPtr emailxml = parser.parse(str, "" AosMemoryCheckerArgs);
	bool rslt = pc.sendmail(emailxml, rdata);
	if (!rslt)
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}
bool
AosSdocVerCode::sendShortMethod()
{
	OmnShouldNeverComeHere;
	return false;
}
#endif
