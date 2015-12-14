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
// Testing the Conditions:
// 1. It creates an array of docs. 
// 	  <doc gruop_id="ddd" 	// u64
// 	  	   field_str_00="random_str"
// 	  	   ...
// 	  	   field_str_nn="random_str"
// 	  	   field_u64_00="random_str"
// 	  	   ...
// 	  	   field_u64_nn="random_str"/>
// 	  Field values are calculated as:
// 	  		value[field_id] = strFieldValue(field_id, docid);
// 	  		value[field_id] = u64FieldValue(field_id, docid);
// 	  It uses 'docid' to create the value. 
//
// 2. Once docs are created, it creates doc groups by randomly
//    picking some docs. For each doc being picked, it adds
//    a field:
//    		group_field_<group_id>="group_id"
//    Docs are selected by rules, such as all the odd docids, 
//    all the even docids, etc.
//	  A group is defined as:
//	  	Group = 
//	  	{
//	  		group_id: ddd,
//	  		doc rule: rule, // the rule that determines the membership
//			field_str_00 Condition,
//			field_str_01 Condition,
//			...
//			field_u64_00 Condition,
//			field_u64_01 Condition,
//			...
//		}
// 3. Randomly pick a group, formulate the conditions, run the query, 
//    and then check the results.
//
// 4. How to handle parallel IILs
//    In the current implementations, there are totally five string IILs:
//    		str_field_0
//    		str_field_1
//    		str_field_2
//    		str_field_3
//    		str_field_4
//    To support parallel IIL, 
//
//    a. The third str field is treated as the time field.
//    b. The first, third, and fifth fields are parallel IILs.
//    c. 
// Modification History:
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordIDMgr/Testers/WordIDMgrTester.h"

#include "WordIDMgr/WordIDMgr.h"
#include "WordIDMgr/SmartArray.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "QueryTorturer/QueryGroup.h"
#include "QueryTorturer/QueryField.h"
#include "QueryTorturer/QueryFieldNum.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEUtil/IILIdx.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadPool.h"
#include "Thread/Thread.h"
#include "UtilTime/TimeUtil.h"
#include "Util/BuffArray.h"
#include "Util/File.h"
#include "Util/HashUtil.h"
#include "Util/UtUtil.h"
#include "Util1/MemMgr.h"


static OmnThreadPool sgThreadPool("field_data_creators", __FILE__, __LINE__);
static vector<AosQueryField::Pattern>	sgPatterns;

AosWordIDMgrTester::AosWordIDMgrTester()
:
mTries(0)
{
	mWordMgr = OmnNew AosWordIDMgr();
}


AosWordIDMgrTester::~AosWordIDMgrTester()
{
}


bool 
AosWordIDMgrTester::start()
{	
	OmnScreen << "请选择以下测试内容: " << endl;
	
	OmnScreen << "======================================" << endl;
	OmnScreen << "1.随机产生word, get word id  1000万次, 测试效率" << endl;
	OmnScreen << "2.建立1000万词汇的单词表" << endl;
	OmnScreen << "  在已知单词表中随机挑选word查询1000万次, 测试效率" << endl;
	OmnScreen << "3.建立1000万词汇的单词表" << endl;
	OmnScreen << "  在已知单词表中随机挑选word id查询1000万次, 测试效率" << endl;
	OmnScreen << "4.建立1000万词汇的单词表" << endl;
	OmnScreen << "  在已知单词表中随机挑选两个word id进行大小比较, 1000万次, 测试效率" << endl;
	OmnScreen << "  做1000万次 strcmp 进行对比" << endl;
	OmnScreen << "5.随机以下行为：" << endl;
	OmnScreen << "   1. 产生word, 查询word id, 并记录到vector中"<< endl;
	OmnScreen << "   2.（vector.size() >= 1）从vector中随机提取一个word 查询word id, 看是否一致" << endl;
	OmnScreen << "   3. （vector.size() >= 2）从vector中随即提取两个word，比较两个word的大小，通过word id mgr比较两个							word的大小，对比结果是否一致" << endl;
	OmnScreen << "   4.save word id mgr, load word id mgr" << endl;
	OmnScreen << "   5.sync测试（待定）" << endl;
	OmnScreen << "======================================" << endl;
	int select1;
	cin >> select1 ;
	
	bool rslt = false;
	switch(select1)
	{
		case 1:
				//随机产生word, get word id  1000万次, 测试效率
				rslt = addWordPfmTest();
				break;
		case 2:
				//在已知单词表中随机挑选word查询1000万次, 测试效率
				rslt = getWordIDPfmTest();
				break;
		case 3:	
				//在已知单词表中随机挑选word id查询1000万次, 测试效率
				rslt = getWordPfmTest();
				break;
		case 4:
				//在已知单词表中随机挑选两个word id进行大小比较, 1000万次, 测试效率
				//做1000万次 strcmp 进行对比
				rslt = compWordByIDPfmTest();
				break;
		case 5:
				rslt = normalTest();
				break;
			default: break;
	}
	aos_assert_r(rslt,false);
	return true;
}

bool 
AosWordIDMgrTester::addWordPfmTest()
{
	mRandStr.resize(10000000);
	for(int i = 0;i < 10000000;i++)
	{
		mRandStr[i] = random_str();
	}
	//记时
	time_t start,stop;

	start = time(NULL);
	u64 word_id;
	for (u64 i = 0; i < 10000000; i++)
	{
		word_id = mWordMgr->getWordIDSafe(mRandStr[i]);
		if( i % 100000 == 99999)
		{
			stop = time(NULL);
			OmnScreen << "运行:" << i/100000 +1 << "%，  消耗时间：..." << endl;
			OmnScreen << stop -start << " 秒" << endl;
			start = time(NULL);
		}
	}
}


bool
AosWordIDMgrTester::getWordIDPfmTest()
{
    //建立单词表；
	u64 word_id;
	mRandStr.resize(10000000);
	mOrigID.resize(10000000);
	OmnScreen << "建立单词表" << endl;
	for(u64 i = 0; i < 100000; i++)
	{
		mRandStr[i] = random_str();
		word_id = mWordMgr->getWordIDSafe(mRandStr[i]);
		mOrigID[i] = word_id;
	}

	OmnScreen << "单词表建立完成" << endl;
	//记时
	u64 start = 0;
	u64 stop = 0;
	u64 idx= 0;
	start = time(NULL);
	for (u64 i = 0; i < 10000000; i++)
	{
		idx = rand()%100000;
		word_id = mWordMgr->getWordIDSafe(mRandStr[idx]);
		if(i%100000 == 99999)
		{
			stop = time(NULL);
			OmnScreen << "运行:" << i/100000 << "%，  消耗时间："<< endl;
			OmnScreen << stop -start << " 秒" << endl;
			start = time(NULL);
		}
	}
}


bool
AosWordIDMgrTester::getWordPfmTest()
{
    //建立单词表；
	u64 word_id;
	mRandStr.resize(10000000);
	mOrigID.resize(10000000);
	OmnScreen << "建立单词表" << endl;
	for(u64 i = 0; i < 100000; i++)
	{
		mRandStr[i] = random_str();
		word_id = mWordMgr->getWordIDSafe(mRandStr[i]);
		mOrigID[i] = word_id;
	}
	OmnScreen << "单词表建立完成" << endl;
	//记时
	u64 start = 0;
	u64 stop = 0;
	u64 idx= 0;
	start = time(NULL);
	for (u64 i = 0; i < 10000000; i++)
	{
		idx = rand()%100000;
		mWordMgr->getWord(mOrigID[idx]);
	}
	stop = time(NULL);
	OmnScreen << "消耗时间：";
	OmnScreen << stop -start << " 秒"<< endl;
	return true;
}

bool
AosWordIDMgrTester::compWordByIDPfmTest()
{
    	//建立单词表；
	u64 word_id;
	mRandStr.resize(10000000);
	mOrigID.resize(10000000);
	OmnScreen << "建立单词表" << endl;
	for(u64 i = 0; i < 100000; i++)
	{
		mRandStr[i] = random_str();
		word_id = mWordMgr->getWordIDSafe(mRandStr[i]);
		mOrigID[i] = word_id;
	}
	OmnScreen << "单词表建立完成" << endl;

	//记时
	u64 start = 0;
	u64 stop = 0;
	u64 idx= 0;
	
	u64 idx1;
	u64 idx2;


u64 id_array[100000];	
i64 xxxx;
	start = time(NULL);
	for (u64 i = 0; i < 100000000; i++)
	{
		
		idx1 = rand()%100000;
		idx2 = rand()%100000;
		
		xxxx = id_array[idx1] - id_array[idx2];
//		mWordMgr->compareWord(mOrigID[idx1],mOrigID[idx2]);
	}
	stop = time(NULL);
	OmnScreen << "使用 wordid比较 消耗时间：...";
	OmnScreen << stop -start << endl;

	start = time(NULL);
	for (u64 i = 0; i < 100000000; i++)
	{
		idx1 = rand()%100000;
		idx2 = rand()%100000;
		strcmp(mRandStr[idx1].data(),mRandStr[idx2].data());
	}
	stop = time(NULL);
	OmnScreen << "使用 strcmp 方法  消耗时间：...";
	OmnScreen << stop -start << endl;
	return true;
}

bool
AosWordIDMgrTester::normalTest()
{
	u64 cirTi;
	cirTi = 10000000;
	u64 j = 0;
	for(u64 i = 0; i < cirTi; i++)
	{
//		OmnScreen << "请再次选择测试内容：" << endl
//		OmnScreen << "1.产生word, 查询word id, 并记录到vector中" << endl;
//		OmnScreen << "2.（vector.size() >= 1）从vector中随机提取一个word,查询word id,看是否一致" << endl;
//		OmnScreen << "3.（vector.size() >= 2）从vector中随即提取两个word，比较两个word的大小,通过" << endl;
//		OmnScreen << "    word id mgr比较两个word的大小，对比结果是否一致" << endl;
//		OmnScreen << "4.word id mgr比较两个word的大小，对比结果是否一致" << endl;
//		OmnScreen << "5.sync测试（待定）" << endl;
//		OmnScreen << "5.sync测试（待定）" << endl;
//		
//		int select2;
//		cin >> select2;
	
		// 随机挑选操作类型
		//int select2 = 1;
		int select2 = rand()%1 + 1;
		bool rslt = false;
		
//		u64 * orig_id = new u64[100];
//		u64 * cur_id = new u64[100];
//		
//		vector<OmnString>   orig_str[100];
//		vector<u64>         orig_id[100];

//		u64 idx,idx1,idx2,wordid;
//		OmnString str1,str2;
		
		
		switch(select2)
		{
			case 1:
					rslt = addWordTest(j);
					aos_assert_r(rslt,false);
					break;
			case 2:
					rslt = getWordIDTest();
					aos_assert_r(rslt,false);
					break;
			case 3:
					rslt = getWordTest();
					aos_assert_r(rslt,false);
					break;

			case 4:
					rslt = compWordByIDTest();
					aos_assert_r(rslt,false);
					break;
			case 5:
					rslt = saveLoadTest();
					aos_assert_r(rslt,false);
					break;
			case 6:
					rslt = syncTest();
					aos_assert_r(rslt,false);
					break;
			default : break;
		}
		if(i%100000 == 99999)
		{
			OmnScreen << "测试" << i+1 <<"次， 状态正常" << endl;
		}
	}
	return true;
}

OmnString
AosWordIDMgrTester::random_str()
{
	OmnString str;
	str << rand();
	return str;
}



bool 
AosWordIDMgrTester::addWordTest(u64& j)
{

/*
	//产生word, 查询word id, 并记录到vector中
	//Func_5_1();
	orig_str[i] = ;
	orig_id[i] = getWordIDSafe(OrigStr[i]);
	break;
*/
	
		OmnString str=random_str();
		mWordArray1.push_back(str);
		OmnString str1 = mWordArray1[j];
		u64 word_id = mWordMgr->getWordIDSafe(str1);
		mIDArray1.push_back(word_id);
		j++;

	return true;
}

bool 
AosWordIDMgrTester::getWordIDTest()
{

	//（vector.size() >= 1）从vector中随机提取一个word,查询word id,看是否一致
	//Func_5_2();
	if (mWordArray1.size() == 0)
	{
	   return true;
	}
	u64 idx = rand()%mWordArray1.size();
	OmnString word = mWordArray1[idx];
	u64 wordid = mWordMgr->getWordIDSafe(word);
	if (wordid  != mIDArray1[idx])
	{
		aos_assert_r(0,false);
		OmnScreen << "=======================error=======================" << endl;
		OmnScreen << "===================================================" << endl;
		OmnScreen << "===================================================" << endl;
		return false;
	}
	

	return true;
}

bool 
AosWordIDMgrTester::getWordTest()
{
	//（vector.size() >= 1）从vector中随机提取一个word,查询word id,看是否一致
	//Func_5_2();
	if (mWordArray1.size() == 0)
	{
	   return true;
	}
	u64 size = mWordArray1.size();
	u64 idx = rand()%size;
	OmnString word = mWordArray1[idx];
	u64 wordid = mIDArray1[idx];
	OmnString real_word = mWordMgr->getWord(wordid);
	if (word != real_word)
	{
		aos_assert_r(0,false);
		OmnScreen << "=======================error=======================" << endl;
		OmnScreen << "===================================================" << endl;
		OmnScreen << "===================================================" << endl;
		return false;
	}
	

	return true;
/*
	//（vector.size() >= 1）从vector中随机提取一个word,查询word id,看是否一致
	//Func_5_2();
	if (orig_str.size() == 0)
	{
	   continue;
	}
	idx = rand(0,orig_str.size()-1);
	wordid = getWordIDSafe(orig_str[idx]);
	if (wordid  != orig_id[idx])
	{
		OmnAlarm << "error" << endl;
		OmnScreen << "=======================error=======================" << endl;
		OmnScreen << "===================================================" << endl;
		OmnScreen << "===================================================" << endl;
		return false;
	}
	
	OmnScreen << "正常结束!" << endl;

	break;
*/
}

bool 
AosWordIDMgrTester::compWordByIDTest()
{
	//（vector.size() >= 1）从vector中随机提取一个word,查询word id,看是否一致
	//Func_5_2();
	if (mWordArray1.size() == 0)
	{
	   return true;
	}
	u64 idx1 = rand()%mWordArray1.size();
	u64 idx2 = rand()%mWordArray1.size();
	OmnString word1 = mWordArray1[idx1];
	OmnString word2 = mWordArray1[idx2];

	u64 wordid1 = mIDArray1[idx1];
	u64 wordid2 = mIDArray1[idx2];
	int rslt = mWordMgr->compareWord(wordid1, wordid2);
	int rslt2 = strcmp(word1.data(),word2.data());
	if(rslt > 0)
	{
		aos_assert_r(rslt2 > 0, false);
	}
	if(rslt < 0)
	{
		aos_assert_r(rslt2 < 0, false);
	}
	if(rslt == 0)
	{
		aos_assert_r(rslt2 == 0, false);
	}
	return true;
/*
	//（vector.size() >= 2）从vector中随即提取两个word，比较两个word的大小
	// 通过word id mgr比较两个word的大小，对比结果是否一致
	// Func_5_3();
	if (orig_str.size() = 0)
	{
		continue;
	}
	
	if (orig_str.length() >= 2)
	{
		idx1 = rand(0,orig_str.size()-1);
		idx2 = rand(0,orig_str.size()-1);
		str1 = orig_str[idx1];
		str2 = orig_str[idx2];
		if (strcmp(str1,str2) != compareWord(str1,str2))
		{
			OmnAlarm << "error" << endl;

			return false;
		}
	}
*/
}

bool 
AosWordIDMgrTester::saveLoadTest()
{
/*
	// save word id mgr， load word id mgr
	// Func_5_4();
	save_WordIDMgr();
	load_WordIDMgr();
*/
	return true;
}

bool 
AosWordIDMgrTester::syncTest()
{
/*
	//sync测试（待定）
	//Func_5_5();
	break;
*/
	return true;
}




