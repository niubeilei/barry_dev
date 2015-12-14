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
// A Distribution Map is defined as:
// 		[value, docid, num_docs]
// 		[value, docid, num_docs]
// 		...
// 		[value, docid, num_docs]
// It is used for paging. 
//
// Modification History:
// 2014/01/20 Created by Shawn Li
////////////////////////////////////////////////////////////////////////////
#include "QueryUtil/QueryDistrMap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Base64/Base64.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "Util/StrUtil.h"
#include "Util/Opr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


AosQueryDistrMap::AosQueryDistrMap()
:
mIdx(0)
{
}


AosQueryDistrMap::~AosQueryDistrMap()
{
}


AosQueryDistrMapObjPtr 
AosQueryDistrMap::createObject()
{
	return OmnNew AosQueryDistrMap();
}


bool 
AosQueryDistrMap::setInfo(
		vector<OmnString> &values,  
		vector<u64> &docids,
		vector<u64> &num_entries)
{
	mValues.swap(values);
	mDocids.swap(docids);
	mNumEntries.swap(num_entries);
	return true;
}


void
AosQueryDistrMap::clear()
{
	mValues.clear();
	mDocids.clear();
	mNumEntries.clear();
}


bool			
AosQueryDistrMap::serializeToXml(AosXmlTagPtr &xml)
{
	OmnString str = toString();

	AosXmlParser parser;
	xml = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	return true;
}


OmnString
AosQueryDistrMap::toString()
{
	aos_assert_r(mValues.size() == mDocids.size() && mValues.size() == mNumEntries.size(),"");
	OmnString str;
	str << "<distrmap>";
	
	for(u32 i = 0;i < mValues.size();i++)
	{
		str << "<entry " 
			<< "docid=\"" << mDocids[i] << "\" "
			<< "numentries=\"" << mNumEntries[i] << "\">"
			<< "<value>" << AosStrUtil::cdata(mValues[i]) << "</value>"
			<< "</entry>";
//		str << "<entry value=\"" << mValues[i] << "\" " 
//			<< "docid=\"" << mDocids[i] << "\" "
//			<< "numentries=\"" << mNumEntries[i] << "\"/>";
	}
	
	str	<< "</distrmap>";
	return str;	
}


bool			
AosQueryDistrMap::serializeFromXml(const AosXmlTagPtr &xml)
{
	clear();
	
	AosXmlTagPtr entry_xml = xml->getFirstChild("entry");
	OmnString value;
	u64 docid;
	u64 num_entries;
	while(entry_xml)
	{
		value = entry_xml->getAttrStr("value");
		if(value == "")
		{
			value = xml->getNodeText("value");
		}
		mValues.push_back(value);
		docid = entry_xml->getAttrU64("docid",0);
		mDocids.push_back(docid);
		num_entries = entry_xml->getAttrU64("numentries",0);
		mNumEntries.push_back(num_entries);

		entry_xml = xml->getNextChild("entry");
	}
	
	return true;	
}


bool 
AosQueryDistrMap::clone(const AosQueryDistrMapObjPtr &obj)
{
	aos_assert_r(obj,false);
	AosQueryDistrMapPtr obj2 = dynamic_cast<AosQueryDistrMap*>(obj.getPtr());
	aos_assert_r(obj2->mValues.size() == obj2->mDocids.size() && obj2->mValues.size() == obj2->mNumEntries.size(),false);
	for(u32 i = 0;i < mValues.size();i++)
	{
		mValues.push_back(obj2->mValues[i]);
		mDocids.push_back(obj2->mDocids[i]);
		mNumEntries.push_back(obj2->mNumEntries[i]);
	}
	return true;
}


bool
AosQueryDistrMap::reset()
{
	mIdx = 0;
	return true;
}


bool    
AosQueryDistrMap::getDistrEntry(
		OmnString &value,
		u64 &docid,
		u64 &numdocs)
{
	if (mIdx >= mValues.size())
	{
		value = "";
		docid = 0;
		numdocs = 0;
		return false;
	}

	value = mValues[mIdx];
	docid = mDocids[mIdx];
	numdocs = mNumEntries[mIdx];
	mIdx++;
	return true;
}

