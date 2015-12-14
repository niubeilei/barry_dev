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
// This condition is used to search for tags. There is an IIL for each
// tag:
// 	TagIIL: <docid> <docid> ... <docid>
// The IIL is sorted based on docids. 
//
// Modification History:
// 08/07/2011	Created by Chen Ding
// 08/08/2011	Assigned to Linda Lin
////////////////////////////////////////////////////////////////////////////
#include "Query/TermContainer.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "SEUtil/IILName.h"
#include "IILSelector/IILSelector.h"
#include "Rundata/Rundata.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/XmlTag.h"


AosTermContainer::AosTermContainer(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_CONTAINER, AosQueryType::eContainer, regflag)
{
}


AosTermContainer::AosTermContainer(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_CONTAINER, AosQueryType::eContainer, false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_CONTAINER reverse="true|false" order="true|false">
	// 		<selector type=AOIILSELTYPE_CONTAINER
	// 			container="xxx"
	// 			aname="xxx"/>
	// 		<cond .../>
	// 	</term>
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	mOpr = eAosOpr_an;
	mIsGood = true;
}


AosTermContainer::AosTermContainer(
		const OmnString &ctnr_objid, 
		const OmnString &aname,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_CONTAINER, AosQueryType::eContainer, false)
{
	mIILName = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	mOrder = order;
	mReverse = reverse;
	mIsGood = true;
	mOpr = eAosOpr_an;
}


void
AosTermContainer::toString(OmnString &str)
{
	//  <term iilname=AOSTERMTYPE_CONTAINER 
	//  	reverse="xxx" 
	//  	order="xxx"><![CDATA[xxx]]></term>
	OmnString order = (mReverse)?"true":"false";
	OmnString reverse = (mReverse)?"true":"false";
	str << "<term " 
		<< "\" reverse=\"" << reverse
		<< "\" order=\"" << order
		<< "\">"
		<< "<selector " << AOSTAG_TYPE << "=\"" << AOSTERMTYPE_CONTAINER 
		<< "\"><![CDATA[" << mIILName << "]]></term>";
}


bool
AosTermContainer::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	// 'container' is specified as:
	// 	container|$|=|$|<container_name>
	if (nn < 3 || fields[2] == "")
	{
		OmnAlarm << "Query syntax error (container is missing): " << cond_str << enderr;
		return false;
	}
	
	OmnString ctnr_objid = fields[2];

	// It is for container
	// 1. Add the term tag
	AosXmlTagPtr term_tag = conds_tag->addNode1("term");
	aos_assert_r(term_tag, false);
	term_tag->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	term_tag->setAttr("reverse", "false");
	term_tag->setAttr("order", "false");
	//Johzi, 2011/08/26
	term_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_h");

	// 2. Add the selector tag
	AosXmlTagPtr selector_tag = term_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_ATTR);
	selector_tag->setAttr("container", ctnr_objid);

	// 3. Add the condition
	AosXmlTagPtr cond_tag = term_tag->addNode1("cond");
	aos_assert_r(cond_tag, false);
	cond_tag->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	cond_tag->setAttr(AOSTAG_OPR, "an");

	return true;
}


bool
AosTermContainer::convertToXml(
		const AosXmlTagPtr &conds_tag, 
		const OmnString &ctnrs,
		const AosRundataPtr &rdata)
{
	// 'container' is specified as:
	// 	container|$|=|$|<container_name>
	// It is for container
	// 1. Add the term tag
	AosXmlTagPtr ctnr_tag = conds_tag->addNode1("term");
	aos_assert_r(ctnr_tag, false);
	ctnr_tag->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	ctnr_tag->setAttr("reverse", "false");
	ctnr_tag->setAttr("order", "false");
	//Johzi, 2011/08/26
	ctnr_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_h");


	// 2. Add the selector tag
	AosXmlTagPtr selector_tag = ctnr_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_ATTR);
	selector_tag->setAttr("container", ctnrs);

	// 3. Add the condition
	AosXmlTagPtr cond_tag = ctnr_tag->addNode1("cond");
	aos_assert_r(cond_tag, false);
	cond_tag->setAttr(AOSTAG_TYPE, AOSTERMTYPE_ARITH);
	cond_tag->setAttr(AOSTAG_OPR, "an");
	return true;
}


bool
AosTermContainer::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermContainer::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermContainer(def, rdata);
}



