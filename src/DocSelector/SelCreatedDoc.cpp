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
#include "DocSelector/SelCreatedDoc.h"
#include "Util/String.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDocSelCreatedDoc::AosDocSelCreatedDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_CREATED_DOC, AosDocSelectorType::eCreatedDoc, reg)
{
}


AosDocSelCreatedDoc::~AosDocSelCreatedDoc()
{
}

AosXmlTagPtr 
AosDocSelCreatedDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the created doc.
	return rdata->getCreatedDoc();
}


OmnString 
AosDocSelCreatedDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

