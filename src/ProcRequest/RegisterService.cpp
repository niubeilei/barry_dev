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
// 07/28/2011	Created by Jackie Zhao
////////////////////////////////////////////////////////////////////////////
#include "ProcRequest/RegisterService.h"

#include "SearchEngine/DocServerCb.h"
#include "Security/SecurityMgr.h"
#include "ProcRequest/ReqidNames.h"
#include "SEUtil/SeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"


AosRegisterService::AosRegisterService(const bool rflag)
:
AosProcRequest(AOSREQIDNAME_REGISTER_SERVICE, 
		AosProcReqid::eRegisterService, rflag)
{
}


bool 
AosRegisterService::proc(const AosRundataPtr &rdata)
{
	
	//<request>
	//  <item name="reqid"><![CDATA[register_service]]></item>
	//  <register machine_doc="xxx" machine_name="xxx" machine_ip="xxx" machine_port="xxx">
	//      <![CDATA[xxx]]>
	//  </register>
	//</request>
	AOSMONITORLOG_ENTER(rdata);
	rdata->getLog() << "->register_service";

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	//1.check the procservice has been register
	//2. if not register, it will register procservice on the procserver

	AosXmlTagPtr reg = root->xpathGetFirstChild("register");
	if(!reg)
	{
		rdata->setError() << "Missing the register tag !";
		AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	vector<machine> machines = mServer->getMachines();
	for(u32 i=0; i<machines.size(); i++)
	{
		if(machines[i].mMachineName == reg->getAttrStr("machine_name"))
		{
			rdata->setError() << "now procservice has existed";
			AOSMONITORLOG_FINISH(rdata);
			return false;
		}
	}
	machine m;
	m.mMachineName = reg->getAttrStr("machine_name");
	m.mMachineIp = reg->getAttrStr("machine_ip");
	m.mMachinePort = reg->getAttrInt("machine_port", -1);
	m.mMachineDoc = reg->getAttrStr("machine_doc");
	machines.push_back(m);
	mServer->setMachines(machines);


	//register service to seserver
	if(mServer->isSeServer())
	{
		OmnString objid;
		objid << reg->getAttrStr("machine_doc");
		//retrieve doc by objid
		AosXmlTagPtr doc;
		bool objflag = mServer->retrieveDocByObjid(objid, doc);
		if(!objflag)
		{
			rdata->setError() << "Fail to get the obj !";
			AOSMONITORLOG_FINISH(rdata);
			return false;
		}
		doc->setAttr("machine_name", reg->getAttrStr("machine_name"));
		doc->setAttr("status", "on");
		//modify machine doc
		bool rslt = mServer->modifyDocOnServer(doc);
		if(!rslt)
		{
			rdata->setError() << "Fail to update the doc on seserver!";
			AOSMONITORLOG_FINISH(rdata);
			return false;
		}
	}

	OmnString resp = "now procservice has been registered";
	rdata->setContents(resp);
	rdata->setOk();
	AOSMONITORLOG_FINISH(rdata);
	return true;
}

