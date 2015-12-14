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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/Testers/RecordTester.h"

#include "API/AosApi.h"
#include "DataBlob/BlobRecord.h"
#include "DataBlob/Ptrs.h"
#include "DataRecord/Ptrs.h"
#include "DataRecord/DataRecord.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/StrU64Array.h"
#include "Util/CompareFun.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosRecordTester::AosRecordTester()
{
}


AosRecordTester::~AosRecordTester()
{
}


bool 
AosRecordTester::run(const AosRundataPtr &rdata)
{
	basicTest(rdata);
	return true;
}


bool 
AosRecordTester::basicTest(const AosRundataPtr &rdata)
{
	AosXmlTagPtr appconf = OmnApp::getAppConfig();
	aos_assert_r(appconf, false);
		
	AosXmlTagPtr conf = appconf->getFirstChild("testers");
	aos_assert_r(conf, false);
	int tries = conf->getAttrInt("tries", 0);
	for (int i=0; i<tries; i++)
	{
OmnScreen << "Try: " << i << endl;
		aos_assert_r(oneTry(conf, rdata), false);
	}
	return true;
}


bool
AosRecordTester::oneTry(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_r(conf, false);
	AosXmlTagPtr blob_conf = createConfig(conf);
	aos_assert_r(blob_conf, false);
	mBlob = OmnNew AosBlobRecord(blob_conf, rdata);
	aos_assert_r(mBlob, false);
	
	AosXmlTagPtr data_tag = conf->getFirstChild("datarecord");
	aos_assert_r(data_tag, false);
	mDataRecord = AosDataRecord::createRecord(data_tag, rdata);
	aos_assert_r(mDataRecord, false);

	aos_assert_r(mBlob->start(rdata), false);
	aos_assert_r(appendValue(rdata), false);
	aos_assert_r(checkValueInOrder(rdata), false);
	aos_assert_r(checkValueByIdx(rdata), false);
	aos_assert_r(rewriteRecordInOrder(rdata), false);
	aos_assert_r(rewriteRecordByIdx(rdata), false);
	aos_assert_r(checkValueInOrder(rdata), false);
	aos_assert_r(checkValueByIdx(rdata), false);
	aos_assert_r(checkFullInBackground(rdata), false);
	aos_assert_r(checkFullInMultiThreads(rdata), false);
	aos_assert_r(checkFullInCurrentThread(rdata), false);
	aos_assert_r(mBlob->finish(rdata), false);
	aos_assert_r(checkSerialize(rdata), false);
	return true;
}


bool
AosRecordTester::checkSerialize(const AosRundataPtr &rdata)
{
	aos_assert_r(mBlob, false);

	AosBuffPtr buff = OmnNew AosBuff(1024, 100 AosMemoryCheckerArgs);
	aos_assert_r(buff, false);
	aos_assert_r(mBlob->serializeTo(buff, rdata), false);

	buff->reset();	
	AosDataBlobPtr blob = mBlob->clone();
	aos_assert_r(blob, false);
	aos_assert_r(blob->serializeFrom(buff, rdata), false);

	aos_assert_r(blob->size() == mBlob->size(), false);
	
	AosDataRecordPtr record1 = mDataRecord->clone();
	aos_assert_r(record1, false);
	record1->reset(rdata);

	AosDataRecordPtr record2 = mDataRecord->clone();
	aos_assert_r(record2, false);
	record2->reset(rdata);
	
	mBlob->resetRecordLoop();	
	
	while (blob->nextRecord(record1) && mBlob->nextRecord(record2))
	{
		aos_assert_r(strcmp(record1->getData(), record2->getData()) == 0, false);
		aos_assert_r(record1->getDocid() == record2->getDocid(), false);
	}
	aos_assert_r(!blob->nextRecord(record1) && !mBlob->nextRecord(record2), false);
	aos_assert_r(blob->isSorted() == mBlob->isSorted(), false);
	aos_assert_r(blob->getRewriteLoopIdx() == mBlob->getRewriteLoopIdx(), false);
	return true;
}


bool
AosRecordTester::rewriteRecordByIdx(const AosRundataPtr &rdata)
{
	aos_assert_r(mBlob->resetRewriteLoop(), false);
	
	u64 count = 0;
	u64 num_tries  = rand() % mMaxRcds;
	for (u64 i=0; i<num_tries; i++)
	{
		u64 idx = rand() % mMaxRcds;
		aos_assert_r(initKeyValue(idx, rdata), false);
		if (++count%10000 == 0) 
		{
			//OmnScreen << "Modify by idx: " << num_tries << " : " << count << " : " << i << " : " << mDataRecord->getData() 
			//	<< " : " << mDataRecord->getDocid() << endl;
		}
		
		aos_assert_r(mBlob->rewriteRecordByIdx(idx, *mDataRecord.getPtr(), rdata), false);
	}

	return true;
}


bool
AosRecordTester::addMoreData(const AosRundataPtr &rdata)
{
	u64 count = 0;
	u64 num_more_entry = mMaxRcds * 10 + 1000;
	for (u64 i=0; i<num_more_entry; i++)
	{
		aos_assert_r(initDataRecord(rdata), false);
		if (++count%10000 == 0) 
		{
			//OmnScreen << "Append more: " << num_more_entry << " : " << count << " : " << mDataRecord->getData() 
			//	<< " : " << mDataRecord->getDocid() << endl;
		}
		aos_assert_r(mBlob->appendRecord(*mDataRecord.getPtr(), rdata), false);
	}
	return true;
}


bool
AosRecordTester::rewriteRecordInOrder(const AosRundataPtr &rdata)
{
	// This function will rewrite the record in buffarray located by idx,
	// and it will modidy the value in mKeys and mDocids.
	aos_assert_r(mBlob->resetRewriteLoop(), false);
	
	u64 count = 0;
	u64 idx = rand() % mMaxRcds;
	aos_assert_r(initKeyValue(idx, rdata), false);
	aos_assert_r(mBlob->setRewriteLoopIdx(idx), false);
	while (mBlob->rewriteNextRecord(*mDataRecord.getPtr(), false, rdata))
	{
		idx ++;
		if (++count%10000 == 0) 
		{
			//OmnScreen << "Modify in order: " << " : " << idx << " : " << mDataRecord->getData() 
			//	<< " : " << mDataRecord->getDocid() << endl;
		}
		aos_assert_r(initKeyValue(idx, rdata), false);
	}
	return true;
}


bool
AosRecordTester::initKeyValue(const u64 &idx, const AosRundataPtr &rdata)
{
	mDataRecord->reset(rdata);
	OmnString key = OmnRandom::letterStr(1, mRcdSize);
	aos_assert_r(key != "", false);
	mDataRecord->setData(key.getBuffer(), key.length(), true);
	aos_assert_r(splitKey(key, idx), false);
	
	u64 docid = rand();
	mDataRecord->setDocid(docid);
	mDocids[idx] = docid;
	return true;
}


bool
AosRecordTester::checkFullInBackground(const AosRundataPtr &rdata)
{
	aos_assert_r(mBlob->setInBackGroud(), false);
	aos_assert_r(addMoreData(rdata), false);
	return true;
}


bool
AosRecordTester::checkFullInMultiThreads(const AosRundataPtr &rdata)
{
	aos_assert_r(mBlob->setInMultiThreads(), false);
	aos_assert_r(addMoreData(rdata), false);
	return true;
}


bool
AosRecordTester::checkFullInCurrentThread(const AosRundataPtr &rdata)
{
	aos_assert_r(mBlob->setInCurrentThread(), false);
	aos_assert_r(addMoreData(rdata), false);
	return true;
	
}
	

bool
AosRecordTester::checkValueByIdx(const AosRundataPtr &rdata)
{
//OmnScreen << __FILE__ << " : " << __LINE__ << " : " << mBlob.getPtr() << endl;
	u64 idx = rand() % mMaxRcds;

	mBlob->resetRangeLoop();
	mDataRecord->reset(rdata);
	if (mBlob->firstRecordInRange(idx, mDataRecord))
	{
		aos_assert_r(strlen(mDataRecord->getData()) > 0 && mDataRecord->getDocid() > 0, false);
		aos_assert_r(strcmp(mDataRecord->getData(), mStrKeys[idx].data()) == 0, false);
		aos_assert_r(mDataRecord->getDocid() == mDocids[idx], false);
		mDataRecord->reset(rdata);
		idx ++;

		while (mBlob->nextRecordInRange(mDataRecord))
		{
			if (idx%10000 == 0) 
			{
				//OmnScreen << "Check: " << idx << " : " << mDataRecord->getData()
				//	<< " : " << mStrKeys[idx]
				//	<< " : " << mDataRecord->getDocid() 
				//	<< " : " << mDocids[idx] << endl;
			}
			aos_assert_r(strlen(mDataRecord->getData()) > 0 && mDataRecord->getDocid() > 0, false);
			aos_assert_r(strcmp(mDataRecord->getData(), mStrKeys[idx].data()) == 0, false);
			aos_assert_r(mDataRecord->getDocid() == mDocids[idx], false);
			idx ++;
			mDataRecord->reset(rdata);
		}
	}
	else
	{
		OmnAlarm << "Error:" << mBlob->size() << " : " << idx << enderr;
		return false;
	}
	return true;
}

bool
AosRecordTester::checkValueInOrder(const AosRundataPtr &rdata)
{
	int i=0;
	mBlob->resetRecordLoop();
	mDataRecord->reset(rdata);
	while (mBlob->nextRecord(mDataRecord))
	{
		if (i%10000 == 0) 
		{
			//OmnScreen << "Check: " << i << " : " << mDataRecord->getData()
			//	<< " : " << mStrKeys[i]
			//	<< " : " << mDataRecord->getDocid() 
			//	<< " : " << mDocids[i] << endl;
		}
		aos_assert_r(strlen(mDataRecord->getData()) > 0 && mDataRecord->getDocid() > 0, false);
		aos_assert_r(strcmp(mDataRecord->getData(), mStrKeys[i].data()) == 0, false);
		aos_assert_r(mDataRecord->getDocid() == mDocids[i], false);
		i++;
		mDataRecord->reset(rdata);
	}
	return true;
}


bool
AosRecordTester::appendValue(const AosRundataPtr &rdata)
{
	mU64Keys.clear();
	mStrKeys.clear();
	mDocids.clear();

	int count = 0;
	for (int i=0; i<mMaxRcds; i++)
	{
		aos_assert_r(initDataRecord(rdata), false);
		if (++count%10000 == 0) 
		{
			//OmnScreen << "Append: " << mMaxRcds << " : " << count << " : " << mDataRecord->getData() 
			//	<< " : " << mDataRecord->getDocid() << " : " << mBlob.getPtr() << endl;
		}
		aos_assert_r(mBlob->appendRecord(*mDataRecord.getPtr(), rdata), false);
	}

	return true;
}


bool
AosRecordTester::initDataRecord(const AosRundataPtr &rdata)
{
	mDataRecord->reset(rdata);
	aos_assert_r(initKey(), false); 
	aos_assert_r(initDocid(), false); 
	return true;
}


bool
AosRecordTester::initKey()
{
	return initStrU64();
	if (mType == U64U641) return initU64U64();
	if (mType == U64U642) return initU64U64();
	if (mType == StrU641) return initStrU64();
	if (mType == StrU642) return initStrU64();
	if (mType == Custom) return initCustom();
	return true;
}


bool
AosRecordTester::initU64U64()
{
	aos_assert_r(mWithDocid, false);
	mU64Keys.push_back(rand());
	return true;
}


bool
AosRecordTester::initCustom()
{
	OmnNotImplementedYet;
	return false;
}


bool
AosRecordTester::initStrU64()
{
	OmnString key = OmnRandom::letterStr(1, mRcdSize);
	aos_assert_r(key != "", false);
	mDataRecord->setData(key.getBuffer(), key.length(), true);
	aos_assert_r(splitKey(key, -1), false);
	return true;
}


bool
AosRecordTester::splitKey(const OmnString &key, const int64_t idx)
{

	int keylen = mRcdSize;
	if (mWithDocid) keylen -= sizeof(u64);
	if (mSetFieldNull) keylen -= 1;	
	if (mSetTnailNull) keylen -= 1;
	
	OmnString value;
	if (key.length() > keylen)
	{
		if (mTooLongPolicy == AOSDATATOOLONG_POLICY_TREAT_AS_ERROR) return true;
		if (mTooLongPolicy == AOSDATATOOLONG_POLICY_IGNORE_SILENTLY) value = "";
		if (mTooLongPolicy == AOSDATATOOLONG_POLICY_TRIMLEFT) value = key.subString(key.length()-keylen, keylen);
		if (mTooLongPolicy == AOSDATATOOLONG_POLICY_TRIMRIGHT) value = key.subString(0, keylen);
		if (mTooLongPolicy == AOSDATATOOLONG_POLICY_RESET_RECORD) value = "";
	}
	else
	{
		value = key;
	}
	
	if (idx < 0)
	{
		mStrKeys.push_back(value);
	}
	else
	{
		mStrKeys[idx] = value;
	}
	return true;
}


bool
AosRecordTester::initDocid()
{
	u64 docid = rand() + 1;
	mDataRecord->setDocid(docid);
	mDocids.push_back(docid);
	return true;
}


AosXmlTagPtr 
AosRecordTester::createConfig(const AosXmlTagPtr &conf)
{
	OmnString docstr = "<blob_config ";
    docstr << "type=\"record\" "
		<< AOSTAG_PAGESIZE << "=\"" << getPageSize() << "\" "
		<< AOSTAG_RUN_INSHELL << "=\"" << conf->getAttrStr("run_in_shell", "true") << "\" "
		<< AOSTAG_RFACTS_IN_BGD << "=\"false\" "
		<< AOSTAG_SORT_FULL << "=\"true\" "
		<< AOSTAG_RUNFINISH_ACTIONS_INTHRDS << "=\"true\" "
    	<< "zky_maxrcds=\"" << getMaxRcds(conf) << "\" " 
    	<< "zky_max_size=\"" << getMaxSize(conf) << "\" "
    	<< "zky_isstable=\"true\" "
    	<< "zky_run_finish_acts_threads=\"true\" >"
    	<< 		"<full_actions_before_sort>"
        <<			"<action type=\"test\" />"
    	<<		"</full_actions_before_sort>"
    	<< 		"<full_actions_after_sort>"
        <<			"<action type=\"test\" />"
    	<<		"</full_actions_after_sort>"
    	<<			"<append_actions>"
        <<		"<action type=\"test\" />"
    	<<		"</append_actions>"
    	<< 		"<start_actions>"
        <<			"<action type=\"test\" />"
    	<<		"</start_actions>"
    	<< 		"<finish_actions_before_sort>"
        <<			"<action type=\"test\" />"
    	<<		"</finish_actions_before_sort>"
    	<< 		"<finish_actions_after_sort>"
        <<			"<action type=\"test\" />"
    	<<		"</finish_actions_after_sort>"
    	<<		"<handler type=\"actions\"/>"
		<<		"<record_def type=\"fixbin\" />"
    	<<		"<zky_buffarray "
        <<			"zky_stable=\"true\" "
        <<			"zky_withdocid=\"" << withDocid() << "\" "
        <<			"zky_datatoolongplc=\"" << getTooLongPolicy() << "\" "
        <<			"zky_datatooshortplc=\"" << getTooShortPolicy() << "\" "
        <<			"zky_nullpolicy=\"" << getNullPolicy() << "\" "
        <<			"zky_frontpadding=\"0\" "
        <<			"zky_backpadding=\"0\" "
        <<			"zky_tooshortpad=\"0\" "
        <<			"zky_toolongpad=\"0\" "
		<<			"zky_setfieldnull=\"" << setFieldNull() << "\" "
        <<			"zky_settrailnull=\"" << setTnailNull() << "\">"
        << 			"<zky_cmparefunc "
        <<			    "cmpfun_type=\"" << getType() << "\" "
        <<			    "cmpfun_size=\"" << getRcdSize() << "\" " 
        <<			    "cmpfun_alphabetic=\"true\"/>"
    	<<		"</zky_buffarray>"
		<< "</blob_config>";
	
	AosXmlTagPtr tag = AosXmlParser::parse(docstr, AosMemoryCheckerArgsBegin);
	return tag;
}


OmnString
AosRecordTester::getType()
{
	return StrU641;
	if (rand()%100 <= 20) mType = U64U641;
	if (rand()%100 <= 40) mType = U64U642;
	if (rand()%100 <= 60) mType = StrU641;
	if (rand()%100 <= 80) mType = StrU642;
	if (rand()%100 <= 100) mType = Custom;
	return mType;
}


int 
AosRecordTester::getPageSize()
{
	return rand()%mMaxRcds;
}


int 
AosRecordTester::getRcdSize()
{
	mRcdSize = rand()%30 + sizeof(u64) + 3;
	return mRcdSize;
}


int
AosRecordTester::getMaxSize(const AosXmlTagPtr &conf)
{
	mMaxSize = conf->getAttrU64("zky_maxsize", 0);
	return mMaxSize;
}


int
AosRecordTester::getMaxRcds(const AosXmlTagPtr &conf)
{
	mMaxRcds = conf->getAttrU64("zky_maxrcds", 0);
	mMoreRcds = conf->getAttrU64("zky_morercds", 0);
	return mMaxRcds;
}


OmnString
AosRecordTester::setTnailNull()
{
	mSetTnailNull = true;
	return "true";
	
	if (rand()%100 <= 50) 
	{
		mSetTnailNull = true;
		return "true";
	}
	mSetTnailNull = true;
	return "false";
}


OmnString
AosRecordTester::setFieldNull()
{
	mSetFieldNull = true;
	return "true";
	if (rand()%100 <= 50) 
	{
		mSetFieldNull = true;
		return "true";
	}
	mSetFieldNull = false;
	return "false";
}


OmnString
AosRecordTester::withDocid()
{
	mWithDocid = true;
	return "true";

	if (rand()%100 <= 50) 
	{
		mWithDocid = true;
		return "true";
	}
	mWithDocid = false;
	return "false";
}


OmnString
AosRecordTester::getNullPolicy()
{
	int idx = rand()%100;
	if (idx <= 30) mNullPolicy = AOSDATANULL_POLICY_TREAT_AS_ERROR;
	if (idx <= 60) mNullPolicy = AOSDATANULL_POLICY_IGNORE_SILENTLY;
	if (idx <= 100) mNullPolicy = AOSDATANULL_POLICY_RESET;
	if (mNullPolicy == "")
	{
		OmnAlarm << "Undefined policy" << enderr;
		return "";
	}
	return mNullPolicy;
}


OmnString
AosRecordTester::getTooLongPolicy()
{
	mTooLongPolicy = AOSDATATOOLONG_POLICY_TRIMRIGHT;
	return mTooLongPolicy;
	int idx = rand()%100;
	if (idx <= 20) mTooLongPolicy = AOSDATATOOLONG_POLICY_TREAT_AS_ERROR;
	if (idx <= 40) mTooLongPolicy = AOSDATATOOLONG_POLICY_IGNORE_SILENTLY;
	if (idx <= 60) mTooLongPolicy = AOSDATATOOLONG_POLICY_TRIMLEFT;
	if (idx <= 80) mTooLongPolicy = AOSDATATOOLONG_POLICY_TRIMRIGHT;
	if (idx <= 100) mTooLongPolicy =  AOSDATATOOLONG_POLICY_RESET_RECORD;
	if (mTooLongPolicy == "")
	{
		OmnAlarm << "Undefined policy" << enderr;
		return "";
	}
	return mTooShortPolicy;
}


OmnString
AosRecordTester::getTooShortPolicy()
{
	int idx = rand()%100;
	if (idx <= 20) mTooShortPolicy = AOSDATATOOLONG_POLICY_TREAT_AS_ERROR;
	if (idx <= 40) mTooShortPolicy = AOSDATATOOLONG_POLICY_IGNORE_SILENTLY;
	if (idx <= 60) mTooShortPolicy = AOSDATATOOLONG_POLICY_TRIMLEFT;
	if (idx <= 80) mTooShortPolicy = AOSDATATOOLONG_POLICY_TRIMRIGHT;
	if (idx <= 100) mTooShortPolicy =  AOSDATATOOLONG_POLICY_RESET_RECORD;
	if (mTooShortPolicy == "")
	{
		OmnAlarm << "Undefined policy" << enderr;
		return "";
	}
	return mTooShortPolicy;
}

