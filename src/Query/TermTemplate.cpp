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
// 08/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermTemplate.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/IILName.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosTermTemplate::AosTermTemplate(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_TEMPLATE, AosQueryType::eTemplate, false)
{
	OmnNotImplementedYet;
}


AosTermTemplate::AosTermTemplate(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_TEMPLATE, AosQueryType::eTemplate, false)
{
	// This constructor creates a new instance of the term. 'def' is the
	// XML definition of the term.
	OmnNotImplementedYet;
}


AosTermTemplate::~AosTermTemplate()
{
}


bool
AosTermTemplate::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermTemplate::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosTermTemplate::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the docids from the database. 
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermTemplate::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	OmnNotImplementedYet;
	return false;
}


i64		
AosTermTemplate::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	OmnNotImplementedYet;
	return -1;
}


void	
AosTermTemplate::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
	OmnNotImplementedYet;
}

void
AosTermTemplate::toString(OmnString &str)
{
	// This function converts the term into an XML.
	OmnNotImplementedYet;
}


AosQueryTermObjPtr
AosTermTemplate::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermTemplate(def, rdata);
}


bool 	
AosTermTemplate::nextDocid(
		const AosQueryTermObjPtr &and_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTermTemplate::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

