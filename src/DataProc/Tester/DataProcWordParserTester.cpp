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
//
// Modification History:
// 2013/06/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/Tester/DataProcWordParserTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThreadPool.h"
#include "Thread/Mutex.h"


AosDataProcWordParserTester::AosDataProcWordParserTester()
:
mFinished(false)
{
	config();
}


AosDataProcWordParserTester::~AosDataProcWordParserTester()
{
}

bool
AosDataProcWordParserTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr test_node = conf->getFirstChild("tester");
	aos_assert_r(test_node, false);
	mConf = test_node->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(mConf, false);
	
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
	
	mTestNum = mConf->getAttrInt64("test_num", 1000);
	mThrdNum = mConf->getAttrInt("thrd_num", 1);

}

bool 
AosDataProcWordParserTester::start()
{
	OmnThreadPool threadPool("AosDataProcWordParserTester", __FILE__, __LINE__); 
	while(1)
	{
		vector<OmnThrdShellProcPtr> v;
		for (int i=0; i<mThrdNum; i++)
		{                                               
			OmnThrdShellProcPtr t = OmnNew RunTest(this, mRundata.getPtr(), mConf->clone(AosMemoryCheckerArgsBegin)); 
			v.push_back(t);
		}
		int start_time = OmnGetSecond();
		threadPool.procSync(v); 
		OmnScreen << "AosDataProcWordParserTester " << mTestNum*mThrdNum << " :: " << OmnGetSecond() - start_time << endl;
	}
	//run();
}

bool 
AosDataProcWordParserTester::RunTest::init()
{
	mTestNum = mConf->getAttrInt64("test_num", 1000);

	bool rslt = initDataProc(mConf);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr record_node = mConf->getFirstChild("datarecords");
	aos_assert_r(record_node, false);

	rslt = initRecord(mInputRcds, record_node->getFirstChild("input_rcds"));
	aos_assert_r(rslt, false);

	rslt = initRecord(mOutputRcds, record_node->getFirstChild("output_rcds"));
	aos_assert_r(rslt, false);

	rslt = setInputRcdValue(mConf->getFirstChild("input_rcd_values"));
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataProcWordParserTester::RunTest::initDataProc(const AosXmlTagPtr &test_conf)
{
	aos_assert_r(test_conf, false);
	AosXmlTagPtr conf = test_conf->getFirstChild("dataproc");
	aos_assert_r(conf, false);
	mDataProc = AosDataProcObj::createDataProcStatic(conf, mRundata);
	aos_assert_r(mDataProc, false);
	mDataProc->setTask(0);
	
	vector<RecordFieldInfo *> rf_infos;
	mDataProc->getRecordFieldInfosRef(rf_infos);

	AosXmlTagPtr rcd_info_conf = test_conf->getFirstChild("rf_infos");
	aos_assert_r(rcd_info_conf, false);
	AosXmlTagPtr node;

	OmnString field_name;
	for(u32 i=0; i<rf_infos.size(); i++)
	{
		OmnString name = rf_infos[i]->mRecordName;
		node  = rcd_info_conf->getChildByAttr("record_name", name);
		aos_assert_r(node, false);
		field_name = rf_infos[i]->mFieldName;
		node = node->getChildByAttr("field_name", field_name);
		aos_assert_r(node, false);
		rf_infos[i]->mIsDataEngineInput = node->getAttrBool("is_input", true);
		rf_infos[i]->mRecordIdx = node->getAttrU32("rcd_idx", 0);
		rf_infos[i]->mFieldIdx = node->getAttrU32("field_idx", 0);
	}
	return true;
}


bool
AosDataProcWordParserTester::RunTest::initRecord(
		vector<AosDataRecordObj*> &rcds, 
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	AosValueRslt vv;
	AosXmlTagPtr rcd_conf = conf->getFirstChild(true);
	while (rcd_conf)
	{
		AosDataRecordObjPtr rcd = AosDataRecordObj::createDataRecordStatic(rcd_conf, NULL, mRundata) ;
		aos_assert_r(rcd, false);
		mRcds.push_back(rcd);
		rcds.push_back(rcd.getPtr());
		rcd_conf = conf->getNextChild();
	}
	return true;
}

bool 
AosDataProcWordParserTester::RunTest::setInputRcdValue(const AosXmlTagPtr &conf)
{
	if (mInputRcdValues.empty())
	{
		aos_assert_r(conf, false);
		OmnString name;
		vector<OmnString> values;
		AosXmlTagPtr rcd = conf->getFirstChild(true);
		while (rcd)
		{
			values.clear();
			name = rcd->getAttrStr("name");
			AosXmlTagPtr val_conf = rcd->getFirstChild(true);
			while (val_conf)
			{
				values.push_back(val_conf->getNodeText());
				val_conf = rcd->getNextChild();
			}
			mInputRcdValues[name] = values;
			rcd = conf->getNextChild();
		}
	}

	AosValueRslt vv;
	bool outofmem = false;
	for (u32 i = 0; i < mInputRcds.size(); i++)	
	{
		OmnString name = mInputRcds[i]->getRecordName();
		if (mInputRcdValues.find(name) != mInputRcdValues.end())
		{
			vector<OmnString> strV = mInputRcdValues[name];
			for (u32 j = 0; j < strV.size(); j++)
			{
				vv.setValue(strV[j]);
				mInputRcds[i]->setFieldValue(j, vv, outofmem, mRundata);
			}
		}
	}
	return true;
}


bool		
AosDataProcWordParserTester::RunTest::run()
{
	bool rslt = init();
	aos_assert_r(rslt, false);
	int64_t tmp = 0;
	int start_time = OmnGetSecond();
	//int64_t times = 1 * 1000 * 1000;

	while (tmp <= mTestNum)
	{
		procData();
		tmp++;
		/*
		if (tmp%times == 0)
		{
			OmnScreen << "AosDataProcWordParserTester " << times << " :: " << OmnGetSecond() - start_time << endl;
			start_time = OmnGetSecond();
		}
		*/
	}
	OmnScreen << "AosDataProcWordParserTester " << mTestNum << " :: " << OmnGetSecond() - start_time << endl;
	mDataProc->finish(mRundata);
	return true;
}


bool
AosDataProcWordParserTester::RunTest::procData()
{
	// It processes the data

//	vector<AosDataRecordObj*>       mInputRcds;
//	vector<AosDataRecordObj*>       mOutputRcds;
	mDataProc->procData(mRundata, &(mInputRcds[0]), &(mOutputRcds[0]));
	for(size_t i=0; i<mOutputRcds.size(); i++)
	{
		mOutputRcds[i]->clear();
	}
	//type = mDataProc->procData(mRundata.getPtr(), &(mInputRcd.getPtr()), &output);
//	int field_idx = mOutputRcd->getFieldIdx("docid", mRundata.getPtr());
//	bool rslt = mOutputRcd->getFieldValue(field_idx, value_rslt, true, mRundata.getPtr());
//	aos_assert_r(rslt, false);
//	aos_assert_r(value_rslt.getValueStr1() == "value", false);
	return true;
}


