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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermTag.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "IILSelector/IILSelector.h"
#include "IILSelector/IILSelectorNames.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/XmlTag.h"


AosTermTag::AosTermTag(const bool regflag)
:
AosTermHitType(AOSTERMTYPE_TAG, AosQueryType::eTag, regflag)
{
}


AosTermTag::AosTermTag(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosTermHitType(AOSTERMTYPE_TAG, AosQueryType::eTag, false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS container="xxx">tag,tag,...</selector>
	// 	</term>
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	mIsGood = AosIILSelector::resolveIIL(def, mIILName, rdata);
	mIsGood = true;
}


AosTermTag::AosTermTag(
		const OmnString &ctnr_objid, 
		const OmnString &tags, 
		const bool reverse, 
		const bool order, 
		const AosRundataPtr &rdata)
:
AosTermHitType(AOSTERMTYPE_TAG, AosQueryType::eTag, false)
{
	mReverse = reverse;
	mOrder = order;
	mIsGood = parse(ctnr_objid, tags, rdata);
}


bool
AosTermTag::parse(
		const OmnString &ctnr_objid, 
		const OmnString &tags, 
		const AosRundataPtr &rdata)
{
	/*
	mTags = tags;
	mCtnrObjid = ctnr_objid;

	char wordbuff[AosXmlTag::eMaxWordLen+1];
	i64 tagLen = tags.length();
	char * newWord = AosQueryReq::mWordNorm->normalizeWord(
			(char *)tags.data(), tagLen, wordbuff, AosXmlTag::eMaxWordLen);
	mIILName = AosIILName::composeTagIILName(ctnr_objid, OmnString(newWord,tagLen));
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


void
AosTermTag::toString(OmnString &str)
{
	// 	<term type="tag" 
	// 		container="xxx"
	// 		reverse="true|false"
	// 		order="true|false">xxx</term>
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<tag type=\"tag\" ";
	if (mCtnrObjid != "") str << "container=\"" << mCtnrObjid;
	str << "\" reverse=\"" << reverse 
		<< "\" order=\"" << order 
		<< "\"><![CDATA[" << mTags << "]]></tag>";
}


bool
AosTermTag::convertToXml(
		const OmnString &cond_str,
		const AosXmlTagPtr &conds_tag,
		const OmnString fields[5], 
		const i64 &nn, 
		const AosRundataPtr &rdata)
{
	// It is searching for tags:
	// 	tag[|$|reverse],tag[|$|reverse],...
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS container="xxx">tag,tag,...</selector>
	// 	</term>
	aos_assert_r(fields[2] != "", false);
	AosXmlTagPtr cond_tag = conds_tag->addNode1("term");
	aos_assert_r(cond_tag, false);

	cond_tag->setAttr("type", AOSTERMTYPE_TAG_AND);
	cond_tag->setAttr("reverse", "false");
	cond_tag->setAttr("order", "false");

	//Johzi, 2011/09/06
	cond_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_tagand_h");

	AosXmlTagPtr selector_tag = cond_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_TAGS);
	selector_tag->setNodeText(fields[2], true);

	return true;
}

bool
AosTermTag::convertToXml(
		const AosXmlTagPtr &conds_tag,
		const OmnString &tags, 
		const AosRundataPtr &rdata)
{
	// It is searching for tags:
	// 	tag[|$|reverse],tag[|$|reverse],...
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS container="xxx">tag,tag,...</selector>
	// 		<cond .../>
	// 	</term>
	AosXmlTagPtr cond_tag = conds_tag->addNode1("term");
	aos_assert_r(cond_tag, false);

	cond_tag->setAttr("type", AOSTERMTYPE_TAG_AND);
	cond_tag->setAttr("reverse", "false");
	cond_tag->setAttr("order", "false");
	
	//Johzi, 2011/09/06
	cond_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_tagand_h");

	AosXmlTagPtr selector_tag = cond_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_TAGS);
	selector_tag->setNodeText(tags, true);

	return true;
}


bool
AosTermTag::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


AosQueryTermObjPtr
AosTermTag::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermTag(def, rdata);
}

