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
// 08/11/2011	Created by Brian Zhang 
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/ShmConfirm.h"

#include "SeReqProc/ReqidNames.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "XmlUtil/XmlTag.h"


AosShmConfirm::AosShmConfirm(const bool rflag)
:
AosSeRequestProc(AOSREQIDNAME_SHMCONFIRM, AosSeReqid::eShmConfirm, rflag)
{
}


bool 
AosShmConfirm::proc(const AosRundataPtr &rdata)
{
	// This function is created by Brian Zhang 11/08/2011
	// root should be this format:
	//<request reqid = "shmcfrm" >
	//  <objdef>
	//  	<shm recevers="xxx">
	//  		<content>xxxx</content>
	//		</shm>
	//  </objdef>
	//</request>
	AOSLOG_ENTER_R(rdata, false);		

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSLOG_LEAVE(rdata);
		return false;
	}

	AosXmlTagPtr xmldoc = root->getFirstChild("objdef");
	if (!xmldoc)
	{
		rdata->setError() << "Missing object";
		OmnAlarm <<  rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	AosXmlTagPtr doc = xmldoc->getFirstChild();
	aos_assert_r(doc, false);

	OmnString receivers = doc->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receivers == "")
	{
		rdata->setError() << "Missing receivers";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	OmnString msg = doc->xpathQuery("contents/_#test");
	if (msg == "")
	{
		rdata->setError() << "Missing contents of message";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	// It need be inited by system but the user.
	OmnString shmCfmCode = doc->getAttrStr(AOSTAG_SHM_CFMCODE);
	if (shmCfmCode == "")
	{
		rdata->setError() << "Missing confirm code";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString shmSobjid = doc->getAttrStr(AOSTAG_SHM_SOBJID);
	if (shmSobjid == "")
	{
		rdata->setError() << "Missing smartdoc objid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosShortMsgClt::getSelf()->confirmThrShortmsg(receivers,
											msg, shmCfmCode, shmSobjid, rdata);
	if (!rslt)
	{
	    rdata->setError() << "Send short message failed!";
	    OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
	    return false;
	}
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}


bool
AosShmConfirm::sendShortMsgResp(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata)
{
	//<request reqid = "sdshm" >
	//<item name="needresp">noneed</item>
	//  <objdef>
	//  	<shm recevers="xxx">
	//  		<content>xxxx</content>
	//		</shm>
	//  </objdef>
	//</request>
	AOSLOG_ENTER_R(rdata, false);
	aos_assert_rr(doc, rdata, false);

	OmnString receivers = doc->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receivers == "")
	{
		rdata->setError() << "No receivers";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	OmnString msg = doc->xpathQuery("contents/_#test");
	if (receivers == "")
	{
		rdata->setError() << "No contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	bool rslt = AosShortMsgClt::getSelf()->sendShortMsg(receivers, msg, rdata);
	if (!rslt)
	{
		rdata->setError() << "Send short message failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return false;
}


bool
AosShmConfirm::sendShortMsgNoResp(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata)
{
	//root is this format:
	//<request reqid = "sdshm" >
	//<item name="needresp">need</item>
	//  <objdef>
	//  	<shm recevers="xxx" timeout="xxx">
	//  		<content>xxxx</content>
	//		</shm>
	//  </objdef>
	//</request>
	AOSLOG_ENTER_R(rdata, false);
	aos_assert_rr(doc, rdata, false);

	OmnString receivers = doc->getAttrStr(AOSTAG_SHM_RECEIVERS);
	if (receivers == "")
	{
		rdata->setError() << "No receivers";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	u64 timeout = doc->getAttrU64(AOSTAG_SHM_TIMEOUT, 0);
	if (timeout == 0)
	{
		rdata->setError() << "Define timeout error";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString msg = doc->xpathQuery("contents/_#test");
	if (receivers == "")
	{
		rdata->setError() << "No contents";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	AosXmlTagPtr resp;
	bool rslt = AosShortMsgClt::getSelf()->sendShortMsg(
								receivers, msg, resp, timeout, rdata);
	if (!rslt)
	{
		rdata->setError() << "Send short message failed";
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSLOG_LEAVE(rdata);
		return false;
	}
	
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return false;
}
