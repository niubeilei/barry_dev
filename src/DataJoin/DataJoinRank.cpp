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
#include "DataJoin/DataJoinRank.h"

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


AosDataJoinRank::AosDataJoinRank()
:
AosDataJoin(),
mCrtMonth(0)
{
}


AosDataJoinRank::AosDataJoinRank(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata),
mCrtMonth(0)
{
}


AosDataJoinRank::~AosDataJoinRank()
{
}


bool
AosDataJoinRank::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// The config is in the form:
	// 	<sdoc 
	// 		<AOSTAG_IIL_SCANNERS>
	// 			<AOSTAG_IILSCANNER .../>
	// 		</AOSTAG_IIL_SCANNERS>
	// 		<iilassembler .../>
	// 	</sdoc>
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	u16 key_head = AosCounterUtil::composeTimeEntryType(
		4, 1, AosStatType::toEnum("sum"), AosTimeGran::toEnum("mnt"));
	mKeyHead.assign((char *)&key_head, sizeof(u16));

	u16 month_head = AosCounterUtil::composeTimeEntryType(
		3, 0, AosStatType::toEnum("sum"), AosTimeGran::toEnum("mnt"));
	mMonthHead.assign((char *)&month_head, sizeof(u16));

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

	mIILAssembler = mCtlr->getIILAssembler();
	aos_assert_r(mIILAssembler, false);

	return true;
}
	

bool
AosDataJoinRank::run()
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
AosDataJoinRank::getNextKey(
		u64 &month,
		OmnString &id,
		int64_t &value,
		const AosRundataPtr &rdata)
{
	month = 0;
	id = "";
	value = 0;
	bool has_more;
	OmnString key;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(key, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 {
				 OmnString k = AosCounterUtil::getAllTerm2(key);
				 aos_assert_r(k !="", false);
				 vector<OmnString> str;
				 AosCounterUtil::splitTerm2(k, str);
				 aos_assert_r(str.size() >= 2, false);

				 month = str[0].toU64();
				 id = str[1];
			 	 value = mIILScanner->getCrtValue();
			 }
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
AosDataJoinRank::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);
	
	mValueMap.clear();

	OmnString id;
	u64 month;
	int64_t value = 0;
	while (1)
	{
		rslt = getNextKey(month, id, value, rdata);
		aos_assert_r(rslt, false);

		if (id == "")
		{
			break;
		}

		if (month != mCrtMonth)
		{
			rslt = appendAllEntry(rdata);
			aos_assert_r(rslt, false);

			mCrtMonth = month;
		}

		mValueMap.insert(make_pair(value, id));
	}

	rslt = appendAllEntry(rdata);
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosDataJoinRank::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinRank::getProgress() 
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


bool
AosDataJoinRank::appendAllEntry(const AosRundataPtr &rdata)
{
	bool rslt;
	OmnString str, id, rank_str;
	u64 total = mValueMap.size();
	int64_t value = 0;
	u64 rank = 1;
	multimap<int64_t, OmnString>::reverse_iterator itr = mValueMap.rbegin();
	while (itr != mValueMap.rend())
	{
		value = itr->first;
		id = itr->second;
	
		str = "";
		str << mKeyHead << id << mSep << mCrtMonth << mSep << rank << mSep << total;

		AosValueRslt value_rslt;
		value_rslt.setKeyValue(str.data(), str.length(), true, (u64)value);
		rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
		aos_assert_r(rslt, false);

		if (rank <= 100)
		{
			if (rank < 10) {
				rank_str = "00";
			} else if (rank < 100) {
				rank_str = "0";
			} else {
				rank_str = "";
			}
			rank_str << rank;

			str = "";
			str << mMonthHead << mCrtMonth << mSep << rank_str << mSep << id;

			AosValueRslt value_rslt;
			value_rslt.setKeyValue(str.data(), str.length(), true, (u64)value);
			rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
		}

		rank++;
		itr++;
	}

	mValueMap.clear();

	return true;
}


void
AosDataJoinRank::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
	mIILAssembler = 0;
}


AosDataJoinPtr
AosDataJoinRank::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinRank * join = OmnNew AosDataJoinRank(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinRank::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

