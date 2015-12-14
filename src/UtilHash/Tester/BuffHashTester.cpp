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
// 01/01/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilHash/Tester/BuffHashTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrmsgId.h"
#include "UtilHash/HashedObjBuff.h"
#include "UtilHash/HashedObj.h"
#include "UtilHash/StrObjHash.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/XmlTag.h"

static bool sgPrintFlag = false;
extern int gAosLogLevel;

AosBuffHashTester::AosBuffHashTester()
:
mLock(OmnNew OmnMutex()),
mIsGood(false)
{
	mIsGood = init();
	if(!mIsGood)
	{
		OmnAlarm << "Faild to Construct AosBuffHashTester" << enderr;
		exit(0);
	}
}


bool
AosBuffHashTester::init()
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
	AosHashedObjPtr dftObj = OmnNew AosHashedObjBuff();
	aos_assert_r(dftObj, false);
	AosErrmsgId::E  errorId;
	OmnString errmsg;
	mHashObj = OmnNew AosStrObjHash(dftObj, fullName, 
			cacheSize, keyBitmap, bucketSize, needReset, errorId, errmsg); 	
	aos_assert_r(mHashObj, false);
	return true;
}


bool
AosBuffHashTester::start()
{
	return basicTest();
}


bool
AosBuffHashTester::basicTest()
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
AosBuffHashTester::strHashTest()
{
	int randomValue = rand() % 100;
	if (randomValue < 30)
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
	// if (randomValue < 100)
	// {
	// 	aos_assert_r(appOpr(), false);
	// 	return true;
	// }

	return true;
}


AosBuffPtr
AosBuffHashTester::createNewBuff()
{
	// This tester will create buff with the following:
	// 		str	
	// 		u64		
	// 		int64
	// 		char
	// 		str
	// 		u32
	// 		u32
	OmnString 	v1 = OmnRandom::letterDigitDashUnderscore(1, 5, 80, 6, 15, 15, 16, 50, 5);
	u64 		v2 = rand();
	int64_t 	v3 = rand() - 0x7fffffff;
	char 		v4 = rand() % 50 + 32;
	OmnString 	v5 = OmnRandom::letterDigitDashUnderscore(1, 5, 80, 6, 15, 15);
	u32			v6 = rand();
	u32			v7 = rand();
	AosBuffPtr buff = OmnNew AosBuff(10, 10 AosMemoryCheckerArgs);
	buff->setOmnStr(v1);
	buff->setU64(v2);
	buff->setInt64(v3);
	buff->setChar(v4);
	buff->setOmnStr(v5);
	buff->setU32(v6);
	buff->setU32(v7);
	return buff;
}


OmnString
AosBuffHashTester::toString(const AosBuffPtr &buff) const
{
	OmnString ss;
	ss << "v1: " << buff->getStr("")
		<< "v2: " << buff->getU64(0)
		<< "v3: " << buff->getInt64(-1)
		<< "v4: " << buff->getChar(0)
		<< "v5: " << buff->getStr("")
		<< "v6: " << buff->getU32(0)
		<< "v7: " << buff->getU32(0);
	return ss;
}


bool
AosBuffHashTester::addData()
{
	// It randomly generates a pair [key, value], add it to the hash table. 
	// It then adds the pair to mEntries and mKeys. It may randomly determine
	// whether to add or to modify.
	OmnString key = OmnRandom::letterDigitDashUnderscore(eMinKeyLen, eMaxKeyLen);
	// static int lsNum = 0;
	// OmnString key = "keyddddddddddddd";
	// key << lsNum++;

	// Create the object. 
	AosBuffPtr buff = createNewBuff();
	aos_assert_r(key != "", false);
	AosHashedObjBuffPtr hashObjU64 = OmnNew AosHashedObjBuff(key, buff);
	u64 ss; if (sgPrintFlag) ss = OmnGetTimestamp();
	mHashObj->setPrintFlag(sgPrintFlag);

	bool rslt = mHashObj->addDataPublic(key, hashObjU64, true);
	aos_assert_r(rslt, false);

	aos_assert_r(mHashObj->getHead() == hashObjU64, false);
	map<OmnString, AosBuffPtr>::iterator itr = mEntries.find(key);
	if (itr == mEntries.end())
	{
		if (gAosLogLevel >= 2) OmnScreen << "add: " << key << " : " 
			<< toString(buff) << endl;
		mEntries[key] = buff;
		mKeys.push_back(key);
		mNumAdds++;
		return true;
	}

	if (gAosLogLevel >= 2) OmnScreen << "To add but found one: " << key 
		<< " : " << itr->second << ":" << toString(buff) << endl;
	mNumModifies++;
	return true;
}


bool
AosBuffHashTester::deleteData()
{
	u32 size = mKeys.size();
	if (size == 0) return true;
	
	u32 index = rand() % size;
	OmnString key = mKeys[index];
	aos_assert_r(key != "", false);
	bool rslt = mHashObj->erase(key);
	aos_assert_r(rslt, false);
	if (gAosLogLevel >= 2) OmnScreen << "delete: " << key << " : " << mEntries[key] <<  endl;	

	mKeys.erase(mKeys.begin() + index);
	mEntries.erase(key);
	mNumDeletes++;
	return true;
}


bool
AosBuffHashTester::modifyData()
{
	u32 size = mKeys.size();
	if(size == 0) return true;

	// Determine the key
	u32 index = rand() % size;
	OmnString key = mKeys[index]; 
	aos_assert_r(key != "", false);

	// Retrieve the object from the testing data
	AosBuffPtr old_buff = mEntries[key];
	aos_assert_r(old_buff, false);
	AosHashedObjBuffPtr testObj = OmnNew AosHashedObjBuff(key, old_buff);

	// Retrieve the data from the hash
	AosHashedObjPtr hashed_obj = mHashObj->find(key);
	aos_assert_r(hashed_obj, false);
	aos_assert_r(hashed_obj->isSame(testObj), false);

	AosBuffPtr new_value = createNewBuff();
	testObj->setValue(new_value);
	if (gAosLogLevel >= 2) OmnScreen << "modify: " 
		<< key << " : " << toString(new_value) << endl;
	bool rslt = mHashObj->modifyDataPublic(key, testObj);
	aos_assert_r(rslt, false);

	mEntries[key] = new_value;
	mNumModifies++;
	return true;
}


bool
AosBuffHashTester::getData()
{
	// It randomly retrieves one data. 
	u32 size = mKeys.size();
	if(size == 0) return true;

	OmnString key;
	AosBuffPtr value;
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
		map<OmnString, AosBuffPtr>::iterator itr = mEntries.find(key);
		if (itr == mEntries.end())
		{
			exist = false;
		}
		else
		{
			value = itr->second;
		}
	}
	if (gAosLogLevel >= 2) OmnScreen << "get: " 
		<< key << " : " << toString(value) << endl;
	AosHashedObjPtr dbObj = mHashObj->find(key);

	if (exist)
	{
		aos_assert_r(dbObj, false);
		aos_assert_r(dbObj->getType() == AosHashedObj::eBuff, false);
		aos_assert_r(mHashObj->getHead() == dbObj, false);
		AosHashedObjPtr new_obj = OmnNew AosHashedObjBuff(key, value);
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


/*
bool
AosBuffHashTester::appOpr()
{
	OmnString key = OmnRandom::letterDigitDashUnderscore(eMinKeyLen, eMaxKeyLen);
	aos_assert_r(key != "", false);
	u64 value = rand();
	AosHashedObjBuffPtr hashObjU64 = OmnNew AosHashedObjBuff(key, value);
	
	AosHashedObjPtr dbObj = mHashObj->find(key);
	if (!dbObj)
	{
		bool rslt = mHashObj->addDataPublic(key, hashObjU64, false);
		aos_assert_r(rslt, false);
		aos_assert_r(mHashObj->getHead() == hashObjU64, false);

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
	aos_assert_r(mHashObj->getHead() == dbObj, false);
	return true;
}
*/

