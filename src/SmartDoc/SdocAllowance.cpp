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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocAllowance.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosSdocAllowance::AosSdocAllowance(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_ALLOWANCE, AosSdocId::eAllowance, flag)
{
}


AosSdocAllowance::~AosSdocAllowance()
{
}


bool
AosSdocAllowance::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//req:
	//	<request ..>
	//		<item name="value">xxx</item>
	//		......
	//	</request>
	//	
	//sdoc:
	//  <doc
	//  	zky_otype = AOSOTYPE_SMARTDOC
	//      iilid="xxx"
	//      task_num = "xxx">
	//      ...
	//  </doc>
	
	/*
	 * Chen Ding, 08/12/2011
	 * Need to re-work.
	const OmnString siteid = rdata->getSiteid();
	const AosWebRequestPtr req = rdata->getWebRequest();
	AosXmlTagPtr sdoc = rdata->getSdoc();
	AosSessionPtr session = rdata->getSession();
	if (!session)
	{
		rdata->setError() << "User not logged in!";
		return false;
	}

	//1. get task_num value.
	//	if value > task_num, return.
    AosXmlTagPtr root = rdata->getReceivedDoc();
	AosXmlTagPtr child;
	if (!root || !(child = root->getFirstChild()))
    {
         rdata->setError() << "Failed to parse the request!";
		 OmnAlarm << rdata->getErrmsg() << enderr; 
		 return false;
	}

	//get node text.
	//<taskCount> xxxxx </taskCount>
	AosXmlTagPtr objdef = child->getFirstChild("objdef");
	objdef = objdef->getFirstChild("Contents");
	if (!objdef)
	{
		rdata->setError() << "Missing the task number!";
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;
	}
	OmnString value = objdef->getNodeText("taskCount");
	if (value == "") return true;
	u64 vv = (u64)atoi(value);
	if (vv <= 0) 
	{
		rdata->setError() << "Invalid task value!!!!! " << vv;
		return false;
	}

	u64 taskNum = sdoc->getAttrU64(AOSTAG_TASKNUM, AOS_INVDID);
	if (taskNum == AOS_INVDID)
	{
		rdata->setError() << "Failed to retrieve the task num: " << taskNum;
		return false;
	}

	if (taskNum < vv)
	{
		rdata->setError() << "Failed to Allowance the Task: " << taskNum;
		return false;
	}

	//2. get the iilid, and get the iil list.
	OmnString str = sdoc->getAttrStr(AOSTAG_IILNAME, "");
	if (str == "")
	{
		rdata->setError() << "no iilname! ";
		return false;
	}
	OmnString iilname;
	iilname << AOSZTG_ALLOWANCE << str;

	u64 userid = session->getUserid();
	time_t t = time(NULL);
	u64 crttime = t;
	u64 tvalue = tvalue & 0;
    tvalue = crttime;
    tvalue = tvalue << 32;
    tvalue = tvalue | vv;
	
	// Chen Ding, 05/27/2011
	// bool rslt = AosIILClientObj::getIILClient()->updateKeyedValue(
	//		iilname, true, userid, tvalue);
	bool rslt = AosIILClientObj::getIILClient()->setU64ValueDocUnique(iilname, userid, tvalue, rdata);
	if(!rslt)
	{
		rdata->setError() << "Failed to update iil: " 
			<< iilname << ":" << tvalue << ":" << userid;
		return false;
	}	

	//4. modify the smart doc.
	//if allocate task successful.
	taskNum -= vv;
	OmnString newNum;
	newNum << OmnStrUtil::itoa(taskNum);

	rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				sdoc->getAttrU64(AOSTAG_DOCID, 0),
				sdoc->getAttrStr(AOSTAG_OBJID),
				AOSTAG_TASKNUM, newNum, "", false, false, 
				__FILE__, __LINE__);
	aos_assert_r(rslt, "");
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

