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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelLocalVar.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDocSelLocalVar::AosDocSelLocalVar(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_LOCAL_VAR, AosDocSelectorType::eLocalVar, reg)
{
}


AosDocSelLocalVar::~AosDocSelLocalVar()
{
}


AosXmlTagPtr 
AosDocSelLocalVar::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the doc by a local var. 
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc";
		return 0;
	}

	OmnString varname = sdoc->getAttrStr(AOSTAG_VARNAME);
	if (varname == "")
	{
		rdata->setError() << "Missing Variable Name";
		return 0;
	}

	return rdata->getDocByVar(varname);
}


OmnString 
AosDocSelLocalVar::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


