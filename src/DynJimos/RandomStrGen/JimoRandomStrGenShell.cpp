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
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoRandomStrGen_0(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &jimo_doc, 
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoRandomStrGen(rdata, jimo_doc, version);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


OmnString
AosJimoRandomStrGen_nextValue(		
		const AosRundataPtr &rdata, 
		const AosJimoPtr &abs_jimo,
		const AosXmlTagPtr &worker, 
		const OmnString &dft)
{
	size_t loop = 10;
	AosJimoRandomStrGen *jimo = (AosJimoRandomStrGen*)abs_jimo.getPtr();
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
	aos_assert_r(jimo->mNewStr != "", dft);
	return jimo->mNewStr;
}


