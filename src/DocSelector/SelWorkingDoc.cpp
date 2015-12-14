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
#include "DocSelector/SelWorkingDoc.h"

#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


AosDocSelWorkingDoc::AosDocSelWorkingDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_WORKING_DOC, AosDocSelectorType::eWorkingDoc, reg)
{
}


AosDocSelWorkingDoc::~AosDocSelWorkingDoc()
{
}

AosXmlTagPtr 
AosDocSelWorkingDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the work doc.
	//return rdata->getWorkingDoc();
	aos_assert_r(sdoc, 0);
	OmnString path = sdoc->getAttrStr("zky_doc_xpath", "");

	AosXmlTagPtr workingdoc = rdata->getWorkingDoc();
	if (path == "") return workingdoc;
	
	aos_assert_r(workingdoc, 0);
	AosXmlTagPtr entries = workingdoc->xpathGetChild(path);
	return entries;
}


OmnString 
AosDocSelWorkingDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

