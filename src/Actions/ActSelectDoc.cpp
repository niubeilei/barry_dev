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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSelectDoc.h"

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
#include <string>
#include <vector>
using namespace std;

AosActSelectDoc::AosActSelectDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_SELECTDOC, AosActionType::eSelectDoc, flag)
{
}


AosActSelectDoc::~AosActSelectDoc()
{
}


bool	
AosActSelectDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_ZKY_TYPE=AOSACTTYPE_SELECTDOC
	// 		AOSTAG_LOCAL_VAR_NAME="xxx"
	// 		<AOSTAG_DOCSELECTOR .../>
	// 	</sdoc>
	// where "<docselector>" is a doc selector that selects a doc; AOSTAG_XPATH
	// specifies the attribute, "<value>" specifies how to retrieve the
	// value. 
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the source doc
	AosXmlTagPtr selector = sdoc->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
	 	AosSetError(rdata, AosErrmsgId::eSmartDocIncorrect);
	 	OmnAlarm << rdata->getErrmsg() << ". Def: " << sdoc->toString() << enderr;
	 	return false;
	}
	 
	AosDocSelObjPtr docselector = AosDocSelObj::getDocSelector();
	if (!docselector)
	{
		AosSetErrorU(rdata, "missing_docselector");
		OmnAlarm << rdata->getErrmsg() << ": " << sdoc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr targetDoc = AosRunDocSelector(rdata, selector);
	if (!targetDoc)
	{
		AosSetErrorU(rdata, "failed_retrieve_doc");
	 	return false;
	}

	bool rslt = rdata->setDoc(targetDoc, sdoc);
	if (!rslt)
	{
		AosSetError(rdata, "failed_set_doc") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSelectDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSelectDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


