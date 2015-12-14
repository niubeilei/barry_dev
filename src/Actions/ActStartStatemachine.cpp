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
// 01/03/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActTransState.h"

#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/ValueDefs.h"
#include "SEBase/SeUtil.h"
#include "Security/Ptrs.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "Statemachine/Statemachine.h"
#include "Util/DynArray.h"
#include "Util/OmnNew.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;



AosActTransState::AosActTransState(const bool flag)
:
AosSdocAction(AOSACTTYPE_START_STMC, AosActionType::eStartStmc, flag)
{
}


AosActTransState::~AosActTransState()
{
}


bool
AosActTransState::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	return AosStatemachine::startStatemachineBySdoc(sdoc, rdata);
}


AosActionObjPtr
AosActTransState::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActTransState(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


