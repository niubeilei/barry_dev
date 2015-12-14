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
// 08/09/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/Testers/IILTester3.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/Ptrs.h"
#include "IILMgr/IILStr.h"
#include "IILTrans/AllIILTrans.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"

AosIILTester3::AosIILTester3()
{
}


bool 
AosIILTester3::start()
{
	mRundata = OmnApp::getRundata();

	vector<AosTransPtr> v;
	for (int i=1; i<=500000; i++)
	{
		char num[10];
		sprintf(num, "%06d", i);
		AosIILTransPtr trans;// = 
			//OmnNew AosIILTransHitAddDocByName("good", false, 
			//		i, false, false);
			//OmnNew AosIILTransStrAddValueDocByName("_zt19_good", false, num, i, 
			//		false, false, false, false);
			//OmnNew AosIILTransU64AddValueDocByName("_zt19_good", false, i, i, 
			//		false, false, false, false);
		v.push_back(trans);
		if (i%1000 == 0)
		{
			AosSendManyTrans(mRundata, v);
			v.clear();
		}
		if (i%2000 == 0)
		{
			AosQueryRsltObjPtr query_rslt =           
				    AosQueryRsltObj::getQueryRsltStatic();
			AosQueryContextObjPtr query_context = 
					AosQueryContextObj::createQueryContextStatic();
			query_context->setOpr(eAosOpr_an);
			query_context->setPageSize(1000000);
			query_context->setBlockSize(1000000);
			query_rslt->setWithValues(true);
			AosIILTransPtr trans;// = OmnNew AosIILTransQueryDocByName(
			//		"_zt19_good", query_rslt, NULL, query_context, false, true); 

			AosBuffPtr buff;
			bool timeout;
			bool rslt = AosSendTrans(mRundata, trans, timeout, buff);
			aos_assert_r(rslt && buff, false);
			rslt = buff->getU8(0);   
			aos_assert_r(rslt, false);

			u32 len = buff->getU32(0);
			aos_assert_r(len, 0);
			AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
			aos_assert_r(b, 0);
			AosXmlParser parser;
			AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
			aos_assert_r(xml, false);

			rslt = query_context->serializeFromXml(xml, mRundata);
			aos_assert_r(rslt, false);

			if (query_rslt)
			{
				u32 len = buff->getU32(0);
				aos_assert_r(len, 0);
				AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
				aos_assert_r(b, 0);
				AosXmlParser parser;
				AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
				aos_assert_r(xml, false);

				rslt = query_rslt->serializeFromXml(xml);
				aos_assert_r(rslt, false);
			}
			int total = query_rslt->getNumDocs();
			aos_assert_r(total== i, false);
			u64 docid = 0;
			bool finish;
			for (int i=1 ;i<=total ; i++)
			{
				 //query_rslt->nextDocid(docid, finish, mRundata);
				 //aos_assert_r(docid == i, false);
				 
			     //OmnString s;
				 //query_rslt->nextDocidValue(docid, s, finish, mRundata);
				 //aos_assert_r(atoi(s.data()) == i, false);
				 //aos_assert_r(docid == i, false);
				 
			     u64 s;
				 query_rslt->nextDocidValue(docid, s, finish, mRundata);
				 aos_assert_r(s == i, false);
				 aos_assert_r(docid == i, false);
			}
		}
	}
		
	return true;
}
