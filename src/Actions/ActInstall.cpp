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
// 2014/11/16	Jozhi
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActInstall.h"

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
#include "JimoAgentMonitor/JimoInstall.h"
#include "JimoAgentMonitor/Ptrs.h"
#include <string>
#include <vector>
using namespace std;

AosActInstall::AosActInstall(const bool flag)
:
AosSdocAction(AOSACTTYPE_INSTALL, AosActionType::eInstall, flag)
{
}


AosActInstall::~AosActInstall()
{
}


bool	
AosActInstall::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//<action type="install" opr="install|update|uninstall" cluster_name="xxxx" pass="xxxx">
	//</action>
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString cluster_name = sdoc->getAttrStr("cluster_name");
	aos_assert_r(cluster_name != "", false);
	OmnString opr = sdoc->getAttrStr("opr");
	AosJimoInstallPtr install = OmnNew AosJimoInstall();
	install->init(cluster_name, rdata);
	if (opr == "install")
	{
		install->install();
	}
	else if (opr == "update")
	{
		install->update();
	}
	else if (opr == "uninstall")
	{
		install->unInstall();
	}
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActInstall::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActInstall(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

