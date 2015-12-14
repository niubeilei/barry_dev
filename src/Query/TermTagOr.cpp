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
// If the term is not used as the sorting term, when it is checked for
// a docid, it returns true if the docid is in one of its tags. 
//
// If the term is used as the sorting term, it checks whether there is
// a container. If yes, it uses the container's objid listing as the 
// sorting IIL. Otherwise, it uses the global objid IIL as the sorting
// IIL. 
//
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermTagOr.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "IILSelector/IILSelector.h"
#include "IILSelector/IILSelectorNames.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "WordParser/WordNorm.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/IILClientObj.h"


AosTermTagOr::AosTermTagOr(const bool regflag)
:
AosQueryTerm(AOSTERMTYPE_TAG_OR, AosQueryType::eTagOr, regflag),
mDataLoaded(false)
{
}


AosTermTagOr::AosTermTagOr(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosQueryTerm(AOSTERMTYPE_TAG_OR, AosQueryType::eTagOr, false),
mDataLoaded(false)
{
	// The def should be:
	// 	<term type=AOSTERMTYPE_TAGS reverse="true|false" order="true|false">
	// 		<selector type=AOSIILSELTYPE_TAGS 
	// 			container="xxx"
	// 			aname="xxx">tag,tag,...</selector>
	// 		<cond type="tags" zky_opr="ad|or">tag,tag,...</cond>
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

	OmnString tags;
	AosXmlTagPtr cond_tag = def->getFirstChild("cond");
	if (!cond_tag)
	{
		tags = selector_tag->getNodeText();
		if (tags == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingCondition);
			OmnAlarm << rdata->getErrmsg() << def->toString() << enderr;
			mIsGood = false;
			return;
		}
	}
	else
	{
		tags = cond_tag->getNodeText();
		if (tags == "")
		{
			tags = selector_tag->getNodeText();
		}

		if (tags == "")
		{
			AosSetError(rdata, AosErrmsgId::eMissingCondition);
			OmnAlarm << rdata->getErrmsg() << def->toString() << enderr;
			mIsGood = false;
			return;
		}
	}

	mIsGood = parse(tags, rdata);
}


AosTermTagOr::~AosTermTagOr()
{
}


bool
AosTermTagOr::parse(
		const OmnString &tags, 
		const AosRundataPtr &rdata)
{
	// 'tags' is in the form:
	// 		tag,tag,...
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

	// Chen Ding, 02/06/2012
	// Tags are separated by commas. Do not use word parser. 
	// AosWordParser wordParser;
    // wordParser.setSrc(tags);
	// OmnString word; 
	// i64 guard = eMaxTags;
    // while (wordParser.nextWord(word) && guard--)
    // {
	// 	char wordbuff[AosXmlTag::eMaxWordLen+1];
	// 	i64 len = word.length();
	// 	char * newWord = AosQueryReq::mWordNorm->normalizeWord(
	// 		(char *)word.data(), len, wordbuff, AosXmlTag::eMaxWordLen);
	// 	OmnString iilname = AosIILName::composeContainerAttrWordIILName(mContainer,AOSTAG_TAG,
	// 			newWord);
	// 	mIILNames.push_back(iilname);
	// }

	OmnStrParser1 parser(tags, ", ");
	OmnString word;
	i64 guard = eMaxTags;
	while ((word = parser.nextWord()) != "" && guard--)
	{
		char wordbuff[AosXmlTag::eMaxWordLen+1];
		int len = word.length();
		char * newWord = AosQueryReq::smWordNorm->normalizeWord(
			(char *)word.data(), len, wordbuff, AosXmlTag::eMaxWordLen);
		OmnString iilname = AosIILName::composeContainerAttrWordIILName(mContainer,AOSTAG_TAG,
				newWord);
		mIILNames.push_back(iilname);
	}

	mIsGood = (mIILNames.size() > 0);
	return true;
}


void
AosTermTagOr::toString(OmnString &str)
{
	// 	<term type="tag" 
	// 		container="xxx"
	// 		reverse="true|false"
	// 		order="true|false">xxx</term>
	OmnString reverse = (mReverse)?"true":"false";
	OmnString order = (mOrder)?"true":"false";
	str << "<tag type=\"tagor\" ";
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
AosTermTagOr::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return OmnNew AosTermTagOr(def, rdata);
}


bool 	
AosTermTagOr::nextDocid(
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
AosTermTagOr::moveTo(const i64 &startidx, const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->moveTo(startidx, mNoMoreDocs, rdata);
}


i64		
AosTermTagOr::getTotal(const AosRundataPtr &rdata)
{
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return 0;
	return mQueryRslt->getTotal(rdata);
}


bool
AosTermTagOr::setOrder(
		const OmnString &container, 
		const OmnString &name, 
		const bool reverse, 
		const AosRundataPtr &)
{
	return false;
}


bool
AosTermTagOr::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// It checks whether the doci 'docid' can be selected by this term.
	if (!mDataLoaded) loadData(rdata);
	if (!mQueryRslt) return false;
	return mQueryRslt->checkDocid(docid, rdata);
}


void	
AosTermTagOr::reset(const AosRundataPtr &rdata)
{
	if (!mQueryRslt) return;
	mQueryRslt->reset();
}


bool
AosTermTagOr::loadData(const AosRundataPtr &rdata)
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

	if (!mQueryContext)
	{
		mQueryContext = AosQueryContextObj::createQueryContextStatic();
	}

	mQueryContext->setReverse(mReverse);
	return getDocidsFromIIL(mQueryRslt, mBitmap, mQueryContext, rdata);
}

bool 	
AosTermTagOr::getRsltSizeFromIIL(const AosRundataPtr &rdata)
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
AosTermTagOr::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context, 
		const AosRundataPtr &rdata)
{
	// 1. If there is only one tag, it retrieves the results and saves
	//    it in 'query_rslt. 
	// 2. If the term is used as the sorting term, it checks whether there is
	//    a container. If yes, it uses the container's objid listing as the 
	//    sorting IIL. Otherwise, it uses the global objid IIL as the sorting
	//    IIL. 
	// 3. Otherwise, it loads all the data into 'query_rslt'. 
	aos_assert_rr(mIILNames.size() > 0, rdata, false);
	aos_assert_rr(query_context > 0, rdata, false);
	if (mIILNames.size() == 1)
	{
		query_context->setOpr(eAosOpr_an);
		return AosQueryColumn(mIILNames[0], query_rslt, query_bitmap, query_context, rdata);
	}

	// Create the bitmap for all the tags.
	AosBitmapObjPtr bitmap = query_bitmap;
	if (!bitmap) bitmap = AosBitmapMgrObj::getBitmapStatic();
	aos_assert_rr(bitmap, rdata, false);
	bitmap->setOprOr();
	for (u32 i=0; i<mIILNames.size(); i++)
	{
		query_context->setOpr(eAosOpr_an);
		bool rslt = AosQueryColumn(mIILNames[i], 0, bitmap, query_context, rdata);
		aos_assert_rr(rslt, rdata, false);
	}
	if (!query_rslt) return true;

	// We need to use a member listing to select objects. 
	OmnString iilname;
	iilname = AosIILName::composeCtnrMemberObjidIILName(mContainer);
	query_context->setOpr(eAosOpr_an);
	bool rslt = AosQueryColumn(iilname, query_rslt, bitmap, query_context, rdata);
	if (!query_bitmap && bitmap)
	{
		// Need to release the bitmap
		AosBitmapMgrObj::returnBitmapStatic(bitmap);
	}
	return rslt;
}


bool	
AosTermTagOr::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


