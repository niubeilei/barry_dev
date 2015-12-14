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
// This action sends a command to a remote server. 
//
// Modification History:
// 09/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSendCommand.h"

#include "Alarm/Alarm.h"
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
#include <string>
#include <vector>
using namespace std;

AosActSendCommand::AosActSendCommand(const bool flag)
:
AosSdocAction(AOSACTTYPE_SENDCOMMAND, AosActionType::eSendCommand, flag)
{
}


AosActSendCommand::~AosActSendCommand()
{
}


bool	
AosActSendCommand::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// 'sdoc' is in the form:
	// 	<sdoc ...>
	// 		<command .../>
	// 		...
	// 	</sdoc>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnNotImplementedYet;
	return true;
}


AosActionObjPtr
AosActSendCommand::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSendCommand(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


