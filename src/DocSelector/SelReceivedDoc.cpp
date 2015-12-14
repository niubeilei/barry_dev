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
#include "DocSelector/SelReceivedDoc.h"

#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


AosDocSelReceivedDoc::AosDocSelReceivedDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_RECEIVED_DOC, AosDocSelectorType::eReceivedDoc, reg)
{
}


AosDocSelReceivedDoc::~AosDocSelReceivedDoc()
{
}

AosXmlTagPtr 
AosDocSelReceivedDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the created doc.
	//return rdata->getReceivedDoc();
	aos_assert_r(sdoc, 0);
	OmnString path = sdoc->getAttrStr("zky_doc_xpath", "");
	if (path == "")
	{
		return rdata->getReceivedDoc();
	}
	AosXmlTagPtr receivedoc = rdata->getReceivedDoc();
	aos_assert_r(receivedoc, 0);
	AosXmlTagPtr entries = receivedoc->xpathGetChild(path);
	return entries;

}

AosDocSelectorPtr
AosDocSelReceivedDoc::clone()
{
	AosDocSelector *pp = (AosDocSelector*) OmnNew AosDocSelReceivedDoc(false);
	return pp;
}


OmnString 
AosDocSelReceivedDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

