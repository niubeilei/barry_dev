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
////////////////////////////////////////////////////////////////////////////
#include "Query/TermKeywords.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/XmlTag.h"


AosTermKeywords::AosTermKeywords(const bool regflag)
:
AosTermHitType(AOSTERMTYPE_KEYWORDS, AosQueryType::eContainer, regflag)
{
}


AosTermKeywords::AosTermKeywords(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermHitType(AOSTERMTYPE_KEYWORDS, AosQueryType::eContainer, false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_KEYWORD
	// 		ctnr_objid="xxx"
	// 		reverse="true|false" 
	// 		order="true|false">xxx</term>
	//
	// The new format is:
	// 	<term type=AOSTERMTYPE_KEYWORD reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_KEYWORD container="xxx">keyword,...</selector>
	// 		<cond .../>
	// 	</term>
	//
	OmnString ctnr_objid = def->getAttrStr("ctnr_objid");
	OmnString keywords = def->getNodeText();
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	mIsGood = parse(ctnr_objid, keywords, rdata);
}


AosTermKeywords::AosTermKeywords(
		const OmnString &ctnr_objid, 
		const OmnString &keyword,
		const bool reverse,
		const bool order,
		const AosRundataPtr &rdata)
:
AosTermHitType(AOSTERMTYPE_KEYWORDS, AosQueryType::eContainer, false)
{
	mOrder = order;
	mReverse = reverse;
	mIsGood = parse(ctnr_objid, keyword, rdata);
}


void
AosTermKeywords::toString(OmnString &str)
{
	//  <term iilname=AOSTERMTYPE_KEYWORD
	//  	reverse="xxx" 
	//  	order="xxx"><![CDATA[xxx]]></term>
	OmnString order = (mReverse)?"true":"false";
	OmnString reverse = (mReverse)?"true":"false";
	str << "<keyword type=\"" << AOSTERMTYPE_KEYWORDS
		<< "\" reverse=\"" << reverse
		<< "\" order=\"" << order;
	if (mCtnrObjid != "") str << "\" container=\"" << mCtnrObjid;
	str << "\"><![CDATA[" << mKeyword << "]]></keyword>";
}


bool
AosTermKeywords::parse(
		const OmnString &ctnr_objid,
		const OmnString &keywords,
		const AosRundataPtr &rdata)
{
	/*
	char wordbuff[AosXmlTag::eMaxWordLen+1];
	i64 wordLen = keywords.length();
	char * newWord = NULL;
	if (wordLen > 1) 
	{
		newWord = AosQueryReq::mWordNorm->normalizeWord(
				(char *)keywords.data(), wordLen, wordbuff, AosXmlTag::eMaxWordLen);
	}

	mKeyword = OmnString(newWord, wordLen);

	mCtnrObjid = ctnr_objid;
	if (ctnr_objid != "")
	{
		mIILName = AosIILName::composeContainerHitIILName(ctnr_objid, mIILName);
	}
	else
	{
		mIILName = mKeyword;
	}

	*/
	OmnNotImplementedYet;
	return true;
}


bool
AosTermKeywords::convertToXml(
		const OmnString &cond_str, 
		const AosXmlTagPtr &conds_tag, 
		const OmnString fields[5],
		const i64 &nn,
		const AosRundataPtr &rdata)
{
	// It is searching for tags:
	// 	tag[|$|reverse],tag[|$|reverse],...
	//
	// 	<term type=AOSTERMTYPE_KEYWORDS_AND reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_KEYWORDS container="xxx">tag</selector>
	// 	</term>
	aos_assert_r(fields[2] != "", false);
	AosXmlTagPtr cond_tag = conds_tag->addNode1("term");
	aos_assert_r(cond_tag, false);

	cond_tag->setAttr("type", AOSTERMTYPE_KEYWORDS_AND);
	cond_tag->setAttr("reverse", "false");
	cond_tag->setAttr("order", "false");

	//Johzi, 2011/09/06
	cond_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_keywords_h");

	AosXmlTagPtr selector_tag = cond_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_KEYWORDS);
	selector_tag->setNodeText(fields[2], true);

	return true;
}


bool
AosTermKeywords::convertToXml(
		const AosXmlTagPtr &conds_tag, 
		const OmnString &words,
		const AosRundataPtr &rdata)
{
	// It is searching for tags:
	// 	tag[|$|reverse],tag[|$|reverse],...
	//
	// 	<term type=AOSTERMTYPE_KEYWORDS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_KEYWORDS_AND container="xxx">tag</selector>
	// 		<cond .../>
	// 	</term>
	AosXmlTagPtr cond_tag = conds_tag->addNode1("term");
	aos_assert_r(cond_tag, false);

	cond_tag->setAttr("type", AOSTERMTYPE_KEYWORDS_AND);
	cond_tag->setAttr("reverse", "false");
	cond_tag->setAttr("order", "false");

	//Johzi, 2011/09/06
	cond_tag->setAttr("zky_ctobjid", "giccreators_querynew_term_keywords_h");

	AosXmlTagPtr selector_tag = cond_tag->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_KEYWORDS);
	selector_tag->setNodeText(words, true);

	return true;
}


bool
AosTermKeywords::convertQuery(
		const AosXmlTagPtr &term, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;

	/*
	term->setAttr("type", AOSTERMTYPE_KEYWORDS);
	term->setAttr("reverse", "false");
	term->setAttr("order", "false");
	OmnString words = term->getNodeText();
	aos_assert_r(term->removeTexts(), false);
	//1. add selector tag
	AosXmlTagPtr selector_tag = term->addNode1("selector");
	aos_assert_r(selector_tag, false);
	selector_tag->setAttr(AOSTAG_TYPE, AOSIILSELTYPE_KEYWORDS_AND);
    selector_tag->setNodeText(words, true);	
	*/

	return true;
}


AosQueryTermObjPtr
AosTermKeywords::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermKeywords(def, rdata);
}

