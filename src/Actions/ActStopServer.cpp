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
// This action sets an attribute:
// 	<action zky_type="AOSACTION_SETATTR" xpath="xxx">
// 		<doc .../>
// 		<value .../>
// 	</action>
// 	where <doc> is a Doc Selection tag. <value> is a Value Tag. 
// 	This is the value to be set.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActStopServer.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SysMsg/StopProcessMsg.h"
#include <string>
#include <vector>
using namespace std;

AosActStopServer::AosActStopServer(const bool flag)
:
AosSdocAction(AOSACTTYPE_STOPSERVER, AosActionType::eStopServer, flag)
{
}


AosActStopServer::~AosActStopServer()
{
}


bool	
AosActStopServer::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<action type="stopserver" signal_no="14">
	//</action>
	//stop all server's process
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	int signal_no = sdoc->getAttrInt("signal_no", 14);
	vector<u32> svr_ids = AosGetServerIds();
	for (u32 i=0; i<svr_ids.size(); i++)
	{
		//AosTransPtr trans = OmnNew AosAdminStopProcessTrans(signal_no, svr_ids[i], false, false);
		//AosSendTrans(rdata, trans);
		AosAppMsgPtr msg = OmnNew AosStopProcessMsg(signal_no, svr_ids[i], 0);
		AosSendMsg(msg);
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActStopServer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActStopServer(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

