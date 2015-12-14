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
// 2012/07/30	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinCRI.h"

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


AosDataJoinCRI::AosDataJoinCRI()
:
AosDataJoin()
{
}


AosDataJoinCRI::AosDataJoinCRI(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata)
{
}


AosDataJoinCRI::~AosDataJoinCRI()
{
}


bool
AosDataJoinCRI::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		2, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("mnt"));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	u16 key_head2 = AosCounterUtil::composeTimeEntryType(
		3, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("mnt"));
	mKeyHead2.assign((char *)&key_head2, sizeof(u16));

	mSep = def->getAttrStr(AOSTAG_SEPARATOR);
	aos_assert_r(mSep != "", false);
	AosConvertAsciiBinary(mSep);

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

	OmnString calldur_iilname = def->getAttrStr("calldur_iilname");
	aos_assert_r(calldur_iilname != "", false);
	mCallDurMap = AosIILEntryMapMgr::retrieveIILEntryMap(calldur_iilname, rdata);
	aos_assert_r(mCallDurMap, false);

	return true;
}


bool
AosDataJoinCRI::run()
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
AosDataJoinCRI::getNextKey(
		u64 &month,
		OmnString &phonenum,
		OmnString &region,
		u64 &value,
		const AosRundataPtr &rdata)
{
	month = 0;
	phonenum = "";
	region = "";
	value = 0;

	bool has_more;
	OmnString k, kk;
	vector<OmnString> str;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(k, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 kk = AosCounterUtil::getAllTerm2(k);
			 aos_assert_r(kk !="", false);

			 str.clear();
			 AosCounterUtil::splitTerm2(kk, str);
			 aos_assert_r(str.size() == 3, false);

			 month = str[0].toU64();
			 phonenum = str[1];
			 region = str[2];
			 value = mIILScanner->getCrtValue();
			 return true;
		
		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if(!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 return true;

		default:
			 return false;
		}
	}
	return false;
}


bool
AosDataJoinCRI::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);
	
	OmnString phonenum, region;
	u64 month, value;
	
	while (1)
	{
		rslt = getNextKey(month, phonenum, region, value, rdata);
		aos_assert_r(rslt, false);

		if (month == 0 || phonenum == "" || region == "")
		{
			break;
		}

		if (value == 0)
		{
			continue;
		}

		if (mCrtMonth == 0 || mCrtPhoneNum == "")
		{
			mRegions.clear();
			mCrtMonth = month;
			mCrtPhoneNum = phonenum;
			mCrtValue = value;
			mRegions.push_back(region);
			continue;
		}

		if (mCrtMonth != month || mCrtPhoneNum != phonenum)
		{
			procCrtValues(rdata);

			mRegions.clear();
			mCrtMonth = month;
			mCrtPhoneNum = phonenum;
			mCrtValue = value;
			mRegions.push_back(region);
			continue;
		}
		
		if (value < mCrtValue)
		{
			continue;
		}
		else if (value == mCrtValue)
		{
			mRegions.push_back(region);
		}
		else 
		{
			mRegions.clear();
			mCrtValue = value;
			mRegions.push_back(region);
		}
	}

	procCrtValues(rdata);
	return true;
}


bool
AosDataJoinCRI::procCrtValues(const AosRundataPtr &rdata)
{
	aos_assert_r(mRegions.size() > 0, false);
	if (mRegions.size() == 1)
	{
		return addAsmEntry(mRegions[0], rdata);
	}

	OmnString str, vv;
	u64 value = 0;
	AosIILEntryMapItr itr;
	multimap<u64, OmnString> region_map;

	for (u32 i=0; i<mRegions.size(); i++)
	{
		str = mKeyHead2;
		str << mCrtMonth << mSep << mCrtPhoneNum << mSep << mRegions[i];

		value = 0;
		mCallDurMap->lock();
		itr = mCallDurMap->find(str);
		if (itr != mCallDurMap->end()) 
		{
			vv = itr->second;
			value = vv.toU64();
		}
		mCallDurMap->unlock();
		region_map.insert(make_pair(value, mRegions[i]));
	}

	multimap<u64, OmnString>::reverse_iterator map_itr = region_map.rbegin();
	OmnString region = map_itr->second;
	return addAsmEntry(region, rdata);
}	


bool
AosDataJoinCRI::addAsmEntry(const OmnString &region, const AosRundataPtr &rdata)
{
	aos_assert_r(region != "", false);
	AosValueRslt value_rslt;
	OmnString str = mKeyHead;
	str << mCrtMonth << mSep << mCrtPhoneNum;
	u64 r_id = region.toU64();
	value_rslt.setKeyValue(str.data(), str.length(), true, r_id);
	bool rslt = mIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinCRI::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinCRI::getProgress() 
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
AosDataJoinCRI::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAsm = 0;
}


AosDataJoinPtr
AosDataJoinCRI::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinCRI * join = OmnNew AosDataJoinCRI(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinCRI::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

