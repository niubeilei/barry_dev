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
// This type of commands run in one thread, either on all the machines
// or the selected machines. It sends the command to all the involved
// machines, which runs the command and then sends a response back 
// to the caller. 
//
// This proc will wait for a given amount of time, collecting resposnes.
// If all responses are received before the timer expires, it sends the
// response back to the requester. This finishes the command processing.
// Otherwise, it sends back whatever responses being collected and 
// indicates that the command has not been finished yet. 
//
// The requester can fetch the results later on. 
//   
//
// Modification History:
// 2013/08/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CommandProc/CmdProcSimple.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"                 
#include "Util/DynArray.h"               
#include "ValueSel/ValueSel.h"           
#include "XmlUtil/XmlTag.h"              
#include "XmlUtil/XmlDoc.h"              
#include "API/AosApiS.h"                 
//#include "JobTrans/RemoteCmdTrans.h"     
#include "AdminTrans/AdminRunCmdTrans.h"
#include <string>                        
#include <vector>                        

static AosJimoUtil::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosCmdProcSimple_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosCmdProcSimple(version);
	}

	catch (...)
	{
		AosSetError(rdata, "failed_create_jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosCmdProcSimple::AosCmdProcSimple(const AosCmdProcSimple &rhs)
:
AosCommandProc(rhs),
mNumResponded(0)
{
}


AosCmdProcSimple::AosCmdProcSimple(const u32 version)
:
AosCommandProc(rdata, version, "cmd_proc_simple"),
mNumResponded(0)
{
	if (!init(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool 
AosCmdProcSimple::run(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &cmd)
{
	// The cmd's format just like this.
	//                                                   
	// <action type="actcmd" machines=”xxx,xxx,…,xxx”, all="true/false">   
	//      <command_def cmd_id="xxx">
	//      	command 
	//      </command_def>                               
	// </action>                                         
	
	if (!cmd)
	{
		AosSetErrorUser(rdata, "internal_error") << enderr;
		return false;
	}

	// run command 
	bool runall = cmd->getAttrBool("all", true);
	if (runall)
	{
		return sendToAll(rdata, cmd);
	}

	return sendToSelected(rdata, cmd);
}



bool
AosCmdProcSimple::sendToSelected(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &cmd)
{
	return true;
}


bool
AosCmdProcSimple::sendToAll(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &cmd)
{
	aos_assert_rr(cmd, rdata, false);

	// 1. get serverids from attribute machines
	OmnString machines = cmd->getAttrStr("machines");
	if (machines == "")
	{
		AosSetErrorUser(rdata, "missing_server_ids") << cmd->toString() << enderr;
		return false;
	}

	AosStrSplit split(machines, ",");
	vector<OmnString> str_vector = split.entriesV();

	// 2. get the command 
	AosXmlTagPtr command = cmd->getFirstChild("command_def");
	if (!command)
	{
		AosSetErrorUser(rdata, "donot_have_command_def_node") << cmd->toString() << enderr;
		return false;
	}

	// Construct the transactions
	OmnString resp = "<Contents>";                                    
	OmnString cmdExeValue = "";
	OmnString cmdExeState = "false";

	vector<AosTransPtr> transReqs;        
	string cmd_str= command->getNodeText().data();
	if (cmd_str == "")
	{
		for (u32 k = 0; k < str_vector.size(); k++)
		{
			int server_id = str_vector[k].toInt();	
			resp << "<record zky_tip=\"serverid\" serverid=\""<< server_id 
				<< "\" cmdExeState=\"" << cmdExeState << "\" command=\"" << cmd_str << "\"><zky_task__n><![CDATA[";
			resp << cmdExeValue;
			resp << "]]></zky_task__n></record>";
		}
		resp << "</Contents>";
		OmnScreen << "================================" << endl 
			  << resp << endl;
		rdata->setResults(resp.data());
		return true;
	}

	int num_phys = AosGetNumPhysicals();
	for (u32 i=0; i<str_vector.size(); i++)
	{
		int server_id = str_vector[i].toInt();
		if (server_id < 0 || server_id >= num_phys)
		{
			AosSetErrorUser(rdata, "invalid_machine_id") << cmd->toString() << enderr;
			return false;
		}

		//AosTransPtr trans = OmnNew AosRemoteCmdTrans(cmd_str, server_id, false, false);
		AosTransPtr trans = OmnNew AosAdminRunCmdTrans(cmd_str, server_id, false, false);
		transReqs.push_back(trans);
	}

	AosSendTransSyncResp(rdata, transReqs);
	for (u32 i = 0; i < transReqs.size(); i++)
	{
		int server_id = str_vector[i].toInt();	
		AosBuffPtr returnResp = transReqs[i]->getResp();
		//OmnString cmdExeValue, cmdExeState = "false";
		if (returnResp)
		{
			OmnString strValues = returnResp->getOmnStr("");
			char state = strValues[0];
			cmdExeState = "false";
			if (state == 'T') cmdExeState = "true"; 
			cmdExeValue = strValues.substr(1, strValues.length()-1);
		}
		else
		{
			OmnAlarm << "Missing response: " << cmd->toString() << enderr;
			cmdExeValue ="Machine not responding";
		}

		resp << "<record zky_tip=\"serverid\" serverid=\""<< server_id 
			<< "\" cmdExeState=\"" << cmdExeState << "\" command=\"" << cmd_str << "\"><zky_task__n><![CDATA[";
		resp << cmdExeValue;
		resp << "]]></zky_task__n></record>";
	}
	resp << "</Contents>";
	OmnScreen << "================================" << endl 
			  << resp << endl;
	rdata->setResults(resp.data());
	//rdata->setOk();
	return true;

	//int timer = cmd->getAttrInt("timer", smDefaultTimer);
	//if (timer < eMiniTimer) timer = eMiniTimer;
	//if (timer > eMaxTimer) timer = eMaxTimer;

	//u32 start_sec = OmnGetSecond();	
	//u32 crt_sec = start_sec;
	//mNumResponded = 0;

	//int num_phys = AosGetNumPhysicals();
	//if (num_phys <= 0)
	//{
	//	AosSetErrorUser(rdata, "internal_error") << enderr;
	//	return false;
	//}

	//for (int i=0; i<num_phys; i++)
	//{
	//	// Create the transaction and send it.
	//}

	//while (crt_sec < start_sec + timer)
	//{
	//	mLock->lock();
	//	if (mNumResponded == num_phys)
	//	{
	//		// All have responded. 
	//		return constructRespond(rdata);
	//	}
	//	bool timed_out;
	//	mCondVar->timedWait(mLock, timed_out, 1, 0);
	//	mLock->unlock();
	//	crt_sec = OmnGetSecond();
	//}

	//return constructResponse(rdata);
}


bool
AosCmdProcSimple::constructResponse(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosCmdProcSimple::comandFinished(
		const AosRundataPtr &rdata,
		const int py_id,
		const bool status,
		const OmnString &results)
{
	/*
	<Contents complete="true|false">
		<machine id="0" .../>
		<machine id="0" .../>
	</Contents>
	*/

	return true;
}


bool
AosCmdProcSimple::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool                    
AosCmdProcSimple::registerMethods()
{   
    sgLock.lock();      
    if (sgInited)       
    {
        sgLock.unlock();
        return true;    
    }                   

    sgInited = true;
    sgLock.unlock();
    return true;
}   


bool 
AosCmdProcSimple::run(const AosRundataPtr &rdata)   
{   
	OmnNotImplementedYet;
	return false;       
}                       


AosJimoPtr
AosCmdProcSimple::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) const            
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	try
	{
		return OmnNew AosCmdProcSimple(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
	return 0;
}


bool
AosCmdProcSimple::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


void *
AosCmdProcSimple::getMethod(                                
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}                       
     
#endif

