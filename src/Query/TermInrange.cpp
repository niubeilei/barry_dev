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
//
// Modification History:
// 01/05/2010 Created by Chen Ding
// 08/08/2011 Assigned to Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "Query/TermInrange.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "Query/QueryTerm.h"
#include "QueryCond/QueryCond.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static AosXmlParser    sgParser;


AosTermInrange::AosTermInrange(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_RANGE, AosQueryType::eRange, regflag)
{
}


AosTermInrange::AosTermInrange(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_RANGE, AosQueryType::eRange, false)
{
	// The old format is:
	// 	<term type="range" left="(|[" right=")|]" reverse="true">
	// 		<aname><![CDATA[xxx]]></aname>	
	// 		<min><![CDATA[xxx]]></min>	
	// 		<max><![CDATA[xxx]]></max>	
	// 	</term>
	//
	// The new format is:
	// 	<term type=AOSTERMTYPE_RANGE reverse="true|false" order="true|false">
	// 		<selector .../>
	// 		<cond type="xxx" ...>
	// 			<min inclusive="true|false"><![CDATA[xxx]]></min>	
	// 			<max inclusive="true|false"><![CDATA[xxx]]></max>	
	// 		</cond>
	// 	</term>
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	mCond = AosQueryCond::parseCondStatic(def, rdata);
	mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
}


void
AosTermInrange::toString(OmnString &str)
{
	// The form is :
	// <term iilname="xxx"
	// 		type=AOSTERMTYPE_RANGE
	//           reverse="true|false"
	//           order="true|false">
	//     <cond opr="xxx">
	//     	  <min inclusive="true|false"><!CDATA[[xxx]]></min>
	//     	  <max inclusive="true|false"><!CDATA[[xxx]]></max>
	//     </cond>
	// </term>
	OmnString cond;
	if (mCond)
	{
		mCond->toString(cond);
		if (cond == "")
		{
			OmnAlarm << "Condition error" << enderr;
			return;
		}
	}

	str << "<term type=\"" << AOSTERMTYPE_RANGE 
		<< "\" iilname=\"" << mIILName
		<< "\" reverse=\"" << mReverse
		<< "\" order=\"" << mOrder
		<< "\" >"
		<< cond
		<< "</term>";
}


bool
AosTermInrange::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosTermInrange::convertToXml(
		const OmnString &cond_str,
		AosXmlTagPtr &conds,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;

	aos_assert_r(cond_str != "", false);
	aos_assert_r(conds, false);

	AosXmlTagPtr conds_tag = sgParser.parse(cond_str, "" AosMemoryCheckerArgs);
	aos_assert_r(conds_tag, false);

	conds  = conds->addNode(conds_tag);
	aos_assert_r(conds ,false);
	
	return true;
}


AosQueryTermObjPtr
AosTermInrange::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermInrange(def, rdata);
}

