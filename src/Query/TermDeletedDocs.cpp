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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 08/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermDeletedDocs.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/IILName.h"
#include "Query/IILSelTypes.h"
#include "QueryCond/QueryCond.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosTermDeletedDocs::AosTermDeletedDocs(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_DELETED_DOCS, AosQueryType::eDeletedDocs, false)
{
}


AosTermDeletedDocs::AosTermDeletedDocs(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_DELETED_DOCS, AosQueryType::eDeletedDocs, false)
{
	if (!parse(def, rdata))
	{
		OmnExcept e(__FILE__, __LINE__, rdata->getErrmsg());
		throw e;
	}
}


AosTermDeletedDocs::~AosTermDeletedDocs()
{
}


bool
AosTermDeletedDocs::parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// It assumes 'def':
	// 	<term type=AOSTERMTYPE_DELETED_DOCS order="true|false" reverse="true|false">
	// 		<iilselector type=AOSIILSEL_DELETED_DOCS/>
	// 		<cond .../>
	// 	</term>
	aos_assert_rr(def, rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid != 0, rdata, false);

	mIILName = AosIILName::composeDeletedObjidIILName();
	aos_assert_rr(mIILName != "", rdata, false);

	// mIILId = AosIILClient::getSelf()->getIILId(iilname, eAosIILType_Str, true, false, rdata);
	mCond = AosQueryCond::parseCondStatic(def, rdata);
	return true;
}


void
AosTermDeletedDocs::toString(OmnString &str)
{
	// This function converts the term into an XML.
	// 	<term type=AOSTERMTYPE_DELETED_DOCS order="true|false" reverse="true|false">
	// 		<iilselector type=AOSIILSEL_DELETED_DOCS/>
	// 		<cond .../>
	// 	</term>
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "\n<term type=\"" << AOSTERMTYPE_DELETED_DOCS
		<< "\" order=\"" << order
		<< "\" reverse=\"" << reverse
		<< "\">"
		<< "<iilselector type=\"" << AOSIILSELTYPE_DELETED_DOCS << "\"/>";
		
	if (mCond)
	{
		mCond->toString(str);
	}
	str << "</term>";
}


AosQueryTermObjPtr
AosTermDeletedDocs::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermDeletedDocs(def, rdata);
}

