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
#include "ProcRequest/ServiceCmds.h"

#include "SearchEngine/DocServerCb.h"
#include "Security/SecurityMgr.h"
#include "ProcRequest/ReqidNames.h"
#include "SEUtil/SeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "SmartDoc/SMDMgr.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"


AosServiceCmds::AosServiceCmds(const bool rflag)
:
AosProcRequest(AOSREQIDNAME_SERVICE_CMDS, 
		AosProcReqid::eServiceCmds, rflag)
{
}


bool 
AosServiceCmds::proc(const AosRundataPtr &rdata)
{
	
	//<request>
	//  <item name="reqid"><![CDATA[service_cmds]]></item>
	//  <cmds>
	//      <cmd type="getotherinfo" >
	//          <process machine_name="xxx"  process_name="xxx"/>
	//          <process machine_name="xxx"  process_name="xxx"/>
	//      </cmd>
	//      <cmd type="ctrlprocess" >
	//          <process machine_name="xxx" process_name="xxx" action="startup"/>
	//          <process machine_name="xxx" process_name="xxx" action="shutdown"/>
	//          <process machine_name="xxx" process_name="xxx" action="getinformation"/>
	//      </cmd>
	//  </cmds>
	//  <register machine_doc="xxx" machine_name="xxx" machine_ip="xxx" machine_port="xxx">
	//      <![CDATA[xxx]]>
	//  </register>
	//</request>
	AOSMONITORLOG_ENTER(rdata);
	rdata->getLog() << "->service_cmds";

	AosXmlTagPtr root = rdata->getRequestRoot();
	if (!root)
	{
		rdata->setError() << "Missing request";
		AOSMONITORLOG_FINISH(rdata);
		return false;
	}


	//1.check cmds tag
	//2.sort out cmds
	//3.send to procservice
	//4.collect the response
		
	AosXmlTagPtr cmds = root->getFirstChild("cmds");
	if(!cmds)
	{
		rdata->setError() << "Failed to get the cmds tag!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr cmd = cmds->getFirstChild("cmd");
	if(!cmd)
	{
		rdata->setError() << "Failed to get the cmd!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	map<OmnString, vector<AosXmlTagPtr> > cmds_map;
	//sort out cmds by machine name
	while(cmd)
	{
		OmnString opr = cmd->getAttrStr("type", "");
		if(opr == "ctrlprocess")
		{
			AosXmlTagPtr process = cmd->getFirstChild("process");
			if(!process)
			{
				rdata->setError() << "Failed to get the process!";
				AOSMONITORLOG_FINISH(rdata);
				return false;
			}
			while(process)
			{
				OmnString machine_name = process->getAttrStr("machine_name", "");
				if(machine_name != "")
				{
					map<OmnString, vector<AosXmlTagPtr> >::iterator it = cmds_map.find(machine_name);
					if(it == cmds_map.end())
					{
						vector<AosXmlTagPtr> temp;
						temp.push_back(process);
						cmds_map[machine_name] = temp;
					}
					else
					{
						it->second.push_back( process);
					}
				}
				process = cmd->getNextChild();
			}
		}
		else if(opr == "getotherinfo")
		{
			//need to implemented
			continue;
		}
		else if(opr == "")
		{
			OmnString errmsg = "Failed to get the operate type!";
			OmnAlarm << errmsg << enderr;
			continue;
		}
		cmd = cmds->getNextChild();
	}
	//response format:
	//<Contents>
	//	<record attr="xxx"/>
	//	<record attr="xxx"/>
	//		.
	//		.
	//		.
	//</Contents>
	//<status  error="true" />
	bool errflag = false;
	int count = 0;
	OmnString resp;
	map<OmnString, vector<AosXmlTagPtr> >::iterator it;
	for( it = cmds_map.begin(); it!=cmds_map.end(); it++)
	{
		OmnString request = "<request ><cmds>";
		int size =(int ) it->second.size();
		for (int i=0; i<size; i++)
		{
			AosXmlTagPtr process = (it->second)[i];
			request << "<cmd type=\"ctrlprocess\" >";
			request << process->toString();
			request << "</cmd>";
		}
		request << "</cmds></request>";
		OmnString machine_name = it->first;
		errflag = mServer->sendToProcService(request, machine_name, resp);
		if(errflag)
		{
			count++;
		}
OmnScreen << "machine_name : " << machine_name << "  resp1  :  " << resp.toString() << endl;
	}
OmnScreen << "resp -++++++++++++++++----:  " << resp.toString() << endl;
	rdata->setContents(resp);
	rdata->setOk();
	AOSMONITORLOG_FINISH(rdata);
	return true;
}

