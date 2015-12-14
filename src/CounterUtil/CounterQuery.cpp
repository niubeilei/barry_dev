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
// Aggregate Multiple 
// For the statistics IIL:
//		epoch_day.call_duration
// if it is to retrieve all the call durations for a given day, 
// it is a prefix search or a range search: [epoch_day, epoch_day+1]. It returns:
//	[epoch_day.call_duration1, num]
//	[epoch_day.call_duration2. num]
//		...
// If it is a day range: [epoch_day1, epoch_day2], 
// it is a range query: [epoch_day1, epoch_day2]. 
// It collects all the data, creates an array based on call durations, 
// sums all the values, and then return the array.
// In general, given a statistics:
//		term1.term2...termn
// there terms are divided into three groups:
//		1.Fixed Value Terms: 
//			values for terms in this group are given. These must be the leading terms. This group can be empty.
//		2. Aggregate Term: 
//			This term must be the term right after the last term in the Fixed Value group. 
//		This it the term whose values are aggregated. If there are no fixed value terms, 
//		the query should be a range query (but single value query should not considered an error). 
//		The range will match multiple values for this term. These values will be aggregated. 
//		3. Member Term: 
//			this is the next term and also the last term whose values are enumerated.
// In the above example, there are not fixed value terms. The Aggregate Term is epoch_day and Member Term is call_duration. 
// Note that this may be done in queries. It should be done in IIL. 
// Modification History:
// 09/18/2012 Created by Linda  
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/CounterQuery.h"

#include "CounterUtil/CounterUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosCounterQuery::AosCounterQuery()
{
}


AosCounterQuery::~AosCounterQuery()
{
}

void
AosCounterQuery::clear()
{
	mType = eInvalid;
	mMergeFieldIndex = -1;
	mConvertConfig = 0;
}


bool			
AosCounterQuery::serializeToXml(AosXmlTagPtr &xml)
{
	OmnString str;
	str << "<counter_query ";
	if (mType != eInvalid) str << "aggregateType=\"" << toStr(mType) << "\" ";
	if (mMergeFieldIndex >= 0) str << "mergeFieldIndex=\"" << mMergeFieldIndex << "\" ";
	str << " >";
	if (mConvertConfig) str << mConvertConfig->toString(); 
	if (mNeedFilterValue)
	{
		str << "<filtervalue opr=\"" << AosOpr_toStr(mValueFilterOpr) << "\" "
			<< "value=\"" << mValueFilterValue << "\"/>";
	}
	str	<< "</counter_query>";
		
	AosXmlParser parser;
	xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	return true;
	
}


bool			
AosCounterQuery::serializeFromXml(const AosXmlTagPtr &xml)
{
	clear();

	mType = toEnum(xml->getAttrStr("aggregateType", ""));
	mMergeFieldIndex = xml->getAttrInt("mergeFieldIndex", -1);
	mConvertConfig = xml->getFirstChild();

	AosXmlTagPtr filter_value_tag = xml->getFirstChild("filtervalue");
	if (filter_value_tag)
	{
		mNeedFilterValue = true;
		mValueFilterOpr = AosOpr_toEnum(filter_value_tag->getAttrStr("opr", "an"));
		mValueFilterValue = filter_value_tag->getAttrU64("value", 0);
	}
	return true;	
}

bool
AosCounterQuery::aggregateMultiple(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	switch (mType)
	{
	case eConversion:
		 return conversionEntries(query_rslt, rdata);

	case eFindMaxMin:
		 return findMaxMinValue(query_rslt, rdata); 

	case eMerge:
		 return mergeEntries(query_rslt, rdata);

	case eMergeCount:
		 return mergeCount(query_rslt, rdata);

	default:
		return true;
	}
	return true;
}


bool
AosCounterQuery::mergeEntries(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	//If a statistics IIL is in the following format:
	//	epoch_day.call_duration
	//we want to search entries for a day range, it needs to assemble results:
	//	data[call_duration1] = sum(for all days for the same call_duration value)
	//	data[call_duration2] = sum(...)
	
	if (mMergeFieldIndex < 0) return true;
	u64 docid;
	OmnString key, name;
	bool finished = false;
	query_rslt->resetByValue();
	while(query_rslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
//OmnScreen << " key : " << key << ", docid : " << docid << endl;
		name = AosCounterUtil::decomposeRemoveTerm(key, mMergeFieldIndex);	
		if (mMap.count(name) == 0)
		{
			mMap.insert(make_pair(name, docid));
		}
		else
		{
			mMap[name] = mMap[name] + docid;
		}
		// Linda, 2012/12/17 temporary 
		//if (mMap.size() >= AosCounterUtil::eMaxPerCounterEntry) return true; 
	}
	return true;
}

	
bool
AosCounterQuery::mergeCount(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	//If a statistics IIL is in the following format:
	//	epoch_day.call_duration
	//we want to search entries for a day range, it needs to assemble results:
	//	data[call_duration1] = sum(for all days for the same call_duration value)
	//	data[call_duration2] = sum(...)
	
	if (mMergeFieldIndex < 0) return true;
	u64 docid;
	OmnString key, name;
	bool finished = false;
	query_rslt->resetByValue();
	while (query_rslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
//OmnScreen << " key : " << key << ", docid : " << docid << endl;
		name = AosCounterUtil::decomposeRemoveTerm(key, mMergeFieldIndex);	
		if (mMap.count(name) == 0)
		{
			mMap.insert(make_pair(name, 1));
		}
		else
		{
			mMap[name] = mMap[name] + 1;
		}
		// Linda, 2012/12/17 temporary 
		//if (mMap.size() >= AosCounterUtil::eMaxPerCounterEntry) return true; 
	}
	return true;
}


bool
AosCounterQuery::filterValue(const u64 &value)
{
	if (!mNeedFilterValue) return true;
	switch (mValueFilterOpr)
	{
	case eAosOpr_lt : return value < mValueFilterValue;
	case eAosOpr_le : return value <= mValueFilterValue;
	case eAosOpr_gt : return value > mValueFilterValue;
	case eAosOpr_ge : return value >= mValueFilterValue;
	case eAosOpr_eq : return value == mValueFilterValue;
	case eAosOpr_ne : return value != mValueFilterValue;
	default : break;
	}
	return true;
}


bool
AosCounterQuery::copyData(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	if (mType == eFindMaxMin) 
	{
		return copyData1(query_rslt, rdata);
	}
	bool addValue = query_rslt->isWithValues();
	query_rslt->resetByValue();
	query_rslt->setNumDocs(0);

	map<OmnString, u64>::iterator itr;
	u64 num = 0;
	for (itr = mMap.begin(); itr != mMap.end(); itr ++)
	{
//OmnScreen << "map key : " << itr->first <<" , docid: " << itr->second << endl;
		if (mNeedFilterValue)
		{
			if (!filterValue(itr->second)) continue;	
		}
		query_rslt->appendDocid(itr->second);
		if(addValue)
		{
		    query_rslt->appendStrValue(itr->first.data());
		}

		num++;
		if (num >= AosCounterUtil::eMaxPerCounterEntry) break;
	}
	query_rslt->setNumDocs(num);
	return true;
}


bool
AosCounterQuery::conversionEntries(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	//This is a utility that converts an integer to another integer. 
	//As an example, we want to convert call durations into segments:
	//	[1, 9]
	//	[10, 19]
	//	[20, 29]
	//		...
	//	> 1000
	//	Or in other word, the original integer is segmented into a number of 10-second ranges. 
	//	When given an integer, it is rounded to the nearest range. 
	//	This converter is defined as:
	//		Convert(input, min, max, interval)
	//			If input is less than or equal to min, return min.
	//			If input is greater than or equal to max, return max.
	//			Otherwise, it is rounded to the nearest multiples of interval.
	
	int mergefield_index2;
	AosDataType::E data_type;
	vector<OmnString> value;
	vector<OmnString> str;
	bool rslt = parseConfig(mergefield_index2, data_type, value, str);
	aos_assert_r(rslt, false);

	aos_assert_r(value.size() > 0, false);
	aos_assert_r(str.size() > 0, false);
	u64 docid;
	OmnString key, name;
	bool finished = false;
	query_rslt->resetByValue();
	while(query_rslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
//OmnScreen << " key : " << key << ", docid : " << docid << endl;
		if (mMergeFieldIndex >= 0) 
		{
			key = AosCounterUtil::decomposeRemoveTerm(key, mMergeFieldIndex);	
		}

		name = AosCounterUtil::getTerm2(key, mergefield_index2);
		int index = findBoundary(value, name, data_type);
		aos_assert_r(index > 0, false);
		aos_assert_r(index < ((int)str.size() - 1), false);
		name = str[index]; 
		name = AosCounterUtil::decomposeReplaceTerm(key, mergefield_index2, name);
		if (mMap.count(name) == 0)
		{
			mMap.insert(make_pair(name, docid));
		}
		else
		{
			mMap[name] = mMap[name] + docid;
		}
	}
	return true;
}


bool
AosCounterQuery::parseConfig(
		int &mergefield_index2,
		AosDataType::E &data_type,
		vector<OmnString> &value, 
		vector<OmnString> &key)
{
	aos_assert_r(mConvertConfig, false);
	mergefield_index2 = mConvertConfig->getAttrInt("zky_mergefield_index2", -1);
	if (mMergeFieldIndex >= 0 && mMergeFieldIndex < mergefield_index2) mergefield_index2 --;
	OmnString type = mConvertConfig->getAttrStr("zky_mergefield_type");

	data_type = AosDataType::toEnum(type);
	if (type == "") data_type = AosDataType::eU64;	

	if (! AosDataType::isValid(data_type))
	{
		OmnAlarm << "Invalid DataType" << enderr;
		return false;
	}

	AosXmlTagPtr child = mConvertConfig->getFirstChild();
	while (child)
	{
		OmnString vv1 = child->getAttrStr("zky_value", "0");
		OmnString vv2 = child->getAttrStr("zky_key", "");
		if (vv2 != "")
		{
			key.push_back(vv2);
			if (!(vv1 == "0" && value.size() != 0))
			{
				value.push_back(vv1);
			}
		}
		
		child = mConvertConfig->getNextChild();
	}
	return true;
}


int
AosCounterQuery::findBoundary(
		const vector<OmnString> &value, 
		const OmnString &vv,
		const AosDataType::E &data_type)
{
	int left, right, mid, low;
	low = 0;
	left = 0;
	right = value.size();
	mid = (right + left) /2;
	while(left <right)
	{
		if (cmp(data_type, vv, value[mid]) <= 0)
		{
			right = mid;
			low = right;
		}
		else
		{
			left = mid +1;
			low = left;
		}
		mid = (right + left)/2;
	}
	return low;
}


int
AosCounterQuery::cmp(
		const AosDataType::E &data_type,
		const OmnString &lhs, 
		const OmnString &rhs)
{
	int rslt = 0;
	switch (data_type)
	{
	case AosDataType::eU64:
		{
			u64 l = atoll(lhs.data());
			u64 r = atoll(rhs.data());
			if (l==r) rslt = 0;
			else if(l<r) rslt = -1;
			else rslt = 1;
			break;
		}

	case AosDataType::eString:
		 rslt = strcmp(lhs.data(), rhs.data());
		 break;

	default:
		OmnAlarm << "Unrecognized data type: " << data_type << enderr;
		break;
	}
	return rslt;
}


void 
AosCounterQuery::setConvertEntriesConfig(const AosXmlTagPtr &term)
{
	if (mType == eConversion)
	{
		AosXmlParser parser;
		mConvertConfig = parser.parse(term->toString(), "" AosMemoryCheckerArgs);
	}
}


bool
AosCounterQuery::findMaxMinValue(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	//If a statistics IIL is in the following format:
	//	epoch_day.call_duration
	//we want to search entries for a day range, it needs to assemble results:
	//	data[call_duration1] = min(for all days for the same call_duration value)
	//	data[call_duration2] = max(...)
	
	u64 docid;
	OmnString key;
	bool finished = false;
	query_rslt->resetByValue();
	while(query_rslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
//OmnScreen << " key : " << key << ", docid : " << docid << endl;
		if (mMergeFieldIndex >= 0) 
		{
			key = AosCounterUtil::decomposeRemoveTerm(key, mMergeFieldIndex);	
		}
		if (mMin.count(key) == 0)
		{
			aos_assert_r(mMap.count(key) == 0, false);
			mMin.insert(make_pair(key, docid));
			mMap.insert(make_pair(key, docid));
		}
		else
		{
			//mmap[key] = mmap[key] + docid;
			if (docid < mMin[key]) mMin[key] = docid;
			if (docid > mMap[key]) mMap[key] = docid;
		}
	}

	aos_assert_r(mMin.size() == mMap.size(), false);
	return true;
}


bool
AosCounterQuery::copyData1(
		const AosQueryRsltObjPtr &query_rslt,
		const AosRundataPtr &rdata)
{
	bool addValue = query_rslt->isWithValues();
	query_rslt->resetByValue();
	query_rslt->setNumDocs(0);
	OmnString maxStr, minStr;
	map<OmnString, u64 >::iterator itr;
	map<OmnString, u64 >::iterator itr1 = mMin.begin();
	aos_assert_r(mMap.size() == mMin.size(), false);
	for (itr = mMap.begin(); itr != mMap.end() && itr != mMin.end(); itr ++, itr1 ++)
	{
//OmnScreen << "map key : " << itr->first <<" , docid: " << itr->second << endl;
		query_rslt->appendDocid(itr->second);
		query_rslt->appendDocid(itr1->second);
		if(addValue)
		{
			maxStr = AosCounterUtil::composeStrTerm2(itr->first, "max");	
			minStr = AosCounterUtil::composeStrTerm2(itr1->first, "min");	
		    query_rslt->appendStrValue(maxStr.data());
		    query_rslt->appendStrValue(minStr.data());
		}
	}
	query_rslt->setNumDocs(mMap.size() + mMin.size());
	return true;
}

