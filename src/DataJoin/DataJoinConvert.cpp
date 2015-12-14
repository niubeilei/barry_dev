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
// IIL joins are over-simplified 'table joins', where IILs are special 
// form of 'tables'. There are only two columns in IILs: Key and Value.
// Two IIls can be joined in a number of ways:
// 		eKey
// 		eValue
// 		eFamily
// 		eMember
// 		eSpecificMember
//
// This action supports only equi-joins. Further, this action assumes the
// matching column (either 'key' or 'value' column) is sorted. For instance, 
// if the matching column is the key-column, the normal IIL is used; if
// the matching column is the value-column, the companion IIL is used. 
//
// Modification History:
// 2014/01/18 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinConvert.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "UtilTime/TimeFormat.h"
#include "XmlUtil/XmlTag.h"


AosDataJoinConvert::AosDataJoinConvert()
:
AosDataJoin(),
mType(eInvalid)
{
}


AosDataJoinConvert::AosDataJoinConvert(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mType(eInvalid)
{
}


AosDataJoinConvert::~AosDataJoinConvert()
{
}


bool
AosDataJoinConvert::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	OmnString type = def->getAttrStr("convert_type");
	if (type == "ignore_last_digital_str")
	{
		mType = eIgnoreLastDigitalStr; 
	}
	else if (type == "unicom_net_status")
	{
		mType = eUnicomNetStatus;
	}
	else
	{
		OmnAlarm << "error" << enderr;
		return false;
	}

	if (mType == eIgnoreLastDigitalStr)
	{
		mConvertIdx = def->getAttrInt("convert_idx", -1);
		aos_assert_r(mConvertIdx >= 0, false);

		mConvertSep = def->getAttrStr("convert_sep");
		aos_assert_r(mConvertSep != "", false);
		AosConvertAsciiBinary(mConvertSep);

		mSep = def->getAttrStr(AOSTAG_SEPARATOR);
		aos_assert_r(mSep != "", false);
		AosConvertAsciiBinary(mSep);
	}
	else if (mType == eUnicomNetStatus)
	{
		mSep = def->getAttrStr(AOSTAG_SEPARATOR);
		aos_assert_r(mSep != "", false);
		AosConvertAsciiBinary(mSep);
	}

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	mIILAsm = mCtlr->getIILAssembler();
	aos_assert_r(mIILAsm, false);

	return true;
}


bool
AosDataJoinConvert::run()
{
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	bool rslt = runJoin(rdata);
	mLock->unlock();
	mFinished = true;
	mSuccess = rslt;
	
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, time:" << mEndTime
		<< ", success:" << mSuccess
		<< ", spend:" << (mEndTime - mStartTime) << endl;
	
	AosDataJoinPtr thisptr(this, true);
	mCtlr->joinFinished(thisptr, rdata);

	clear();
	return true;
}


bool
AosDataJoinConvert::getNextKey(
		OmnString &key,
		u64 &value,
		bool &finish,
		const AosRundataPtr &rdata)
{
	key = "";
	value = 0;
	finish = false;
	bool has_more = true;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 aos_assert_r(key != "", false);

			 value = mIILScanner->getCrtValue();
			 aos_assert_r(value != 0, false);

			 return true;
		
		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 finish = true;
			 return true;

		default:
			 return false;
		}
	}
	return false;
}


bool
AosDataJoinConvert::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	OmnString key;
	u64 value;
	bool finish = false;
	
	while (!finish)
	{
		rslt = getNextKey(key, value, finish, rdata);
		aos_assert_r(rslt, false);

		if (finish)
		{
			break;
		}
		
		switch (mType)
		{
		case eIgnoreLastDigitalStr :
			 rslt = procIgnoreLastDigitalStr(key, value, rdata);
			 if (rslt)
			 {
			 	OmnAlarm << "error" << enderr;
			 }
			 break;

		case eUnicomNetStatus :
			 rslt = procUnicomNetStatus(key, value, rdata);
			 if (rslt)
			 {
			 	OmnAlarm << "error" << enderr;
			 }
			 break;

		default:
			 OmnAlarm << "error" << enderr;
			 break;
		}
	}

	return true;
}


bool
AosDataJoinConvert::procIgnoreLastDigitalStr(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	OmnString k = AosCounterUtil::getAllTerm2(key);
	aos_assert_r(k !="", false);

	vector<OmnString> str;
	AosCounterUtil::splitTerm2(k, str);
	aos_assert_r((int)str.size() > mConvertIdx, false);

	OmnString source_str = str[mConvertIdx]; 
	OmnString rsltStr;
	int idx = source_str.findSubString(mConvertSep, -1, true);
	if (idx == -1)
	{
		rsltStr = source_str;
	}
	else if (idx == source_str.length() - 1)
	{
		rsltStr.assign(source_str.data(), idx);
	}
	else
	{
		int len = source_str.length() - idx - 1;
		OmnString digitalStr(&source_str.data()[idx + 1], len);
		if (digitalStr.isDigitStr())
		{
			rsltStr.assign(source_str.data(), idx);
		}
		else
		{
			rsltStr = source_str;
		}
	}
	
	str[mConvertIdx] = source_str;

	OmnString ss(key.data(), 2);
	ss << str[0];
	for (u32 i=1; i<str.size(); i++)
	{
		ss << mSep << str[i];	
	}

	AosValueRslt value_rslt;
	value_rslt.setKeyValue(ss.data(), ss.length(), true, value);
	bool rslt = mIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}
	

bool
AosDataJoinConvert::procUnicomNetStatus(
		const OmnString &key,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(key !="", false);

	vector<OmnString> str;
	bool finished;
	AosStrSplit::splitStrBySubstr(key.data(), mSep.data(), str, 100, finished);
	aos_assert_r(str.size() == 4, false);

	OmnString k;
	k << str[3] << mSep << str[0] << mSep << str[1] << mSep << str[2];

	AosValueRslt value_rslt;
	value_rslt.setKeyValue(k.data(), k.length(), true, value);
	bool rslt = mIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataJoinConvert::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinConvert::getProgress() 
{
	if(mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mIILScanner, 0);
	mProgress = mIILScanner->getProgress();
	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}


void
AosDataJoinConvert::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
}


AosDataJoinPtr
AosDataJoinConvert::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinConvert * join = OmnNew AosDataJoinConvert(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinConvert::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

