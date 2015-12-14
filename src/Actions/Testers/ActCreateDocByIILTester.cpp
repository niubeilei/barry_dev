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
// How to torture: 
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActCreateDocByIILTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/IILName.h"
#include "IILClient/IILClient.h"
#include "IILClient/Ptrs.h"
#include "IILAssembler/IILAssembler.h"
#include "Porting/Sleep.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryContextObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

AosIILClientObjPtr AosActCreateDocByIILTester::smIILClient;
AosActCreateDocByIILTester::AosActCreateDocByIILTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mRundata->setSiteid(100);
	mRundata->setUserid(307);
	mName = "ActCreateDocByIILTester";
}


AosActCreateDocByIILTester::~AosActCreateDocByIILTester()
{
}


bool 
AosActCreateDocByIILTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActCreateDocByIILTester::basicTest()
{
	aos_assert_r(config(), false);
	smIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(smIILClient, false);
	mFirstRun = true;
	int i = 1;
	while(i <= 500)
	{
OmnScreen << "=======================Tries: " << i++ << endl;
	clearData();
	createData();
	//u32 n=0; 
	//for(OmnRecordMapItr mit = mRecords.begin(); mit != mRecords.end(); mit++)
	//{
	//	OmnScreen << "================================: " << n++ << endl;
	//	OmnScreen << "1111=============================: cpn : " << mit->first << endl;
	//	map<OmnString, OmnString> mp = mit->second;	
	//	for(map<OmnString, OmnString>::iterator mattr = mp.begin(); mattr != mp.end(); mattr++)
	//	{
	//		OmnScreen << "2222=============================: " << mattr->first << " : " << mattr->second << endl;
	//	}
	//}
	createLocalResults();
	runAction();
	while(mDocids.size() == 0)
	{
		OmnSleep(10);
		retrieveResults();
OmnScreen << "doc size: " << mDocids.size() << " local size: " << mRecords.size() << endl;
	}
	compareResults();
	}
	return true;
}


bool
AosActCreateDocByIILTester::config()
{
	//<testers name="createdoc" result_iilname="xxxx" keyname="xxxx">
	//	<iils>
	//		<iil attrname="call_duration" iilname="_zt44_iil001" value_len="10" >
	//		</iil>
	//		<iil attrname="call_num" iilname="_zt44_iil002" value_len="10" >
	//		</iil>
	//		<iil attrname="call_time" iilname="_zt44_iil003" value_len="10" >
	//		</iil>
	//		<iil attrname="town_code" iilname="_zt44_iil004" value_len="10" >
	//		</iil>
	//	</iils>
	//	<sdoc>
	//		<action .../>
	//		...
	//	</sdoc>
	//</testers>
		
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);

	AosXmlTagPtr conf = app_conf->getFirstChild("testers");
	aos_assert_r(conf, false);

	AosXmlTagPtr iils = conf->getFirstChild("iils");
	aos_assert_r(iils, false);

	mResultIILName = iils->getAttrStr("result_iilname", "");
	aos_assert_r(mResultIILName != "", false);

	mKeyName = iils->getAttrStr("keyname", "");
	aos_assert_r(mKeyName != "", false);

	AosXmlTagPtr sdoc = conf->getFirstChild("sdoc");
	aos_assert_r(sdoc, false);
	mAction = sdoc->getFirstChild("action");
	aos_assert_r(mAction, false);

	AosXmlTagPtr iil = iils->getFirstChild("iil");
	while(iil)
	{
		mIILInfo.push_back(iil);
		iil = iils->getNextChild();
	}
	return true;
}


bool
AosActCreateDocByIILTester::clearData()
{
	if (!mFirstRun)
	{
		for(u32 i=0; i<mIILInfo.size(); i++)
		{
			AosXmlTagPtr iil = mIILInfo.at(i);
			OmnString iilname = iil->getAttrStr("iilname");
			aos_assert_r(iilname != "", false);
			aos_assert_r(AosIILClient::getSelf()->deleteIILByName(iilname, mRundata), false);

			OmnString attrname = iil->getAttrStr("attrname");
			OmnString idx_iilname = "_zt44_cpn_doc_0609_3_";
			idx_iilname << attrname;
			aos_assert_r(AosIILClient::getSelf()->deleteIILByName(idx_iilname, mRundata), false);
		}
		OmnString idx_iilname = "_zt44_cpn_doc_0609_3_";
		idx_iilname << mKeyName;
		aos_assert_r(AosIILClient::getSelf()->deleteIILByName(idx_iilname, mRundata), false);
		OmnRecordMapItr it;
		for(it = mRecords.begin(); it != mRecords.end(); it++)
		{
			(it->second).clear();
		}
		OmnSleep(10);
	}
	else
	{
		mFirstRun = false;
	}

	mDocids.clear();
	mKeys.clear();
	mDistinctCpns.clear();
	mRecords.clear();
	return true;
}


bool
AosActCreateDocByIILTester::createLocalResults()
{
	return true;
}


bool
AosActCreateDocByIILTester::runAction()
{
	aos_assert_r(mAction, false);
	aos_assert_r(mRundata->getSiteid() > 0, false);
	AosActionObjPtr action = AosActionObj::createAction(mAction, mRundata);
	aos_assert_r(action, false);
	return action->run(mAction, mRundata);
}


bool
AosActCreateDocByIILTester::compareResults()
{
	aos_assert_r(mDocids.size() == mRecords.size(), false);
	u64 docid = 0;
	AosXmlTagPtr doc;
	OmnRecordMapItr itr;
	u32 n = 0;
	for(u32 i=0; i<mDocids.size(); i++)
	{
		docid = mDocids.at(i);
		doc = AosGetDocByDocid(docid, mRundata);
		aos_assert_r(doc, false);
//OmnScreen << doc->toString() << endl;

		//compare to hash_map;
		OmnString keyvalue = doc->getNodeText(mKeyName);
		aos_assert_r(keyvalue != "", false);

		//find record
		itr = mRecords.find(keyvalue);
		aos_assert_r(itr != mRecords.end() , false);

		//compare all the attrs

		map<OmnString, OmnString>::iterator it;
		for(it = (itr->second).begin(); it != (itr->second).end(); it++)
		{
			OmnString local_name = it->first;
			OmnString local_value = it->second;
			OmnString doc_value = doc->getNodeText(local_name);
//OmnScreen << "size=" << mDocids.size() << " idx: " << n << " [" << local_name << "] [local: " << local_value << "] , [doc: " << doc_value << "]" <<endl;
			aos_assert_r(atoll(local_value.data()) == atoll(doc_value.data()), false);
		}
//OmnScreen << "===================================================" << endl;
		n++;
	}
	return true;
}

bool
AosActCreateDocByIILTester::retrieveResults()
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	query_context->setStrValue("*");
	query_context->setReverse(false);
	query_context->setBlockSize(100000);

	int i = 0;
	while(1)
	{
		if (i > 100)
		{
			break;
		}
		AosQueryRsltPtr query_rslt = AosQueryRslt::getQueryRslt();
		query_rslt->setWithValues(true);
		aos_assert_r(AosQueryColumn(mResultIILName, query_rslt, 0, query_context, mRundata), false);


		while (1)
		{
			u64 docid;
			OmnString key;
			bool finished;
			aos_assert_r(query_rslt->nextDocidValue(docid, key, finished, mRundata), false);
			if (finished) break;
			mKeys.push_back(key);
			mDocids.push_back(docid);
		}
		if (query_context->finished())
		{
			break;
		}
	}
	return true;
}

bool
AosActCreateDocByIILTester::createCpn()
{
	createCompetitorPrefix();

	mNumCpns = 100 + rand() % 100000;
	OmnString cpn;
	u32 num_prefix = mCompetitorPrefix.size();
	while((u32)mNumCpns != mDistinctCpns.size())
	{
		cpn = "";
		if (rand() % 4 == 0)
		{
			// Randomly generate a cpn
			cpn = OmnRandom::cellPhoneNumber(24, 13, false, true, ' ');
			mDistinctCpns[cpn] = 1;
		}
		else
		{
			// Pick a prefix
			cpn = mCompetitorPrefix[rand() % num_prefix];
			cpn << OmnRandom::cellPhoneNumber(16, 5, false, true, ' ');
			mDistinctCpns[cpn] = 1;
		}
	}
	return true;
}

bool
AosActCreateDocByIILTester::createCompetitorPrefix()
{
	mCompetitorPrefix.clear();
	mCompetitorPrefix.push_back("13611111");
	mCompetitorPrefix.push_back("13622222");
	mCompetitorPrefix.push_back("13633333");
	mCompetitorPrefix.push_back("13644444");
	return true;
}


bool
AosActCreateDocByIILTester::createData()
{
	// This function creates all the IILs. Each IIL 
	// 1. Create CPN
	createCpn();
OmnScreen << "=========================cpns: " << mDistinctCpns.size() << endl;

	map<OmnString, AosXmlTagPtr>::iterator it;

	for(u32 n=0; n<mIILInfo.size(); n++)
	{
		int stepWeight = rand() % 10;
		if (stepWeight == 0)
		{
			stepWeight++;
		}
		// Pick CPNS
		OmnString docid, cpn;
		AosXmlTagPtr iil = mIILInfo.at(n);
		aos_assert_r(iil, false);
		OmnString iilname = iil->getAttrStr("iilname");
		OmnString attrname = iil->getAttrStr("attrname");
		int docid_len = iil->getAttrInt("docid_len", -1);

		map<OmnString, int>::iterator cpn_it;
		for(cpn_it = mDistinctCpns.begin(); cpn_it != mDistinctCpns.end(); )
		{
			if (attrname == "town_code")
			{
				cpn = cpn_it->first;
				cpn_it++;
			}
			else
			{
				int nn = (rand() % stepWeight);
				if (nn == 0)
				{
					cpn_it++;
				}
				for(int i=0; i<nn; i++)
				{
					cpn_it++;
					if (cpn_it == mDistinctCpns.end())
					{
						break;
					}
				}
				if (cpn_it == mDistinctCpns.end())
				{
					break;
				}
				cpn = cpn_it->first;
			}

			cpn.normalizeWhiteSpace(false, true);

			if (attrname == "first_call_in90")
			{
				u64 time = 1325376000 + (rand() % 60*60*24);
				docid = "";
				docid << time;
				OmnString cpn_compse = "";
				cpn_compse << cpn << "$$" << 1325376000;
				//cpn_compse << cpn;
				bool rslt = smIILClient->addStrValueDoc(iilname, cpn_compse, atoll(docid.data()),
						true, false, mRundata);
				aos_assert_r(rslt, false);
			}
			else
			{
				docid = OmnRandom::digitStr(docid_len);
				bool rslt = smIILClient->addStrValueDoc(iilname, cpn, atoll(docid.data()),
						true, false, mRundata);
				aos_assert_r(rslt, false);
			}

			//creataLocalResult
			OmnRecordMapItr it = mRecords.find(cpn);
			if (it != mRecords.end())
			{
				aos_assert_r((it->second).find(attrname) == (it->second).end(), false);
				(it->second)[attrname] = docid;
			}
			else
			{
				map<OmnString, OmnString> attrs;
				attrs[attrname] = docid;
				mRecords[cpn] = attrs;
			}
		}
	}
	return true;
}

