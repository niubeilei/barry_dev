////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/RandomStrGen/Testers/JimoRandomStrGenTest.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>


static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;


bool
AosJimoRandomStrGenTest_nextValue(		
		const AosRundataPtr &rdata, 
		AosJimo *abs_jimo,
		const AosXmlTagPtr &worker_doc)
{
	size_t loop = 10;
	AosJimoRandomStrGenTest *jimo = (AosJimoRandomStrGenTest*)abs_jimo;
	u32 prefixLen = jimo->randomData(jimo->mPrefixLenList);
	u32 postfixLen = jimo->randomData(jimo->mPostfixLenList);
	u32 totalLen = jimo->randomData(jimo->mTotalLenList);
	while((prefixLen + postfixLen) > totalLen && loop)
	{
		prefixLen = jimo->randomData(jimo->mPrefixLenList);
		postfixLen = jimo->randomData(jimo->mPostfixLenList);
		loop--;
	}
	u32 bodyLen = totalLen - postfixLen - prefixLen;
	
	string prefix = "";
	jimo->createStr(prefix, prefixLen, jimo->mPrefixList);
	string body = "";
	jimo->createStr(body, bodyLen, jimo->mBodyList);
	string postfix = "";
	jimo->createStr(postfix, postfixLen, jimo->mPostfixList);
	
	jimo->mNewStr << prefix << body << postfix; 
	aos_assert_r(jimo->mNewStr != "", false);
	// OmnScreen << "String generated: " << jimo->mNewStr << endl;
	return true;
}


AosJimoRandomStrGenTest::AosJimoRandomStrGenTest(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc)
{
	bool rslt = config(rdata, worker_doc, jimo_doc);
	if (!rslt)
	{
		OmnThrowException(rdata->getErrmsg());
	}
	registerMethods();
}


bool
AosJimoRandomStrGenTest::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	registerMethod("next_value", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoRandomStrGenTest_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoRandomStrGenTest::AosJimoRandomStrGenTest(
		const AosJimoRandomStrGenTest &rhs)
:
AosJimo(rhs),
mType(rhs.mType),
mNewStr(rhs.mNewStr), 
mPrefixList(rhs.mPrefixList), 
mPostfixList(rhs.mPostfixList),
mBodyList(rhs.mBodyList),
mPrefixLenList(rhs.mPrefixLenList),
mPostfixLenList(rhs.mPostfixLenList),
mTotalLenList(rhs.mTotalLenList)
{

}

AosJimoRandomStrGenTest::~AosJimoRandomStrGenTest()
{
}

bool
AosJimoRandomStrGenTest::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	// total_length = prefix_length + postfix_length + body_length
	// <field type=”ascii_str”
	//		prefix_charset=”min:max:weight,min:max:weight,…”
	//		postfix_charset=”min:max:weight,min:max:weight,…”
	//		body_charset=”min:max:weight,min:max:weight,…”
	//		prefix_length=”min:max:weight,min:max:weight,…”
	//		postfix_length=”min:max:weight,min:max:weight,…”
	//		total_length=”min:max:weight,min:max:weight,…”
	// />
	aos_assert_rr(worker_doc, rdata, false);
	mType = worker_doc->getAttrStr("type");
	aos_assert_rr(mType != "", rdata, false);

	OmnString prefix = worker_doc->getAttrStr("prefix_charset");
	aos_assert_rr(prefix != "", rdata, false);
	AosStrSplit split(prefix, ",");
	mPrefixList = split.entriesV();
	
	OmnString postfix = worker_doc->getAttrStr("postfix_charset");
	aos_assert_rr(postfix != "", rdata, false);
	split.splitStr(postfix.data(), ",");
	mPostfixList = split.entriesV();

	OmnString body = worker_doc->getAttrStr("body_charset");
	aos_assert_rr(body != "", rdata, false);
	split.splitStr(body.data(), ",");
	mBodyList = split.entriesV();

	OmnString prefix_length = worker_doc->getAttrStr("prefix_length");
	aos_assert_rr(prefix_length != "", rdata, false);
	split.splitStr(prefix_length.data(), ",");
	mPrefixLenList = split.entriesV();

	OmnString postfix_length = worker_doc->getAttrStr("postfix_length");
	aos_assert_rr(postfix_length != "", rdata, false);
	split.splitStr(postfix_length.data(), ",");
	mPostfixLenList = split.entriesV();

	OmnString total_length = worker_doc->getAttrStr("total_length");
	aos_assert_rr(total_length != "", rdata, false);
	split.splitStr(total_length.data(), ",");
	mTotalLenList = split.entriesV();

	return true;
}


int
AosJimoRandomStrGenTest::randomData(vector<OmnString> tmp)
{
	map<size_t, vector<OmnString> > charset_map;
	AosStrSplit split;
	size_t len = tmp.size();
	aos_assert_r(len, 0);
	for (size_t i = 0; i < len; i++)
	{
		split.splitStr(tmp[i].data(), ":");
		vector<OmnString> charset = split.entriesV();
		charset_map.insert(make_pair(i, charset));
	}

	u32 ww = 0;
	for (size_t i = 0; i < len; i++)
	{
		map<size_t, vector<OmnString> >::iterator itr;
		itr = charset_map.find(i);
		if (itr != charset_map.end())
		{
			ww += (itr->second)[2].toInt();
		}
	}

	u32 idx = rand() % ww;
	int loop = 10;
	while(loop)
	{
		for (size_t i = 0; i < len; i++)
		{
			u32 weight = 0;
			for (size_t j = 0; j <= i; j++)
			{
				map<size_t, vector<OmnString> >::iterator itr2;
				itr2 = charset_map.find(j);
				if (itr2 != charset_map.end())
				{
					weight += (itr2->second)[2].toInt();
				}
			}

			if(idx < weight)
			{
				map<size_t, vector<OmnString> >::iterator itr3;
				itr3 = charset_map.find(i);
				if (itr3 != charset_map.end())
				{
					int min = (itr3->second)[0].toInt();
					int max = (itr3->second)[1].toInt();
					if ( min >= max) return min;
					return (rand() % (max-min)) + min; 
				}
			}
		}
		--loop;
	}
	return -1;
}


OmnString
AosJimoRandomStrGenTest::toString() const
{
	return mNewStr;
}


bool 
AosJimoRandomStrGenTest::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoRandomStrGenTest::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoRandomStrGenTest::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoRandomStrGenTest(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoRandomStrGenTest::supportInterface(
		const AosRundataPtr &rdata, 
		const OmnString &interface_objid) const
{
	AosXmlTagPtr doc = AosGetDocByObjid(interface_objid, rdata);
	if (!doc)
	{
		return false;
	}

	int id = doc->getAttrInt(AOSTAG_INTERFACE_ID, -1);
	return supportInterface(rdata, id);
}


bool 
AosJimoRandomStrGenTest::supportInterface(
		const AosRundataPtr &rdata, 
		const int interface_id) const
{
	switch (interface_id)
	{
	case eRunWithSmartDoc:
		 return true;

	default:
		 break;
	}
	return false;
}


bool
AosJimoRandomStrGenTest::createStr(string &str, const u32 len, vector<OmnString> vec)
{
	aos_assert_r(len, false);
	for (size_t i =0; i < len; i++)
	{
		int ch = randomData(vec);
		str.append<int>(1, ch);
	}
	return true;
}

