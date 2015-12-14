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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Testers/DataGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataStructs/ShuffleStatId.h"
#include "Util/BuffArray.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosDataGen::AosDataGen()
{
}

AosDataGen::AosDataGen(
		const AosRundataPtr &rdata,
		const OmnString &control_objid)
:
mCrtDocid(1),
mRdata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	mBuff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
	createBuffArray();

	AosBuffPtr buff;
	AosXmlTagPtr buff_doc = AosRetrieveBinaryDoc(control_objid, buff, mRdata);
	aos_assert(buff_doc);
	if (!buff || buff->dataLen() <= 0)
	{
		buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		buff->setInt64(0);
	}

	mShuffle = OmnNew AosShuffleStatId(buff_doc, buff);	
	aos_assert(mShuffle);
}

AosDataGen::~AosDataGen()
{
}


u64 
AosDataGen::getEpochDay()
{
	int delta = rand() %1000;
	return eInitEpochDay + delta;
}


OmnString  
AosDataGen::getRandomStr()
{
	OmnString str = "";
	//int length = rand() %200 + 4;
	int length = rand() %10 + 4;
	for (int i=0; i<length; i++)
	{
		str << (char)(random()%26 + 'a');
	}

	return str;
}

bool
AosDataGen::run()
{
	int tries = rand() % 10000 +1;
	while(tries--)
	{
		pickRcd();
	}
	return true;
}

bool
AosDataGen::pickRcd()
{
	int r = rand() % 2;
	if (r)
	{
		return pickNewRcd();
	}
	return pickOldRcd();
}


bool
AosDataGen::pickNewRcd()
{
	Record rcd;
	rcd.field_key = getRandomStr();
	rcd.root_docid = mCrtDocid;
	// int pid = mShuffle->getPhysicalIdByStatId(mRdata, mCrtDocid);
	u64 binary_docid = mShuffle->getBinaryDocid(mCrtDocid);
	rcd.binary_docid = binary_docid;

	int r = rand() % 32 + 1;
	for (int i = 0; i < r; i++)
	{
		u64 v1 = getEpochDay();
		int64_t v2 = rand() %1000 + 1;
		rcd.setSlot(v1, v2);

		int64_t start_pos = 0;
		char data[eRecordLen];
		*(u64*)&data[start_pos] = mCrtDocid;

		start_pos += sizeof(u64);	
		*(u64*)&data[start_pos] = v1;

		start_pos += sizeof(u64);	
		*(u64*)&data[start_pos] = v2;

		start_pos += sizeof(u64);	
		strcpy(&data[start_pos], rcd.field_key.data());
		mBuff->setBuff(data, eRecordLen);
//OmnScreen << "AosDataGen; " << mCrtDocid << ";" << rcd.field_key << ";" << endl;
	}
	mRecord.push_back(rcd);
	//OmnScreen << "new_id :" << mCrtDocid << ";" << endl;
	mCrtDocid ++;
	return true;
}

bool
AosDataGen::pickOldRcd()
{
	if (mRecord.size() == 0) return true;

	int k = rand() % mRecord.size();
	//OmnScreen << "old_id :" << mRecord[k].root_docid << ";" << endl;
	int r = rand() % 32 + 1;
	for (int i = 0; i < r; i++)
	{
		u64 v1 = getEpochDay();
		int64_t v2 = rand() %1000 + 1;
		mRecord[k].setSlot(v1, v2);

		int64_t start_pos = 0;
		char data[eRecordLen];
		*(u64*)&data[start_pos] = mRecord[k].root_docid;

		start_pos += sizeof(u64);	
		*(u64*)&data[start_pos] = v1;

		start_pos += sizeof(u64);	
		*(u64*)&data[start_pos] = v2;

		start_pos += sizeof(u64);	
		strcpy(&data[start_pos], mRecord[k].field_key.data());
//OmnScreen << "AosDataGen; " << mRecord[k].root_docid << ";" << mRecord[k].field_key << ";" << endl;
		mBuff->setBuff(data, eRecordLen);
	}
	return true;
}


bool
AosDataGen::createBuffArray()
{
	// [docid, time, value, key]
	OmnString str;
	str << "<zky_buffarray  zky_stable = \"false\" >"
		//<< "<CompareFun cmpfun_type=\"custom\" cmpfun_size=\"" << eRecordLen << "\">"
		<< "<zky_cmparefunc cmpfun_type=\"custom\" cmpfun_size=\"" << eRecordLen << "\">"
		<<  "<cmp_fields>"
		<<      "<field cmp_size=\"8\" cmp_datatype=\"u64\" cmp_pos=\"0\" />"
		<<      "<field cmp_size=\"8\" cmp_datatype=\"u64\" cmp_pos=\"8\" />"
		<<  "</cmp_fields>"
		<<  "<aggregations>"
		<<      "<aggregation agr_pos=\"16\" agr_type=\"u64\" agr_fun=\"norm\"/>"
		<<  "</aggregations>"
		<< "</zky_cmparefunc>"
		<< "</zky_buffarray>";

	AosXmlTagPtr tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(tag, false);

	mArray = OmnNew AosBuffArray(tag, mRdata.getPtr());
	aos_assert_r(mArray, false);
	return true;
}


bool
AosDataGen::getData(
		const AosRundataPtr &rdata,
		AosBuffPtr &data_buff)
{
	/*
	map<int, u64> snap_ids;
	mShuffle->saveControlDoc(rdata, snap_ids);

	char *data = mBuff->data();
	int64_t data_len = mBuff->dataLen();
	aos_assert_r(data_len % eRecordLen == 0, false);

	int64_t off = 0;
	while (off < data_len)
	{
		mArray->addValue(&data[off], eRecordLen, rdata.getPtr());
		off += eRecordLen;
	}
	mArray->resetRecordLoop();
	mArray->sort();
	mArray->resetRecordLoop();

	data_buff = mArray->getBuff();
	mArray->clear();

	OmnScreen << "num_record:" << data_len / eRecordLen << ";" << endl;
	mBuff->resetMemory(1000);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

bool
AosDataGen::getRecord(
		vector<u64> &dist_block_docids,
		vector<u64> &stat_ids,
		map<u64, u32> &index,
		int64_t &s_time,
		int64_t &e_time)
{
	if (mRecord.size() == 0) return true;
	u32 a = rand() % mRecord.size();
	u32 b = rand() % mRecord.size(); 
	a = 0;
	b = mRecord.size() -1;
	if (a == b) return true;

	u32 t = 0;
	if (a > b) 
	{
		t = a;
		a = b;
		b = t;
	}
	for (u32 i = a; i <= b; i++)
	{
		index.insert(make_pair(mRecord[i].root_docid, i));
		dist_block_docids.push_back(mRecord[i].binary_docid);
		stat_ids.push_back(mRecord[i].root_docid);
	}

	s_time = eInitEpochDay;
	e_time = eInitEpochDay + 1000;
	return true;
}


bool
AosDataGen::isVaild(
		const u32 idx,
		const int64_t &time_slot,
		int64_t &vv)
{
	vv = mRecord[idx].getSlot(time_slot);
	if (!vv) return false;
	return true;
}

u64
AosDataGen::getSlotSize(const u32 idx)
{
	return mRecord[idx].getSlotSize();
}


void
AosDataGen::print(const u32 idx)
{
	 mRecord[idx].print();
}

