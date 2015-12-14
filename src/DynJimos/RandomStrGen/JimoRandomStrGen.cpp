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
#include "DynJimos/RandomStrGen/JimoRandomStrGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DynJimos/RandomStrGen/JimoRandomStrGenShell.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

AosJimoRandomStrGen::AosJimoRandomStrGen(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosJimo(rdata, jimo_doc, AosJimoType::eDataProc, version)
{
	if (!init(rdata, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosJimoRandomStrGen::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool
AosJimoRandomStrGen::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	registerMethod("next_value", sgFuncMap, 
			AosMethodId::eAosRundata_Jimo_WorkerDoc,
			(void *)AosJimoRandomStrGen_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoRandomStrGen::AosJimoRandomStrGen(
		const AosJimoRandomStrGen &rhs)
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

AosJimoRandomStrGen::~AosJimoRandomStrGen()
{
}


bool
AosJimoRandomStrGen::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
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
AosJimoRandomStrGen::randomData(vector<OmnString> tmp)
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
AosJimoRandomStrGen::toString() const
{
	return mNewStr;
}


bool 
AosJimoRandomStrGen::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoRandomStrGen::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoRandomStrGen::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoRandomStrGen(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoRandomStrGen::run(const AosRundataPtr &rdata) 
{
	size_t loop = 10;
	u32 prefixLen = randomData(mPrefixLenList);
	u32 postfixLen = randomData(mPostfixLenList);
	u32 totalLen = randomData(mTotalLenList);
	while((prefixLen + postfixLen) > totalLen && loop)
	{
		prefixLen = randomData(mPrefixLenList);
		postfixLen = randomData(mPostfixLenList);
		loop--;
	}
	u32 bodyLen = totalLen - postfixLen - prefixLen;
	
	string prefix = "";
	createStr(prefix, prefixLen, mPrefixList);
	string body = "";
	createStr(body, bodyLen, mBodyList);
	string postfix = "";
	createStr(postfix, postfixLen, mPostfixList);
	
	mNewStr << prefix << body << postfix; 
	aos_assert_r(mNewStr != "", false);
	// OmnScreen << "String generated: " << mNewStr << endl;
	return true;
}


bool
AosJimoRandomStrGen::createStr(string &str, const u32 len, vector<OmnString> vec)
{
	aos_assert_r(len, false);
	for (size_t i =0; i < len; i++)
	{
		int ch = randomData(vec);
		str.append<int>(1, ch);
	}
	return true;
}


void * 
AosJimoRandomStrGen::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}

