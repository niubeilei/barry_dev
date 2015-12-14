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
// 06/14/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgClt/ShortMsgClt.h"

#include "AppMgr/App.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/SeXmlParser.h"
#include "ShortMsgUtil/ShmReqids.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgSvr/ShortMsgSvr.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "TransUtil/Ptrs.h"


// Chen Ding, 2013/06/09
// static AosShortMsgSvrPtr 			sgSmsvr;

OmnSingletonImpl(AosShortMsgCltSingleton,
                 AosShortMsgClt,
                 AosShortMsgCltSelf,
                "AosShortMsgClt");



AosShortMsgClt::AosShortMsgClt()
{
}


AosShortMsgClt::~AosShortMsgClt()
{
}


bool
AosShortMsgClt::start()
{
	return true;
}


bool
AosShortMsgClt::config(const AosXmlTagPtr &config)
{
	AosXmlTagPtr sms_config = config->getFirstChild("ShortMsgClt");
	if (!sms_config)
	{
		OmnAlarm << "No sms config!" << enderr;
		return false;
	}
	bool isLocal = sms_config->getAttrBool(AOSTAG_ISLOCAL, false);
	if (!isLocal)
	{
		AosXmlTagPtr child = sms_config->getFirstChild(AOSCONFIG_TRANS);
		aos_assert_r(child, false);
	}                                                                                   
	return true;
}


bool
AosShortMsgClt::stop()
{
    return true;
}


bool
AosShortMsgClt::sendShortMsg(
		const OmnString &receivers,
		const OmnString &msg,
		AosXmlTagPtr &resp,
		const u32 timer,
		const AosRundataPtr &rdata)
{
	// It constructs a request to send short message. 
	// 	<request 
	// 		receivers="xxx,xxx,...">
	// 		<msg .../>
	// 	</request>
	OmnString docstr = "<request ";
	docstr << AOSTAG_OPERATOR << "=\""
		<< AOSSHORTMSGREQ_SENDMSG << "\" "
		<< AOSTAG_SHM_RECEIVERS << "=\"" << receivers << "\" ndspe=\"true\">"
		<< "<msg><![CDATA[" << msg << "]]></msg>"
		<< "</request>";
	//AosXmlParser parser;
	//AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	//if (!xml)
	//{
	//	rdata->setError() << "Failed parsing the XML: " << docstr;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}

	// Ketty 2013/02/22
	OmnNotImplementedYet;
	return false;
	/*
	bool rslt;// = mTransClient->addTrans(rdata, mRobin->routeReq(0),
			//docstr.data(), docstr.length(), false, 0);
	aos_assert_r(rslt, false);

	if (!resp)
	{
		rdata->setError() << "Failed receiving the response";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	*/

	// The response should be in the form:
	// 	<response status="xxx"><![CDATA[error message]]></response>
	AosXmlTagPtr child;
	if (!(child = resp->getFirstChild()) && (child->getAttrStr(AOSTAG_STATUS) != "200"))
	{
		rdata->setError() << "Failed sending short message: "
			<< resp->getNodeText();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


bool
AosShortMsgClt::sendShortMsg(
		const OmnString &receivers,
		const OmnString &msg,
		const AosRundataPtr &rdata)
{
	// It constructs a request to send short message. 
	OmnString docstr = "<request ";
	docstr << AOSTAG_OPERATOR << "=\""
		<< AOSSHORTMSGREQ_SENDMSG << "\" "
		<< AOSTAG_SHM_RECEIVERS << "=\"" << receivers << "\" nedspe=\"false\">"
		<< "<msg><![CDATA[" << msg << "]]></msg>"
		<< "</request>";
	// Ketty 2013/02/22
	//AosXmlParser parser;
	//AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	//if (!xml)
	//{
	//	rdata->setError() << "Failed parsing the XML: " << docstr;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}

	// Chen Ding, 2013/06/09
	// Need to move the process to the server, regardless of whether it is
	// local or not.
	/* bool rslt;
	if(mIsLocal)
	{
		AosXmlParser parser;
		AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
		if (!xml)
		{
			rdata->setError() << "Failed parsing the XML: " << docstr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		rslt = sgSmsvr->proc(xml, rdata);
	}
	else
	{
		// Ketty 2013/02/22
		OmnNotImplementedYet;
		//rslt = mTransClient->addTrans(rdata, mRobin->routeReq(0),
		//		docstr.data(), docstr.length(), false, 0);
	}
	
	if(!rslt)
	{
		OmnAlarm << "Send message Failed!" << enderr;
		return false;
	}
	*/
	OmnNotImplementedYet;
	return false;
}


bool	
AosShortMsgClt::confirmThrShortmsg(
	const OmnString &receiver, 
	const OmnString &msg, 
	const OmnString &confirm_code,
	const OmnString &sdoc_objid, 
	const AosRundataPtr &rdata)
{
	// This function creates a short message confirmation request, 
	// it then sends the message to the user. If the user
	// responds, it will check the message to determine whether
	// it is confirmed or rejected. 
	//
	OmnString docstr = "<request ";
	docstr << AOSTAG_OPERATOR << "=\""
		<< AOSREQOPRSHORTMSGCONFIRM << "\" "
		<< AOSTAG_SHM_SOBJID << "=\"" << sdoc_objid << "\" "
		<< AOSTAG_SHM_RECEIVERS << "=\"" << receiver << "\">"
		<< "<msg><![CDATA[" << msg << "]]></msg>"
		<< "<confirmcode><![CDATA[" << confirm_code << "]]></confirmcode>"
		<< "</request>";
	// Ketty 2013/02/22
	//AosXmlParser parser;
	//AosXmlTagPtr xml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	//if (!xml)
	//{
	//	rdata->setError() << "Failed parsing the XML: " << docstr;
	//	OmnAlarm << rdata->getErrmsg() << enderr;
	//	return false;
	//}

	// Ketty 2013/02/22
	OmnNotImplementedYet;

	/*
	bool rslt;// = mTransClient->addTrans(rdata, mRobin->routeReq(0),
			//docstr.data(), docstr.length(), false, 0);
	if(!rslt)
	{
		OmnAlarm << "Send message Failed!" << enderr;
		return false;
	}
	*/
	return false;
}
