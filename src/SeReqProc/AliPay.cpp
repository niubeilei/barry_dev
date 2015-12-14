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
//
// Modification History:
// 08/12/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/AliPay.h"

#include "SEInterfaces/DocClientObj.h"
#include "EventMgr/EventMgr.h"
#include "Rundata/RdataUtil.h"
#include "Security/SecurityMgr.h"
#include "SeReqProc/ReqidNames.h"
#include "SEModules/ObjMgr.h"
#include "SEServer/SeReqProc.h"
#include "SEBase/SeUtil.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


AosAliPay::AosAliPay(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_ALIPAY, AosSeReqid::eAliPay, rflag)
{
}


// bool
// AosSeReqProc::alipay(
// 		const AosXmlTagPtr &root, 
// 		const AosXmlTagPtr &rootchild, 
// 		const AosXmlTagPtr &cmd) 

bool 
AosAliPay::proc(const AosRundataPtr &rdata)
{
	// This function can proc all kinds of alipay req, such as "Login" "pay" "sendGood"
	//
	// 'root' should be in the form:
	// 	<request ...>
	// 		<command>
	// 			<cmd opr="alipayreturn"	
	// 				 alipay_opr="alipaycb"
	// 				 zky_docid="..." />	
	// 		</command>
	// 		<objdef> 
	//			...
	// 		</objdef>
	// 	</request>
	//

	AOSLOG_ENTER_R(rdata, false);

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	if (!cmd)
	{
		rdata->setError() << "Missing command";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// OmnString args = root->getChildTextByAttr("name", "args");
	// if (args == "")
	// {
	// 	errmsg = "Missing args!";
	// 	OmnAlarm << errmsg << enderr;
	// 	rdata->setError() << errmsg;
	// 	return false;
	// }

	OmnString alipay_opr = cmd->getAttrStr("alipay_opr", "");
	OmnString sobjid;

	if(alipay_opr == "alipaycb")
	{
		AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
		AosXmlTagPtr obj;
		if (!xmldoc || !(obj=xmldoc->getFirstChild()))
		{
			rdata->setError() << "Missing the object(Alipay)";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		
		//get order Doc
		OmnString docid = cmd->getAttrStr(AOSTAG_DOCID, "");
		bool duplicated = false;
		AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDoc(rdata, docid, "", duplicated);
		if(!doc)
		{
			rdata->setError() << "Retrieve obj error! by docid: " << docid << "(Alipay)!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}
		
		// this obj is come from alipay	
		// TSK001NOTE: need to verify this.
		// rdata->setReceivedDoc(obj);
		rdata->setReceivedDoc(root, true);
		sobjid = doc->getAttrStr("alipay_sdocObjid", "");
	}
	else
	{

		sobjid = cmd->getAttrStr(AOSTAG_SDOCOBJID, "");
		OmnString postSobjid = cmd->getAttrStr("zky_post_sdobjid", "");
		
		// get order doc
		OmnString objid = cmd->getAttrStr("zky_order_objid", "");
		AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
		if(!doc)
		{
			rdata->setError() << "Retrieve obj error! by objid: " << objid << "(Alipay)!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			AOSLOG_LEAVE(rdata);
			return false;
		}

		//modify the doc add the smartdoc
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		doc->setAttr("alipay_sdocObjid", sobjid);
		doc->setAttr("zky_post_sdobjid", postSobjid);
		
		//set super user
		u32 siteid = rdata->getSiteid();
		rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, "false", false);
		if(!rslt)
		{
			AOSLOG_LEAVE(rdata);
			return false;
		}
	}

	if (sobjid == "")	
	{
		rdata->setError() << "Missing smartdoc(Alipay)!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	// Chen Ding, 11/28/2012
	AosSmartDocObj::procSmartdocsStatic(sobjid, rdata);
	
	if (rdata->getResults() != "")
	{
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AOSLOG_LEAVE(rdata);
	return false;
}

