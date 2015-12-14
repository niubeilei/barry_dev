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
#include "Query/TermKeywordsOr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "IILSelector/IILSelectorNames.h"
#include "SEUtil/IILName.h"
#include "Query/QueryReq.h"
#include "WordParser/WordParser.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "Util/File.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosTermKeywordsOr::AosTermKeywordsOr(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_KEYWORDS_OR, AosQueryType::eKeywordsOr, regflag),
mDataLoaded(false)
{
}


AosTermKeywordsOr::AosTermKeywordsOr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_KEYWORDS_OR, AosQueryType::eKeywordsOr, false),
mDataLoaded(false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS 
	// 			container="xxx"
	// 			aname="xxx"></selector>
	// 		<cond type="tags" zky_opr="ad|or">keyword,keyword,...</cond>
	// 	</term>
	mReverse = def->getAttrBool("reverse", false);
	mOrder = def->getAttrBool("order", false);
	AosXmlTagPtr selector_tag = def->getFirstChild("selector");
	if (!selector_tag)
	{
		rdata->setError() << "Missing the selector!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		mIsGood = false;
		return;
	}
	else
	{
		mContainer = selector_tag->getAttrStr("container");
		mAttrname = selector_tag->getAttrStr("aname");
	}

	OmnString keywords;
	AosXmlTagPtr cond_tag = def->getFirstChild("cond");
	if (!cond_tag)
	{
		keywords = selector_tag->getNodeText();
		if (keywords == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingCondition);
			OmnAlarm << rdata->getErrmsg() << ". Query: " << def->toString() << enderr;
			mIsGood = false;
			return;
		}
	}
	else
	{
		keywords = cond_tag->getNodeText();
		if (keywords == "")
		{
			keywords = selector_tag->getNodeText();
		}

		if (keywords == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingCondition);
			OmnAlarm << rdata->getErrmsg() << ". Query: " << def->toString() << enderr;
			mIsGood = false;
			return;
		}
	}

	mIsGood = parse(keywords, rdata);
}


AosTermKeywordsOr::~AosTermKeywordsOr()
{
}


bool
AosTermKeywordsOr::parse(
		const OmnString &keywords, 
		const AosRundataPtr &rdata)
{
	// 'keywords' is in the form:
	// 		word,word,...
	if (mContainer != "") 
	{
		OmnString errmsg;
		bool rslt = AosObjid::procObjid(mContainer, rdata->getCid(), errmsg);
		if (!rslt)
		{
			AosSetError(rdata, errmsg);
			OmnAlarm << rdata->getErrmsg() << enderr;
			mIsGood = false;
			return false;
		}
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
		char wordbuff[AosXmlTag::eMaxWordLen+1];
		int len = word.length();
		char * newWord = AosQueryReq::smWordNorm->normalizeWord(
			(char *)word.data(), len, wordbuff, AosXmlTag::eMaxWordLen);

		// Chen Ding, 12/08/2011
		if (newWord && strcmp(newWord, AOS_NULL_STRING))
		{
			mIILNames.push_back(AosIILName::composeContainerAttrWordIILName(
						mContainer, mAttrname, newWord));

			keywords1 << word << ",";
		}
	}

	rdata->setArg1("keywords", keywords1);
	mIsGood = (mIILNames.size() > 0);
	return mIsGood;
}


void
AosTermKeywordsOr::toString(OmnString &str)
{
	// 	<term type="tag" 
	// 		container="xxx"
	// 		reverse="true|false"
	// 		order="true|false">xxx</term>
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<keywordor type=\"wordor\" ";
	if (mContainer != "") str << "container=\"" << mContainer << "\" ";
	if (mAttrname != "") str << "aname=\"" << mAttrname << "\" ";
	str << "reverse=\"" << reverse 
		<< "\" order=\"" << order
		<< "\"><![CDATA[";

	for (u32 i=0; i<mIILNames.size(); i++)
	{
		if (i > 0) str << ",";
		str << mIILNames[i];
	}
	str << "]]></tag>";
}


AosQueryTermObjPtr
AosTermKeywordsOr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 	
AosTermKeywordsOr::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) 
	{
		docid = AOS_INVDID;
		finished = true;
		return true;
	}
	return mQueryRslt->nextDocid(docid, finished, rdata);
}


bool 	
AosTermKeywordsOr::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->moveTo(startidx, mNoMoreDocs, rdata);
}


i64		
AosTermKeywordsOr::getTotal(const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->getTotal(rdata);
}


bool
AosTermKeywordsOr::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


bool
AosTermKeywordsOr::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// It checks whether the doci 'docid' can be selected by this term.
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return false;
	return mQueryRslt->checkDocid(docid, rdata);
}


void	
AosTermKeywordsOr::reset(const AosRundataPtr &rdata)
{
	if (!mQueryRslt) return;
	mQueryRslt->reset();
}


bool
AosTermKeywordsOr::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	mDataLoaded = true;
	if (!mQueryRslt)
	{
		mQueryRslt = AosQueryRsltObj::getQueryRsltStatic();
	}

	if (!mBitmap)
	{
		// mBitmap = AosBitmapObj::getBitmapStatic();
		mBitmap = AosGetBitmap();
	}

	if(!mQueryContext)
	{
		mQueryContext = AosQueryContextObj::createQueryContextStatic();
	}
	mQueryContext->setReverse(mReverse);
	return getDocidsFromIIL(mQueryRslt, mBitmap, mQueryContext, rdata);
}

bool 	
AosTermKeywordsOr::getRsltSizeFromIIL(const AosRundataPtr &rdata)
{
	if(!mQueryContext) mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->setOpr(eAosOpr_an);
	
	//bool rslt = AosIILClient::getSelf()->preQuerySafe(
	bool rslt = AosIILClientObj::getIILClient()->preQuerySafe(
				 mIILNames[0], mQueryContext, rdata);
	setTotalInRslt(mQueryContext->getTotalDocInRslt());
	return rslt;
}

bool
AosTermKeywordsOr::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	// 1. If there is only one keyword, it retrieves the results and saves
	//    it in 'query_rslt. 
	// 2. If the term is used as the sorting term, it checks whether there is
	//    a container. If yes, it uses the container's objid listing as the 
	//    sorting IIL. Otherwise, it uses the global objid IIL as the sorting
	//    IIL. 
	// 3. Otherwise, it loads all the data into 'query_rslt'. 
	aos_assert_rr(mIILNames.size() > 0, rdata, false);

	aos_assert_r(query_context,false);
	
	query_context->setOpr(eAosOpr_an);
	if (mIILNames.size() == 1)
	{
		return AosQueryColumn(mIILNames[0], query_rslt, query_bitmap, query_context, rdata);
	}

	// Create the bitmap for all the keywords.
	AosBitmapObjPtr bitmap = query_bitmap;

	// Chen Ding, 11/24/2012
	// if (!bitmap) bitmap = AosQueryRsltMgr::getSelf()->getBitmap();
	if (!bitmap) bitmap = AosBitmapMgrObj::getBitmapStatic();
	aos_assert_rr(bitmap, rdata, false);
	bitmap->setOprOr();
	query_context->setOpr(eAosOpr_an);
	
	for (u32 i=0; i<mIILNames.size(); i++)
	{
		bool rslt = AosQueryColumn(mIILNames[i], 0, bitmap, query_context, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	if (!query_rslt) return true;

	// We need to use a member listing to select objects. 
	OmnString iilname;
	iilname = AosIILName::composeCtnrMemberObjidIILName(mContainer);
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(iilname, query_rslt, bitmap,query_context, rdata);
	if (!query_bitmap && bitmap)
	{
		// Need to release the bitmap
		AosBitmapMgrObj::returnBitmapStatic(bitmap);
	}
	return rslt;
}


bool 	
AosTermKeywordsOr::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	/*
	mCondInfo.reset();
	for (u32 i=0; i<mIILNames.size(); i++)
	{
		AosCondInfo info;
		AosIILClient::getSelf()->collectInfo(mIILNames[i], eAosOpr_an, "*", info, rdata);
		mCondInfo.mergeOr(info);	
	}

	return true;
	*/
}


