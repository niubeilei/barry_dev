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
#include "Query/TermTagAnd.h"

#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Query/QueryReq.h"
#include "IILSelector/IILSelector.h"
#include "IILSelector/IILSelectorNames.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "WordParser/WordNorm.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"


AosTermTagAnd::AosTermTagAnd(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_TAG_AND, AosQueryType::eTagAnd, regflag)
{
}


AosTermTagAnd::AosTermTagAnd(
		const OmnString &ctnr_objid, 
		const OmnString &aname, 
		const OmnString &tag, 
		const bool reverse, 
		const bool order, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_TAG_AND, AosQueryType::eTagAnd, false)
{
	mReverse = reverse;
	mOrder = order;
	mContainer = ctnr_objid;
	mOpr = eAosOpr_an;
	mValue = "*";

	char wordbuff[AosXmlTag::eMaxWordLen+1];
	int tagLen = tag.length();
	char * newWord = AosQueryReq::smWordNorm->normalizeWord(
			(char *)tag.data(), tagLen, wordbuff, AosXmlTag::eMaxWordLen);
	mIILName = AosIILName::composeContainerAttrWordIILName(mContainer, aname, newWord);
	mIsGood = true;
}


void
AosTermTagAnd::toString(OmnString &str)
{
	// 	<term type="tag" 
	// 		container="xxx"
	// 		reverse="true|false"
	// 		order="true|false">xxx</term>
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<tag type=\"tag\" ";
	if (mCtnrObjid != "") str << "container=\"" << mContainer;
	str << "\" reverse=\"" << reverse 
		<< "\" order=\"" << order 
		<< "\"><![CDATA[" << mIILName << "]]></tag>";
}


AosQueryTermObjPtr
AosTermTagAnd::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosQueryTermObjPtr
AosTermTagAnd::addTerms(
		const AosQueryTermObjPtr &andterm,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS 
	// 			container="xxx"
	// 			aname="xxx"></selector>
	// 		<cond type="tags" zky_opr="ad|or">tag,tag,...</cond>
	// 	</term>
	aos_assert_rr(def, rdata, NULL);
	bool reverse = def->getAttrBool("reverse", false);
	bool order = def->getAttrBool("order", false);
	AosXmlTagPtr selector_tag = def->getFirstChild("selector");	
	if (!selector_tag) return 0;

	AosXmlTagPtr cond_tag = def->getFirstChild("cond");
	if (!cond_tag) 
	{
		AosSetError(rdata, AosErrmsgId::eMissingCondition);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	bool ignore_value, ignore_ctnr;
	OmnString xpath = AOSTAG_CTNR_DFT_VALUE;                   
	xpath << "/_#text";
	OmnString dftproc = def->getAttrStr(AOSTAG_CTNR_DFT_PROC);
	OmnString dftvalue = def->xpathQuery(xpath);
	OmnString tags = cond_tag->getNodeText();
	AosQrUtil::procQueryVars(tags, dftproc, dftvalue, ignore_value, rdata);

	if (ignore_value)
	{
		//if (tags == "") return 0;
		return 0;
	}
	OmnString container = selector_tag->getAttrStr("container");

	xpath = AOSTAG_CTNR_DFT_VALUE;
	xpath << "/_#text";
	dftproc = def->getAttrStr(AOSTAG_CTNR_DFT_PROC);
	dftvalue = def->xpathQuery(xpath);
	AosQrUtil::procQueryVars(container, dftproc, dftvalue, ignore_ctnr, rdata);

	OmnString aname = selector_tag->getAttrStr("aname");

	i64 guard = eMaxTags;
	AosQueryTermObjPtr first_term;

	AosWordParser wordParser;
    wordParser.setSrc(tags);
	OmnString word; 
    while (wordParser.nextWord(word) && guard--)
    {
		try
		{
			AosQueryTermObjPtr term = OmnNew AosTermTagAnd(
				container, aname, word, reverse, order, rdata);
			if (!first_term) first_term = term;
			andterm->addTerm(term, rdata);
		}

		catch (...)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateTerm);
			OmnAlarm << rdata->getErrmsg() << ". Condition: " << def->toString() << enderr;
			return 0;
		}
    }

	/*
	OmnStrParser1 parser(tags, ", ");
	OmnString word;
	while ((word = parser.nextWord()) != "" && guard--)
	{
		try
		{
			AosQueryTermObjPtr term = OmnNew AosTermTagAnd(
				container, word, reverse, order, rdata);
			if (!first_term) first_term = term;
			andterm->addTerm(term, rdata);
		}

		catch (...)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateTerm);
			OmnAlarm << rdata->getErrmsg() << ". Condition: " << def->toString() << enderr;
			return false;
		}
	}
	*/

	return first_term;
}

