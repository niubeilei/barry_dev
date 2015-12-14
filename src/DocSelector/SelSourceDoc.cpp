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
#include "DocSelector/SelSourceDoc.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDocSelSourceDoc::AosDocSelSourceDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_SOURCE_DOC, AosDocSelectorType::eSourceDoc, reg)
{
}


AosDocSelSourceDoc::~AosDocSelSourceDoc()
{
}


AosXmlTagPtr 
AosDocSelSourceDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the created doc.
	return rdata->getSourceDoc();
}


OmnString 
AosDocSelSourceDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


