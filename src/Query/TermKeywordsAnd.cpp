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
////////////////////////////////////////////////////////////////////////////
#include "Query/TermKeywordsAnd.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "Query/TermKeywordsOr.h"
#include "QueryUtil/QrUtil.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "WordParser/WordNorm.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"


AosTermKeywordsAnd::AosTermKeywordsAnd(const bool regflag)
:
AosTermHitType(AOSTERMTYPE_KEYWORDS_AND, AosQueryType::eKeywordsAnd, regflag)
{
}


AosTermKeywordsAnd::AosTermKeywordsAnd(
		const OmnString &ctnr_objid, 
		const OmnString &aname,
		const OmnString &kwd, 
		const bool reverse, 
		const bool order, 
		const AosRundataPtr &rdata)
:
AosTermHitType(AOSTERMTYPE_KEYWORDS_AND, AosQueryType::eKeywordsAnd, false)
{
	// This constructor creates one term that has just one keyword.
	mReverse = reverse;
	mOrder = order;
	mCtnrObjid = ctnr_objid;
	mAname = aname;
	mOpr = eAosOpr_an;
	mValue = "*";

	if (mCtnrObjid != "") 
	{
		OmnString errmsg;
		bool rslt = AosObjid::procObjid(mCtnrObjid, rdata->getCid(), errmsg);
		if (!rslt)
		{
			AosSetError(rdata, errmsg);
			OmnAlarm << rdata->getErrmsg() << enderr;
			mIsGood = false;
			return;
		}
	}

	OmnString keyword = kwd;
	if (keyword == AOS_NULL_STRING) 
	{
		mIsGood = false;
		return;
	}

	bool ignore;
	AosQrUtil::procQueryVars(keyword, "", "", ignore, rdata);
	char wordbuff[AosXmlTag::eMaxWordLen+1];
	int len = keyword.length();
	char * newWord = AosQueryReq::smWordNorm->normalizeWord(
			(char *)keyword.data(), len, wordbuff, AosXmlTag::eMaxWordLen);
	if (newWord)
	{
		mIILName = AosIILName::composeContainerAttrWordIILName(mCtnrObjid, mAname, newWord);
	}
	mIsGood = (mIILName != "");
}


void
AosTermKeywordsAnd::toString(OmnString &str)
{
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<wordand type=\"wordand\" ";
	if (mCtnrObjid != "") str << "container=\"" << mCtnrObjid << "\" ";
	if (mAname != "") str << "aname=\"" << mAname;
	str << "\" reverse=\"" << reverse 
		<< "\" order=\"" << order 
		<< "\"><![CDATA[" << mIILName << "]]></tag>";
}


AosQueryTermObjPtr
AosTermKeywordsAnd::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosQueryTermObjPtr
AosTermKeywordsAnd::addTerms(
		const AosQueryTermObjPtr &andterm,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_xxx
	// 			container="xxx"
	// 			aname="xxx"></selector>
	// 		<cond type="keywords" zky_opr="ad|or">word,word,...</cond>
	// 	</term>
	aos_assert_rr(def, rdata, 0);
	bool reverse = def->getAttrBool("reverse", false);
	bool order = def->getAttrBool("order", false);
	AosXmlTagPtr selector_tag = def->getFirstChild("selector");	
	AosXmlTagPtr cond_tag = def->getFirstChild("cond");

	OmnString keywords; 
	if (cond_tag) keywords = cond_tag->getNodeText();
	if (keywords == "") return 0;

	OmnString container;
	OmnString aname;
	if (selector_tag) 
	{
		container = selector_tag->getAttrStr("container");
		aname = selector_tag->getAttrStr("aname");
	}

	//OmnStrParser1 parser(keywords, ", ");
	AosWordParser parser;
	parser.setSrc(keywords);
	OmnString word;
	i64 guard = eMaxWords;
	AosQueryTermObjPtr first_term;
	OmnString keywords1;
	while ((parser.nextWord(word)) && guard--)
	{
		try
		{
			AosQueryTermObjPtr term = OmnNew AosTermKeywordsAnd(
				container, aname, word, reverse, order, rdata);
			if (term->isGood())
			{
				andterm->addTerm(term, rdata);
				if (!first_term) first_term = term;
				keywords1 << word << ",";
			}
		}

		catch (...)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateTerm);
			OmnAlarm << rdata->getErrmsg() << ". Condition: " << def->toString() << enderr;
		}
	}
	rdata->setArg1("keywords", keywords1);

	return first_term;
}

