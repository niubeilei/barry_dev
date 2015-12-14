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
// This doc selector selects a rundata doc.
//
// Modification History:
// 01/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelRdataDoc.h"

#include "Random/CommonValues.h"
#include "SEUtil/ValueDefs.h"
#include "Rundata/Rundata.h"
#include "Util/StrUtil.h"
#include "XmlUtil/XmlTag.h"

static vector<OmnString> sgValues;
static OmnMutex sgLock;
static bool sgInited = false;

AosDocSelRdataDoc::AosDocSelRdataDoc(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_RDATA_DOC, AosDocSelectorType::eRdataDoc, reg)
{
}


AosDocSelRdataDoc::~AosDocSelRdataDoc()
{
}


bool
AosDocSelRdataDoc::init()
{
	if (sgInited) return true;

	sgLock.lock();
	sgValues.push_back(AOSVALUE_RECEIVED_DOC);
	sgValues.push_back(AOSVALUE_RETRIEVED_DOC);
	sgValues.push_back(AOSVALUE_SOURCE_DOC);
	sgValues.push_back(AOSVALUE_TARGET_DOC);
	sgValues.push_back(AOSVALUE_CREATED_DOC);
	sgValues.push_back(AOSVALUE_WORKING_DOC);
	sgValues.push_back(AOSVALUE_SDOC);
	sgValues.push_back(AOSVALUE_OLD_DOC);
	sgValues.push_back(AOSVALUE_NEW_DOC);
	sgValues.push_back(AOSVALUE_NAMED_DOC);
	sgInited = true;
	sgLock.unlock();
	return true;
}


OmnString
AosDocSelRdataDoc::pickSource()
{
	if (!sgInited) init();
	int idx = rand() % sgValues.size();
	return sgValues[idx];
}


AosXmlTagPtr 
AosDocSelRdataDoc::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// It selects a doc from rdata. 
	// 	<docselector AOSTAG_ZKY_TYPE=AOSDOCSELTYPE_RDATA_DOC
	// 		AOSTAG_SOURCE=
	//				AOSVALUE_RECEIVED_DOC |
	//				AOSVALUE_RETRIEVED_DOC |
	//				AOSVALUE_SOURCE_DOC |
	//				AOSVALUE_TARGET_DOC |
	//				AOSVALUE_CREATED_DOC |
	//				AOSVALUE_WORKING_DOC |
	//				AOSVALUE_SDOC |
	//				AOSVALUE_OLD_DOC |
	//				AOSVALUE_NEQ_DOC |
	//				AOSVALUE_NAMED_DOC

	OmnString source = sdoc->getAttrStr(AOSTAG_SOURCE);
	switch (AosStrUtil::selectValue(source, 3, sgValues, 10))
	{
	case 0:
		 return rdata->getReceivedDoc();

	case 1:
		 return rdata->getRetrievedDoc();

	case 2:
		 return rdata->getSourceDoc();

	case 3: 
		 return rdata->getTargetDoc();

	case 4: 
		 return rdata->getCreatedDoc();

	case 5: 
		 return rdata->getWorkingDoc();

	case 6:
		 return rdata->getSdoc();

	case 7:
		 return rdata->getDocByVar(AOSARG_OLD_DOC);

	case 8:
		 return rdata->getDocByVar(AOSARG_NEW_DOC);

	case 9:
		 return rdata->getDocByVar(sdoc->getAttrStr(AOSTAG_DOCNAME));

	default:
		 break;
	}
	AosSetErrorUser(rdata, "docsel_rdatadoc_001") << ": " << source << enderr;
	return 0;
}


OmnString
AosDocSelRdataDoc::getXmlStr(
		const OmnString &tagname, 
		const OmnString &source,
		const OmnString &docname, 
		const AosRundataPtr &rdata)
{
	// This function generates the XMl string for this selector.
	aos_assert_r(tagname != "", "");
	OmnString docstr = "<";
	docstr << tagname << " ";

	OmnString src = source;
	if (src == "")
	{
		// Will randomly determine the source.
		src = pickSource();
	}

	docstr << AOSTAG_ZKY_TYPE << "=\"" << AOSDOCSELTYPE_RDATA_DOC 
		<< "\" " << AOSTAG_SOURCE << "=\"" << src << "\"";
	if (src == AOSVALUE_NAMED_DOC)
	{
		OmnString dname = docname;
		if (dname == "")
		{
			dname = AosCommonValues::pickDocName();
		}

		docstr << " " << AOSTAG_DOCNAME << "=\"" << dname << "\"";
	}
	docstr << "/>";
	return docstr;
}


OmnString 
AosDocSelRdataDoc::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}



