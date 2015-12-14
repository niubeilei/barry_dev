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
// This action sets a value to rundata:
//
// Modification History:
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSetRundataDoc.h"

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

AosActSetRundataDoc::AosActSetRundataDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_SETRUNDATA_DOC, AosActionType::eSetRundataDoc, flag)
{
}


AosActSetRundataDoc::~AosActSetRundataDoc()
{
}


bool	
AosActSetRundataDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action add an attribute to a doc. 
	// 	<sdoc AOSTAG_ZKY_TYPE=AOSACTTYPE_SETRUNDATA_DOC
	// 		AOSTAG_RUNDATA_VAR_NAME="xxx"	
	// 		<AOSTAG_DOCSELECTOR .../>
	// 	</sdoc>
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the var name
	OmnString varname = sdoc->getAttrStr(AOSTAG_RUNDATA_VAR_NAME);
	if (varname == "")
	{
		AosSetErrorU(rdata, "missing_varname") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the doc
	AosXmlTagPtr doc = AosRunDocSelector(rdata, sdoc, AOSTAG_DOCSELECTOR);
	if (!doc)
	{
		AosSetErrorU(rdata, "failed_select_doc") << ": " << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = rdata->setDocByVar(varname, doc, true);
	if (!rslt)
	{
		try
		{
			AosXmlTagPtr doc = doc->clone(AosMemoryCheckerArgsBegin);
			rslt = rdata->setDocByVar(varname, doc, true);
			if (!rslt)
			{
				AosSetErrorU(rdata, "missing_doc") << ": " << sdoc->toString();
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		}

		catch (...)
		{
			AosSetErrorU(rdata, "internal_error");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	rdata->setOk();
	return true;
}


AosActionObjPtr
AosActSetRundataDoc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSetRundataDoc(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


