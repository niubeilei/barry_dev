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
// 04/13/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocRunAction.h"

#include "UtilComm/TcpClient.h"
#include "Actions/ActSeqno.h"
#include "Rundata/Rundata.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEModules/LoginMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/WebRequest.h"

AosSdocRunAction::AosSdocRunAction(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RUNACTION, AosSdocId::eRunAction, flag)
{
}


AosSdocRunAction::~AosSdocRunAction()
{
}


bool
AosSdocRunAction::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosXmlTagPtr actions = sdoc->getFirstChild(AOSTAG_ACTIONS);
	
	if (actions)
	{
		return AosSdocAction::runActions(actions, rdata);
	}

	return true;
}
