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
// 11/17/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocConvertData.h"

#include "Actions/ActSeqno.h"
#include "EventMgr/EventTriggers.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "RemoteConvertData/RemoteConvertData.h"
#include "UtilComm/TcpClient.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosDocClientObjPtr AosSdocConvertData::smDocClient;

AosSdocConvertData::AosSdocConvertData(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_CONVERTDATA, AosSdocId::eConvertData, flag)
{
}


AosSdocConvertData::~AosSdocConvertData()
{
}

bool
AosSdocConvertData::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	/*
	//format
	//<sdoc>
	//<doc start="xxxx" end="xxxx"/>
	//<server 
	//	remote_addr="xxx.xxx.xxx.xxx" 
	//	remote_port="xxxx"/>
	//</sdoc>
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr doc = sdoc->getFirstChild("doc");
	aos_assert_rr(doc, rdata, false);
	u64 start = doc->getAttrU64("start", 0);
	aos_assert_rr(start!=0, rdata, false);
	u64 end = doc->getAttrU64("end", 0);
	aos_assert_rr(end!=0, rdata, false);
	aos_assert_rr(start <= end, rdata, false);
	AosXmlTagPtr server = sdoc->getFirstChild("server");
	aos_assert_rr(server, rdata, false);
	aos_assert_rr(AosRemoteConvertData::getSelf()->config(server), rdata, false);
	aos_assert_rr(AosRemoteConvertData::getSelf()->start(), rdata, false);

	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid!=0, rdata, false);

	//1. send cmd to server, change mode to the building mode ,clean all the data
	bool work_to_build = AosRemoteConvertData::getSelf()->sendServerReq(
			siteid, "convertdatacmd", "work_to_build", 0);
	aos_assert_rr(work_to_build, rdata, false);

	//2. send rebuild request
	AosRemoteConvertData::getSelf()->rebuildDb(start, end, rdata);

	//3. send cmd to server, change mode to the working mode
	bool build_to_work = AosRemoteConvertData::getSelf()->sendServerReq(
			siteid, "convertdatacmd", "build_to_work", 0);
	aos_assert_rr(build_to_work, rdata, false);
	*/
	return true;
}

