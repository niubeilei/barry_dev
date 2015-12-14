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
// 09/03/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/Tester/StrHashTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "UtilHash/HashedObjU64.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/StrObjHash.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/XmlTag.h"

static bool sgPrintFlag = false;
extern int gAosLogLevel;

AosStrHashTester::AosStrHashTester()
:
mLock(OmnNew OmnMutex()),
mIsGood(false)
{
	mIsGood = init();
	if(!mIsGood)
	{
		OmnAlarm << "Faild to Construct AosStrHashTester" << enderr;
		exit(0);
	}
}


bool
AosStrHashTester::init()
{
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);
	OmnString fileDir = config->getAttrStr("filedir", "");
	if (fileDir == "")
	{
		OmnAlarm << "Missing file dir " << enderr;
		return false;
	}
	OmnString fileName = config->getAttrStr("filename", "");
	if (fileName == "") 
	{
		OmnAlarm << "Missing file name" << enderr;
		return false;
	}
	OmnString fullName = fileDir;
	fullName << "/" << fileName;
	bool needReset = config->getAttrBool("needReset", false);
	int	cacheSize = config->getAttrInt("cachesize", 1000);
	int bucketSize = config->getAttrInt("bucketsize", 1000);
	int keyBitmap = config->getAttrInt("keybitmap", 20);
	AosHashedObjPtr dftObj = OmnNew AosHashedObjU64();
	aos_assert_r(dftObj, false);
	AosErrmsgId::E  errorId;
	OmnString errmsg;
	mStrHashObj = OmnNew AosStrObjHash(dftObj, fullName, 
			cacheSize, keyBitmap, bucketSize, needReset, errorId, errmsg); 	
	aos_assert_r(mStrHashObj, false);
	return true;
}


bool
AosStrHashTester::start()
{
	return basicTest();
}


bool
AosStrHashTester::basicTest()
{
	int tries = mTestMgr->getTries();
	OmnScreen << "Tries:" << tries << endl;
	if (tries <= 0) tries = eDefaultTries;

	u64 startTime = OmnGetTimestamp();

	mNumAdds = 0;
	mNumModifies = 0;
	mNumDeletes = 0;
	mNumReads = 0;
	for(int i = 1; i <= tries; i++)
	{
		aos_assert_r(strHashTest(), false);

		sgPrintFlag = ((i % 10000) == 0);
		if (sgPrintFlag)
		{
			OmnScreen << "Time (10000 times): " 
				<< OmnGetTimestamp() - startTime << ":" << i << " R:" << mNumReads
				   << ", A:" << mNumAdds << ", M:" << mNumModifies
				   << ", D:" << mNumDeletes << endl;
			startTime = OmnGetTimestamp();
		}
	}
	return true;
}


bool
AosStrHashTester::strHashTest()
{
	int randomValue = rand() % 100;
	// if (randomValue < 30)
	if (randomValue <= 100)
	{
		aos_assert_r(addData(), false);
		return true;
	}
	if (randomValue < 60)
	{
		aos_assert_r(getData(), false);
		return true;
	}
	if (randomValue < 80)
	{
		aos_assert_r(deleteData(), false);
		return true;
	}
	if (randomValue < 90)
	{
		aos_assert_r(modifyData(), false);
		return true;
	}
	if (randomValue < 100)
	{
		aos_assert_r(appOpr(), false);
		return true;
	}

	return true;
}


bool
AosStrHashTester::addData()
{
	// It randomly generates a pair [key, value], add it to the hash table. 
	// It then adds the pair to mEntries and mKeys. It may randomly determine
	// whether to add or to modify.
	// OmnString key = OmnRandom::letterDigitDashUnderscore(eMinKeyLen, eMaxKeyLen);
	static int lsNum = 0;
	OmnString key = "keyddddddddddddd";
	key << lsNum++;

	u64 new_value = rand();

	aos_assert_r(key != "", false);
	AosHashedObjU64Ptr hashObjU64 = OmnNew AosHashedObjU64(key, new_value);
	u64 ss; if (sgPrintFlag) ss = OmnGetTimestamp();
	mStrHashObj->setPrintFlag(sgPrintFlag);

	bool rslt = mStrHashObj->addDataPublic(key, hashObjU64, true);
	aos_assert_r(rslt, false);

	aos_assert_r(mStrHashObj->getHead() == hashObjU64, false);
	map<OmnString, u64>::iterator itr = mEntries.find(key);
	if (itr == mEntries.end())
	{
		if (gAosLogLevel >= 2) OmnScreen << "add: " << key << " : " << new_value << endl;
		mEntries[key] = new_value;
		mKeys.push_back(key);
		mNumAdds++;
		return true;
	}

	if (gAosLogLevel >= 2) OmnScreen << "To add but found one: " << key 
		<< " : " << itr->second << ":" << new_value << endl;
	mNumModifies++;
	return true;
}


bool
AosStrHashTester::deleteData()
{
	u32 size = mKeys.size();
	if (size == 0) return true;
	
	u32 index = rand() % size;
	OmnString key = mKeys[index];
	aos_assert_r(key != "", false);
	bool rslt = mStrHashObj->erase(key);
	aos_assert_r(rslt, false);
	if (gAosLogLevel >= 2) OmnScreen << "delete: " << key << " : " << mEntries[key] <<  endl;	

	mKeys.erase(mKeys.begin() + index);
	mEntries.erase(key);
	mNumDeletes++;
	return true;
}


bool
AosStrHashTester::modifyData()
{
	u32 size = mKeys.size();
	if(size == 0) return true;

	u32 index = rand() % size;
	OmnString key = mKeys[index]; 
	aos_assert_r(key != "", false);
	u64 value = mEntries[key];
	AosHashedObjU64Ptr testObj = OmnNew AosHashedObjU64(key, value);
	u64 new_value = rand();
	testObj->setValue(new_value);
	if (gAosLogLevel >= 2) OmnScreen << "modify: " << key << " : " << new_value << endl;
	bool rslt = mStrHashObj->modifyDataPublic(key, testObj);
	aos_assert_r(rslt, false);

	mEntries[key] = new_value;
	mNumModifies++;
	return true;
}


bool
AosStrHashTester::getData()
{
	// It randomly retrieves one data. 
	u32 size = mKeys.size();
	if(size == 0) return true;

	OmnString key;
	u64 value = 0;
	bool exist = true;
	if ((rand() % 100) < 90 && mKeys.size() > 0)
	{
		u32 index = rand() % size;
		key = mKeys[index]; 
		aos_assert_r(key != "", false);
		value = mEntries[key];
	}
	else
	{
		key = OmnRandom::letterDigitDashUnderscore(eMinKeyLen, eMaxKeyLen);
		map<OmnString, u64>::iterator itr = mEntries.find(key);
		if (itr == mEntries.end())
		{
			exist = false;
		}
		else
		{
			value = itr->second;
		}
	}
	if (gAosLogLevel >= 2) OmnScreen << "get: " << key << " : " << value << endl;
	AosHashedObjPtr dbObj = mStrHashObj->find(key);

	if (exist)
	{
		aos_assert_r(dbObj, false);
		aos_assert_r(dbObj->getType() == AosHashedObj::eU64, false);
		aos_assert_r(mStrHashObj->getHead() == dbObj, false);
		AosHashedObjPtr new_obj = OmnNew AosHashedObjU64(key, value);
		aos_assert_r(new_obj->isSame(dbObj), false);
		aos_assert_r(dbObj->isSame(new_obj), false);
	}
	else
	{
		aos_assert_r(!dbObj, false);
	}

	mNumReads++;
	return true;
}


bool
AosStrHashTester::appOpr()
{
	OmnString key = OmnRandom::letterDigitDashUnderscore(eMinKeyLen, eMaxKeyLen);
	aos_assert_r(key != "", false);
	u64 value = rand();
	AosHashedObjU64Ptr hashObjU64 = OmnNew AosHashedObjU64(key, value);
	
	AosHashedObjPtr dbObj = mStrHashObj->find(key);
	if (!dbObj)
	{
		bool rslt = mStrHashObj->addDataPublic(key, hashObjU64, false);
		aos_assert_r(rslt, false);
		aos_assert_r(mStrHashObj->getHead() == hashObjU64, false);

		map<OmnString, u64>::iterator itr = mEntries.find(key);
		if (itr != mEntries.end())
		{
			return true;
		}
		mEntries[key] = value;
		mKeys.push_back(key);
		return true;
	}
	aos_assert_r(dbObj->isKeySame(hashObjU64), false);
	aos_assert_r(mStrHashObj->getHead() == dbObj, false);
	return true;
}

