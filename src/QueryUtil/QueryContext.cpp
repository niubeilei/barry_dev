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
#include "QueryUtil/QueryContext.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Base64/Base64.h"
#include "QueryUtil/QueryGroupFilter.h"
#include "QueryUtil/FieldFilter.h"
#include "QueryUtil/QueryFilter.h"
#include "Util/StrUtil.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "Util/Opr.h"
#include "XmlUtil/XmlTag.h"


AosQueryContext::AosQueryContext()
{
	initMemberData();
}


AosQueryContext::~AosQueryContext()
{
}


bool
AosQueryContext::initMemberData()
{
	clear();
	return true;
}


AosQueryContextObjPtr
AosQueryContext::createQueryContext()
{
	return OmnNew AosQueryContext();
}


void
AosQueryContext::returnQueryContext(const AosQueryContextObjPtr &context)
{
}


void
AosQueryContext::clear()
{
	mReverse = false;
	mIndex = -10;
	
	mIILIndex = -10;
	mPageSize = 0;
	mBlockSize = eDefaultBlockSize;
	mOpr = eAosOpr_eq;
	mStrValue = "";	
//	mU64Value = 0;
	mStrValue2 = "";	
//	mU64Value2 = 0;
	mCrtDocid = 0;
	mCrtValue = "";
//	mCrtU64Value = 0;

	mFinished = false;
	mCheckRslt = false;
	mMoveOnly = false;
	mNumDocChecked = 0;
	mTotalDocInIIL = 0;
	mTotalDocInRslt = 0;
	mGrouping = false;
	mGrpFirstEntry = true;
	mGrpCmpKey = "";
	mGrpCurKey = "";
	mGrpCmpU64Key = 0;
	mGrpCurU64Key = 0;
	mGrpCrtValue = 0;
	mGrpCrtNum = 0;
	mSaperator = "";
	mKeyGroupingType = eFullKey;
	mValueGroupingType = eSum;
	mSplitValueType = eKey;
	mSplitValueStr = "";
	mParalIILName = "";
	mLimitStrValue = "";
	mLimitU64Value = 0;
	mLimitDocid = 0;
	mDetect = false;
	mIsAlphaNum = false;
	mEstimate = false;
	mQueryCursor = 0;
	mDistrMap = 0;
	mIILIdx2.reset();
	mGroupFilters.clear();
	mFieldFilter = 0;
	mMultiCond.clear();

	// Ken Lee, 2014/08/19
	mFilters.clear();
	mIILType = eAosIILType_Invalid;
}


// Chen Dign, 2013/02/15
bool			
AosQueryContext::serializeToXml(AosXmlTagPtr &xml, const AosRundataPtr &rdata)
{
	OmnString str;
	bool rslt = serializeToStr(str, rdata);
	aos_assert_r(rslt, false);

	AosXmlParser parser;
	xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
}


bool
AosQueryContext::serializeToStr(OmnString &str, const AosRundataPtr &rdata)
{
	str << "<query_context ";
	
	if(mIILIdx2.toString() != "") str << "IILIdx2=\"" << mIILIdx2.toString() << "\" ";
	if(mReverse) str << "reverse=\"" << mReverse << "\" ";
	if(mPageSize) str << "psize=\"" << mPageSize << "\" ";
	if(mIndex) str << "index=\"" << mIndex << "\" ";
	if(mIILIndex) str << "iilidx=\"" << mIILIndex << "\" ";
	if(mBlockSize) str << "bsize=\"" << mBlockSize << "\" ";

	if(mNumDocChecked) str << "numchecked=\"" << mNumDocChecked << "\" ";
	if(mTotalDocInIIL) str << "numtotal=\"" << mTotalDocInIIL << "\" ";
	if(mTotalDocInRslt) str << "numtotalrslt=\"" << mTotalDocInRslt << "\" ";

	if(mOpr != eAosOpr_eq) str << "opr=\"" << AosOpr_toStr(mOpr)<< "\" ";
//	if(mU64Value) str << "u64Value=\"" << mU64Value << "\" ";  
//	if(mU64Value2) str << "u64Value2=\"" << mU64Value2 << "\" ";  
	if(mCrtDocid) str << "crtDocid=\"" << mCrtDocid << "\" ";  
//	if(mCrtU64Value) str << "crtU64Value=\"" << mCrtU64Value << "\" "; 
	if(mFinished) str << "finished=\"" << mFinished << "\" "; 
	if(mCheckRslt) str << "checkRslt=\"" << mCheckRslt << "\" "; 
	if(mMoveOnly) str << "moveOnly=\"" << mMoveOnly << "\" "; 
	if(mGrouping) str << "grouping=\"" << mGrouping << "\" "; 
	
	if(mKeyGroupingType != eFullKey) str << "keyGroupingType=\"" << (int)mKeyGroupingType << "\" "; 
	if(mValueGroupingType != eSum) str << "valueGroupingType=\"" << (int)mValueGroupingType << "\" "; 
	if(mSplitValueType != eKey) str << "splitValueType=\"" << (int)mSplitValueType << "\" ";


	if (mParalIILName != "") str << "paralIILName=\"" << mParalIILName << "\" ";
	if (mLimitU64Value != 0) str << "limitU64Value=\"" << mLimitU64Value << "\" ";
	if(mLimitDocid) str << "limitDocid=\"" << mLimitDocid << "\" "; 
//	if(mPageStart) str << "pageStart=\"" << mPageStart << "\" "; 
	if(mDetect) str << "detect=\"" << mDetect << "\" ";
	if(mIsAlphaNum) str << "alphanum=\"" << mIsAlphaNum << "\" ";

	if(mEstimate) str << "estimate=\"" << mEstimate << "\" ";
	str << "iiltype=\"" << (int)mIILType << "\" "; 

	if(mQueryCursor) str << "querycursor=\"" << mQueryCursor << "\" "; 

	str << ">";
	if (mCrtValue != "")
	{
		str << "<crtValue>" << AosStrUtil::cdata(mCrtValue) << "</crtValue>";
	}

	if (mStrValue != "")
	{
		str << "<strValue>" << AosStrUtil::cdata(mStrValue) << "</strValue>";
	}

	if (mStrValue2 != "")
	{
		str <<"<strValue2>" << AosStrUtil::cdata(mStrValue2) << "</strValue2>";
	}
	if(mSaperator != "") str << "<saperator>" << AosStrUtil::cdata(mSaperator) << "</saperator>"; 
	if(mSplitValueStr != "") str << "<splitValueStr>" << AosStrUtil::cdata(mSplitValueStr) << "</splitValueStr>";
	if (mLimitStrValue != "") str << "<limitStrValue>" << AosStrUtil::cdata(mLimitStrValue) << "</limitStrValue>";
//	if(mSaperator != "") str << "saperator=\"" << mSaperator << "\" "; 
//	if(mSplitValueStr != "") str << "splitValueStr=\"" << mSplitValueStr << "\" ";
//	if (mLimitStrValue != "") str << "limitStrValue=\"" << mLimitStrValue << "\" ";

	if(mDistrMap) str << mDistrMap->toString();
	
	// add filters here
	if(!mGroupFilters.empty())
	{
		AosQueryGroupFilterPtr filter;
		for(u32 i = 0; i < mGroupFilters.size(); i++)
		{
			str << mGroupFilters[i]->toString();
		}
	}

	// Linda, 2013/02/25
	if (mFieldFilter) 
	{
		str << mFieldFilter->toString();
	}

	if (!mMultiCond.empty())
	{
		str << "<multicond>";
		for (u32 i = 0; i < mMultiCond.size(); i++)
		{
			str << mMultiCond[i].toString();
		}
		str << "</multicond>";
	}

	// Ken Lee, 2014/08/19
	if (mFilters.size() > 0)
	{
		str << "<query_filters>";
		for (u32 i=0; i<mFilters.size(); i++)
		{
			str << mFilters[i]->toXmlString();	
		}
		str << "</query_filters>";
	}

	str	<< "</query_context>";
	return true;	
}


bool			
AosQueryContext::serializeFromXml(const AosXmlTagPtr &xml, const AosRundataPtr &rdata)
{
	clear();

	mIndex = xml->getAttrInt64("index", -10);
	mIILIndex = xml->getAttrInt64("iilidx", -10);
	bool rslt = mIILIdx2.setValue(xml->getAttrStr("IILIdx2"));
	aos_assert_r(rslt, false);

	mReverse = xml->getAttrBool("reverse", false);
	mOpr = AosOpr_toEnum(xml->getAttrStr("opr", "eq"));
	mStrValue = xml->getAttrStr("strValue");
	if (mStrValue == "")
	{
		mStrValue = xml->getNodeText("strValue");
	}
//	mU64Value = xml->getAttrU64("u64Value", 0);
	mStrValue2 = xml->getAttrStr("strValue2");
	if (mStrValue2 == "")
	{
		mStrValue2 = xml->getNodeText("strValue2");
	}
//	mU64Value2 = xml->getAttrU64("u64Value2", 0);
	mCrtDocid = xml->getAttrU64("crtDocid", 0);
	mNumDocChecked = xml->getAttrInt64("numchecked", 0);
	mTotalDocInIIL = xml->getAttrInt64("numtotal", 0);
	mTotalDocInRslt = xml->getAttrInt64("numtotalrslt", 0);
	mCrtValue = xml->getAttrStr("crtValue");
	if (mCrtValue == "")
	{
		mCrtValue = xml->getNodeText("crtValue");
	}
//	mCrtU64Value = xml->getAttrU64("crtU64Value", 0);
	mFinished = xml->getAttrBool("finished", false);
	mCheckRslt = xml->getAttrBool("checkRslt", false);
	mMoveOnly = xml->getAttrBool("moveOnly", false);
	mPageSize = xml->getAttrInt64("psize", 0);
	mBlockSize = xml->getAttrInt64("bsize", 0);
	mGrouping = xml->getAttrBool("grouping", false);
	mSaperator = xml->getAttrStr("saperator");
	if (mSaperator == "")
	{
		mSaperator = xml->getNodeText("saperator");
	}
	
	mParalIILName = xml->getAttrStr("paralIILName");
	mLimitStrValue = xml->getAttrStr("limitStrValue");
	if (mLimitStrValue == "")
	{
		mLimitStrValue = xml->getNodeText("limitStrValue");
	}
	mLimitU64Value = xml->getAttrU64("limitU64Value", 0);
	mLimitDocid = xml->getAttrU64("limitDocid", 0);
//	mPageStart = xml->getAttrU64("pageStart", 0);
	mDetect = xml->getAttrBool("detect", false);
	mIsAlphaNum = xml->getAttrBool("alphanum", false);
	mEstimate = xml->getAttrBool("estimate", false);
	mIILType = (AosIILType)(xml->getAttrU32("iiltype", 0));

	mQueryCursor = xml->getAttrU64("querycursor", 0);

	AosXmlTagPtr distrmap_xml = xml->getFirstChild("distrmap");
	if(distrmap_xml)
	{
		if(!mDistrMap)
		{
			mDistrMap = AosQueryDistrMapObj::getObject();
		}
		mDistrMap->serializeFromXml(distrmap_xml);
	}
	else
	{
		mDistrMap = 0;
	}
	
	int kgt = xml->getAttrInt("keyGroupingType", 0);
	if(kgt == 0)
	{
		mKeyGroupingType = eFullKey;
	}
	else
	{
		mKeyGroupingType = (QueryKeyGroupingType)kgt;
	}

	int vgt = xml->getAttrInt("valueGroupingType", 0);
	if(vgt == 0)
	{
		mValueGroupingType = eSum;
	}
	else
	{
		mValueGroupingType = (QueryValueGroupingType)vgt;
	}
	int svt = xml->getAttrInt("splitValueType", 0);
	mSplitValueType = (SplitValueType)svt;
	mSplitValueStr = xml->getAttrStr("splitValueStr");
	if (mSplitValueStr == "")
	{
		mSplitValueStr = xml->getNodeText("splitValueStr");
	}

	mGroupFilters.clear();
	// get GroupFilters from xml
	AosQueryGroupFilterPtr filter;
	AosXmlTagPtr filter_xml = xml->getFirstChild("query_group_filter");
	while(filter_xml)
	{
		filter = OmnNew AosQueryGroupFilter(filter_xml);
		mGroupFilters.push_back(filter);
		filter_xml = xml->getNextChild("query_qroup_filter");
	}

	// Linda, 2013/02/25
	AosXmlTagPtr filterfield_xml = xml->getFirstChild("fieldfilter");
	if (filterfield_xml)
	{
		mFieldFilter = OmnNew AosFieldFilter(filterfield_xml);
	}

	AosXmlTagPtr multicond_xml = xml->getFirstChild("multicond");
	if (multicond_xml)
	{
		AosXmlTagPtr condxml = multicond_xml->getFirstChild("cond");
		while (condxml)
		{
			AosMultiCond cond(condxml);
			mMultiCond.push_back(cond);
			condxml = multicond_xml->getNextChild("cond");
		}
	}

	// Ken Lee, 2014/08/19
	mFilters.clear();
	AosXmlTagPtr filters_tag = xml->getFirstChild("query_filters");
	if (filters_tag)
	{
		AosXmlTagPtr filter_tag = filters_tag->getFirstChild();
		aos_assert_r(filter_tag, false);

		AosQueryFilterObjPtr filter;
		while (filter_tag)
		{
			filter = AosQueryFilter::create(filter_tag, rdata);
			aos_assert_r(filter, false);

			mFilters.push_back(filter);

			filter_tag = filters_tag->getNextChild();
		}
	}

	return true;	
}


bool			
AosQueryContext::copyFrom(const AosQueryRsltObjPtr &query_rslt)
{
	OmnShouldNeverComeHere;
	return false;
}


bool			
AosQueryContext::copyTo(const AosQueryRsltObjPtr &query_rslt)
{
	OmnShouldNeverComeHere;
	return false;
}


void			
AosQueryContext::appendGroupEntryStrStart(const AosQueryRsltObjPtr &queryRslt)
{
	mGrpFirstEntry = true;
}


void			
AosQueryContext::appendGroupEntry(
		const OmnString &key, 
		const u64 &value, 
		const AosIILIdx &curIdx,
		const AosQueryRsltObjPtr &queryRslt, 
		const bool num_alpha)
{
	if(!mGroupFilters.empty())
	{
		bool createDefault = false;
		bool valid = true;
		for(u32 i = 0;i < mGroupFilters.size(); i++)
		{
			if(!mGroupFilters[i]->isValid(key,createDefault, num_alpha))
			{
				valid = false;
				break;
			}
		}
		if(!valid)
		{
			if(!createDefault)
			{
				return;
			}
		}
	}
	OmnString cmpkey = getCmpKey(key);
	if(mGrpFirstEntry)
	{
		mGrpCmpKey = cmpkey;
		mGrpFirstEntry = false;
		// first entry handling
		if(mValueGroupingType == eFirstEntry||
		   mValueGroupingType == eMaxEntry||
		   mValueGroupingType == eMinEntry)
		{
			mGrpCrtValue = value;
			mGrpCurKey = key;
		}else
		{
			mGrpCurKey = cmpkey;
		}
	}
	else
	{
		if(mGrpCmpKey != cmpkey)
		{
			// proc
			procGroupStr(queryRslt);
			// first entry handling
			if(mValueGroupingType == eFirstEntry||
			   mValueGroupingType == eMaxEntry||
			   mValueGroupingType == eMinEntry)
			{
				mGrpCrtValue = value;
				mGrpCurKey = key;
			}else
			{
				mGrpCurKey = cmpkey;
			}
		}
		mGrpCmpKey = cmpkey;		
	}
	
	// cache current entry
	switch(mValueGroupingType)
	{
		case eFirstEntry:
			break;
		case eLastEntry:
			mGrpCrtValue = value;
			mGrpCurKey = key;
			break;
		case eMaxEntry:
			if(value > mGrpCrtValue)mGrpCrtValue = value;
			break;
		case eMinEntry:
			if(value < mGrpCrtValue)mGrpCrtValue = value;
		case eSum:
			mGrpCrtValue += value;
			break;
		case eAvg:
			mGrpCrtValue += value;
			mGrpCrtNum ++;
			break;
	}

	if(!isFull(queryRslt))
	{
		mIILIdx2 = curIdx;
		mCrtValue = key;
		mCrtDocid = value;
	}
	return;
}


void			
AosQueryContext::appendGroupEntryStrFinish(const AosQueryRsltObjPtr &queryRslt)
{
	if(isFull(queryRslt))
	{
		return;
	}
	if(!mGrpFirstEntry)
	{
		procGroupStr(queryRslt);
	}
	mFinished = true;
}


void			
AosQueryContext::procGroupStr(const AosQueryRsltObjPtr &queryRslt)
{
	queryRslt->appendStrValue(mGrpCurKey.data());
	
	if(mValueGroupingType == eAvg)
	{
		aos_assert(mGrpCrtNum > 0);
		float avg = mGrpCrtValue/mGrpCrtNum;
		queryRslt->appendDocid((u64)avg);
	}
	else
	{
		queryRslt->appendDocid(mGrpCrtValue);
	}
	
	mGrpCmpKey = "";
	mGrpCurKey = "";
	mGrpCrtNum = 0;
	mGrpCrtValue = 0;
}


void			
AosQueryContext::procGroupU64(const AosQueryRsltObjPtr &queryRslt)
{
	queryRslt->appendU64Value(mGrpCurU64Key);
	
	if(mValueGroupingType == eAvg)
	{
		aos_assert(mGrpCrtNum > 0);
		float avg = mGrpCrtValue/mGrpCrtNum;
		queryRslt->appendDocid((u64)avg);
	}
	else
	{
		queryRslt->appendDocid(mGrpCrtValue);
	}
	
	mGrpCmpU64Key = 0;
	mGrpCurU64Key = 0;
	mGrpCrtNum = 0;
	mGrpCrtValue = 0;
}


OmnString		
AosQueryContext::getCmpKey(const OmnString &key)
{
	switch(mKeyGroupingType)
	{
		case eFullKey:
			return key;
		case eFirstKey:
			{
				vector<OmnString> keys;
				AosStrSplit::splitStrByChar(key, mSaperator.data(), keys, 4);
				if(keys.size() <= 1) return key;
				return keys[0];
			}
	    default:
			break;
	}
	return key;
}


void			
AosQueryContext::appendGroupEntryU64Start(const AosQueryRsltObjPtr &queryRslt)
{
	mGrpFirstEntry = true;
}


void			
AosQueryContext::appendGroupEntry(
		const u64 &key, 
		const u64 &value, 
		const AosIILIdx &curIdx,
		const AosQueryRsltObjPtr &queryRslt)
{
	if(mGrpFirstEntry)
	{
		mGrpCmpU64Key = key;
		mGrpFirstEntry = false;
		// first entry handling
		if(mValueGroupingType == eFirstEntry||
		   mValueGroupingType == eMaxEntry||
		   mValueGroupingType == eMinEntry)
		{
			mGrpCrtValue = value;
			mGrpCurU64Key = key;
		}else
		{
			mGrpCurU64Key = key;
		}
	}
	else
	{
		if(mGrpCmpU64Key != key)
		{
			// proc
			procGroupU64(queryRslt);
			// first entry handling
			if(mValueGroupingType == eFirstEntry||
			   mValueGroupingType == eMaxEntry||
			   mValueGroupingType == eMinEntry)
			{
				mGrpCrtValue = value;
				mGrpCurU64Key = key;
			}
		}
		mGrpCmpU64Key = key;		
	}
	
	// cache current entry
	switch(mValueGroupingType)
	{
		case eFirstEntry:
			break;
		case eLastEntry:
			mGrpCrtValue = value;
			mGrpCurU64Key = key;
			break;
		case eMaxEntry:
			if(value > mGrpCrtValue)mGrpCrtValue = value;
			break;
		case eMinEntry:
			if(value < mGrpCrtValue)mGrpCrtValue = value;
		case eSum:
			mGrpCrtValue += value;
			break;
		case eAvg:
			mGrpCrtValue += value;
			mGrpCrtNum ++;
			break;
	}
	
	if(!isFull(queryRslt))
	{
		mIILIdx2 = curIdx;
		mCrtValue = OmnStrUtil::ulltoa(key);
		mCrtDocid = value;
	}
	return;
}


void			
AosQueryContext::appendGroupEntryU64Finish(const AosQueryRsltObjPtr &queryRslt)
{
	if(isFull(queryRslt))
	{
		return;
	}
	if(!mGrpFirstEntry)
	{
		procGroupU64(queryRslt);
	}
	mFinished = true;
}


void
AosQueryContext::appendGroupEntryI64Start(const AosQueryRsltObjPtr &queryRslt)
{
	OmnNotImplementedYet;
}

void
AosQueryContext::appendGroupEntryI64Finish(const AosQueryRsltObjPtr &queryRslt)
{
	OmnNotImplementedYet;
}


void
AosQueryContext::appendGroupEntryD64Start(const AosQueryRsltObjPtr &queryRslt)
{
	OmnNotImplementedYet;
}

void
AosQueryContext::appendGroupEntryD64Finish(const AosQueryRsltObjPtr &queryRslt)
{
	OmnNotImplementedYet;
}

void			
AosQueryContext::addGroupFilter(
			const GroupFilterType type,
			const AosOpr opr, 
			const OmnString &value1,
			const OmnString &value2,
			const bool createDefault) 
{
	AosQueryGroupFilterPtr filter = OmnNew AosQueryGroupFilter(
			type,opr,value1, value2, mSaperator, createDefault);
	mGroupFilters.push_back(filter);
}	


void			
AosQueryContext::addGroupFilter(
			const GroupFilterType type,
			const AosOpr opr, 
			const u64 &value1,
			const u64 &value2,
			const bool createDefault)
{
	AosQueryGroupFilterPtr filter = OmnNew AosQueryGroupFilter(
			type,opr,value1, value2,mSaperator, createDefault);
	mGroupFilters.push_back(filter);
}	


void			
AosQueryContext::removeAllGroupFilter()
{
	mGroupFilters.clear();
}


void			
AosQueryContext::setSaperator(const OmnString &saperator)
{
	mSaperator = saperator;
	for(u32 i = 0;i < mGroupFilters.size();i++)
	{
		mGroupFilters[i]->setSaperator(saperator);
	}
}

	
bool			
AosQueryContext::isFull(const u64 &num) const
{
	if(mBlockSize == 0)
	{
		return false;
	}
	return (i64)num >= mBlockSize;
}


AosQueryContextObjPtr
AosQueryContext::clone()
{
	AosQueryContext *obj = OmnNew AosQueryContext();
	AosQueryContextObj::copyMemberData(obj);

	obj->mIILIdx2 = mIILIdx2;

	obj->mGroupFilters.clear();
	for (u32 i=0; i<mGroupFilters.size(); i++)
	{
		AosQueryGroupFilterPtr filter = mGroupFilters[i]->clone();
		obj->mGroupFilters.push_back(filter);
	}

	obj->mFilters.clear();
	for (u32 i=0; i<mFilters.size(); i++)
	{
		AosQueryFilterObjPtr filter = mFilters[i]->clone();
		obj->mFilters.push_back(filter);
	}

	return obj;
}


bool
AosQueryContext::initQueryContext()
{
	AosQueryContextObj::setQueryContextObj(OmnNew AosQueryContext());
	return true;
}


bool
AosQueryContext::filterFieldCond(const OmnString &key)
{
	if (!mFieldFilter) return false;
	return mFieldFilter->cond(key);
}

bool
AosQueryContext::retrieveMultiCond(const u32 &idx)
{
	if (idx >= mMultiCond.size()) return false;
	setOpr(mMultiCond[idx].opr);
	if(mMultiCond[idx].strValue == "")
	{
		setStrValue(OmnStrUtil::ulltoa(mMultiCond[idx].u64Value));
		setStrValue2(OmnStrUtil::ulltoa(mMultiCond[idx].u64Value2));
	}
	else
	{
		setStrValue(mMultiCond[idx].strValue);
		setStrValue2(mMultiCond[idx].strValue2);
	}
	setIILIndex2(mMultiCond[idx].iilIdx2);	
	setFinished(mMultiCond[idx].finished);
	setIndex(mMultiCond[idx].index);
	setIILIndex(mMultiCond[idx].iilIndex);
	return true;
}


bool
AosQueryContext::updateMultiCond(const u32 &idx)
{
	if (idx >= mMultiCond.size()) return false;
	mMultiCond[idx].finished = mFinished;
	mMultiCond[idx].index = mIndex;
	mMultiCond[idx].iilIndex = mIILIndex;
	mMultiCond[idx].iilIdx2 = mIILIdx2;
	return true;
}


void 
AosQueryContext::addMultiCond(
		const AosOpr &opr, 
		const OmnString &strvalue,
		const u64 &u64value,
		const OmnString &strvalue2,
		const u64 &u64value2)
{
	AosMultiCond cond(opr, strvalue, u64value, strvalue2, u64value2);
	mMultiCond.push_back(cond);	
}


void 
AosQueryContext::setFieldFilterObj(const AosFieldFilterPtr &b)
{
	mFieldFilter = b;
}


bool
AosQueryContext::evalFilter(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	bool rslt = true;
	for (u32 i=0; i<mFilters.size(); i++)
	{
		rslt = mFilters[i]->evalFilter(key, value, rdata);
		if (!rslt) return false;
	}
	return true;
}


bool
AosQueryContext::evalFilter(
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata) const
{
	bool rslt = true;
	for (u32 i=0; i<mFilters.size(); i++)
	{
		rslt = mFilters[i]->evalFilter(key, value, rdata);
		if (!rslt) return false;
	}
	return true;
}

