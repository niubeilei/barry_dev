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
#include "DocSelector/SelRetrievedDoc.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDocSelRetrievedDoc::AosDocSelRetrievedDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_RETRIEVED_DOC, AosDocSelectorType::eRetrievedDoc, reg)
{
}


AosDocSelRetrievedDoc::~AosDocSelRetrievedDoc()
{
}


AosXmlTagPtr 
AosDocSelRetrievedDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the created doc.
	return rdata->getRetrievedDoc();
}


OmnString 
AosDocSelRetrievedDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

