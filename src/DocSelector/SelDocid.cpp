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
#include "DocSelector/SelDocid.h"

#include "SEInterfaces/ValueSelObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"


AosDocSelDocid::AosDocSelDocid(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_BY_DOCID, AosDocSelectorType::eDocid, reg)
{
}


AosDocSelDocid::~AosDocSelDocid()
{
}


AosXmlTagPtr 
AosDocSelDocid::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects the doc identified by a docid specified in 'sdoc'.
	// 	<docselector type="xxx" AOSTAG_VALUE="xxx">
	// 		<AOSTAG_VALUEDEF .../>
	// 	</docselector>
	// where <AOSTAG_VALUEDEF> tag is optional.
	
	aos_assert_r(false, NULL);
	return NULL;
#if 0
	if (!sdoc)
	{
		rdata->setError() << "Missing smart doc";
		return 0;
	}

	u64 docid = sdoc->getAttrU64(AOSTAG_VALUE, 0);
	
	//lynch 2011/07/05
	if(docid == 0) 
	{
		// Retrieve the value
		// The docid is specified by a value selector:
		// 	<docselector type="xxx" AOSTAG_DOC_DOCID="xxx">
		// 		<AOSTAG_VALUEDEF .../>
		// 	</docselector>
		AosXmlTagPtr value_def = sdoc->xpathGetChild(AOSTAG_VALUEDEF);
		if (!value_def)
		{
			rdata->setError() << "Missing value definition";
			return 0;
		}

		AosValueRslt value;
		if (!AosValueSelObj::getValueStatic(value, value_def, rdata))
		{
			return 0;
		}

		// The data type cannot be XML DOC
		if (!value.isValid())
		{
			rdata->setError() << "Value is invalid";
			return 0;
		}
	
		if (value.isXmlDoc())
		{
			rdata->setError() << "Value is an XML doc";
			return 0;
		}
	
		if (!value.getU64Value(docid, rdata.getPtrNoLock())) return 0;

		if (docid == 0) 
		{
			rdata->setError() << "Missing docid!";
			return 0;
		}
	}
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);
	if (doc)
	{
		rdata->setOk();
	}
	else
	{
		rdata->setError() << "Failed retrieving the doc: " << docid;
	}
	rdata->setOk();
	doc = AosDocClientObj::getDocClient()->getDocByDocid(docid, rdata);

	OmnString path = sdoc->getAttrStr("zky_doc_xpath", "");
	if (path == "") return doc;
	
	aos_assert_r(doc, 0);
	AosXmlTagPtr entries = doc->xpathGetChild(path);
	return entries;
#endif
}


OmnString 
AosDocSelDocid::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

