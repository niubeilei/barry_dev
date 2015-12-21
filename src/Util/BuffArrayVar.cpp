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
// This is a buff array that allows buffs to be variable length.
// It uses two memory:
// 		mHeadBuff:	an int array
// 		mBodyBuff:		hold the contents
//
// Modification History:
// 2014/09/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Util/BuffArrayVar.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/CompareFun.h"
#include "Util/Sort.h"


AosBuffArrayVar::AosBuffArrayVar(const bool stable)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mHeadBuff(OmnNew AosBuff(eInitHeadSize AosMemoryCheckerArgs)),
mHeadBuffRaw(mHeadBuff.getPtr()),
mBodyBuff(OmnNew AosBuff(eInitBodySize AosMemoryCheckerArgs)),
mBodyBuffRaw(mBodyBuff.getPtr()),
mBodyAddr((i64)mBodyBuffRaw->data()),
mStable(stable),
mIsSorted(true),
mNumRcds(0),
mWasteSize(0),
mHeadSize(12)
{
}


AosBuffArrayVar::AosBuffArrayVar(
		const AosBuffPtr &buff,
		const AosCompareFunPtr &comp, 
		const bool stable)
:
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr()),
mHeadBuff(OmnNew AosBuff(eInitHeadSize AosMemoryCheckerArgs)),
mHeadBuffRaw(mHeadBuff.getPtr()),
mBodyBuff(OmnNew AosBuff(eInitBodySize AosMemoryCheckerArgs)),
mBodyBuffRaw(mBodyBuff.getPtr()),
mBodyAddr((i64)mBodyBuffRaw->data()),
mStable(stable),
mIsSorted(true),
mNumRcds(0),
mWasteSize(0),
mRemainSize(0),
mHeadSize(12)
{
	mComp = comp;
	mCompRaw = dynamic_cast<AosFunCustom*>(comp.getPtr());
	setBodyBuff(buff, mRemainSize, true);
}


AosBuffArrayVar::~AosBuffArrayVar()
{
}


AosBuffArrayVarPtr
AosBuffArrayVar::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosBuffArrayVar *array = OmnNew AosBuffArrayVar(true);
	bool rslt = array->config(def, rdata);
	aos_assert_r(rslt, 0);
	return array;
}

	
AosBuffArrayVarPtr
AosBuffArrayVar::create(
		const AosCompareFunPtr &comp,
		const bool stable,
		const AosRundataPtr &rdata)
{
	aos_assert_r(comp->getFuncType() == AosCompareFun::eCustom, 0);

	AosBuffArrayVar *array = OmnNew AosBuffArrayVar(stable);
	array->mComp = comp;
	array->mCompRaw = dynamic_cast<AosFunCustom*>(comp.getPtr());
	
	return array;
}


AosBuffArrayVarPtr
AosBuffArrayVar::create(
		const AosXmlTagPtr &cmp_tag,
		const bool stable,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cmp_tag, 0);

	AosCompareFunPtr comp = AosCompareFun::getCompareFunc(cmp_tag);
	if (!comp)
	{
		AosSetErrorU(rdata, "failed_creating_comp_func") << ": " << cmp_tag->toString() << enderr;
		return 0;
	}
	aos_assert_r(comp->getFuncType() == AosCompareFun::eCustom, 0);

	return create(comp, stable, rdata);
}


bool
AosBuffArrayVar::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mStable = def->getAttrBool(AOSTAG_STABLE, true);

	AosXmlTagPtr tag = def->getFirstChild("CompareFun");
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_compare_func") << ": " << def->toString() << enderr;
		return false;
	}

	mComp = AosCompareFun::getCompareFunc(tag);
	if (!mComp)
	{
		AosSetErrorU(rdata, "failed_creating_comp_func") << ": " << def->toString() << enderr;
		return false;
	}
	aos_assert_r(mComp->getRecordFieldsNum()>0, false);
	mHeadSize = sizeof(int) + sizeof(i64) + mComp->getRecordFieldsNum()*sizeof(u16);
	mComp->setSize(mHeadSize);
//	aos_assert_r(mComp->getFuncType() == AosCompareFun::eCustom || mComp->getFuncType() == AosCompareFun::eCustom, false);
	if(mComp->getFuncType() == AosCompareFun::eCustom)
	{
		mCompRaw = dynamic_cast<AosFunCustom*>(mComp.getPtr());
	}
	aos_assert_r(mCompRaw, false);
	return true;
}


bool 
AosBuffArrayVar::reset()
{
	mLockRaw->lock();
	mIsSorted = true;
	mNumRcds = 0;
	mWasteSize = 0;

	mHeadBuff = OmnNew AosBuff(eInitHeadSize AosMemoryCheckerArgs);
	mHeadBuffRaw = mHeadBuff.getPtr();

	mBodyBuff = OmnNew AosBuff(eInitBodySize AosMemoryCheckerArgs);
	mBodyBuffRaw = mBodyBuff.getPtr();
	mBodyAddr = (i64)mBodyBuffRaw->data();

	mLockRaw->unlock();
	return true;
}


/*
bool
AosBuffArrayVar::appendEntry(
		const AosValueRslt &value, 
		AosRundata *rdata)
{
	int len;
	const char *data = value.getCharStr(len);

	//ken 2012/08/02
	if (data[0] == 0)
	{
		OmnAlarm << "the entry is empty" << enderr;
	}

	return appendEntry(data, len, rdata);
}
*/


bool
AosBuffArrayVar::appendEntry(
		AosDataRecordObj *record, 
		AosRundata *rdata)
{
	//header buff format: 
	//1. rcd_offset setInt()
	//2. body_addr	 setI64()
	//3. field0_offset setU16()
	//4. field1_offset setU16()
	//...
	const char *data = record->getData(rdata);
//OmnScreen << "andy@@@@::" << data+4 << endl;
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

	bool rslt = true;
	rslt = appendEntry(data, rcd_len, rdata);
	aos_assert_r(rslt, false);

	//jackie 2015/01/24 append field offset, only one record
	int num_fields = record->getNumFields();
	for(int i=0; i<num_fields; i++)
	{
		AosDataFieldObj* fieldobj = record->getFieldByIdx1(i);
		int field_offset = fieldobj->mFieldInfo.field_offset;
		aos_assert_r(field_offset<65536, rdata);
		mHeadBuffRaw->gotoEnd();
		mHeadBuffRaw->setU16(field_offset);
	}
	return true;
}


bool
AosBuffArrayVar::appendEntry(
		const char *data, 
		const int len,
		AosRundata *rdata)
{
	aos_assert_r(data && len >= 0, false);

	mLockRaw->lock();
	if (mBodyBuffRaw->dataLen() + len + sizeof(int) >= eMaxBuffSize)
	{
		mLockRaw->unlock();
		return false;
	}

	mBodyBuffRaw->gotoEnd();
	int offset = mBodyBuffRaw->getCrtIdx();

	//Jozhi for RecordBuff length encode
	mBodyBuffRaw->setEncodeCharStr(data, len);
	//mBodyBuffRaw->setCharStr(data, len);
	if (mBodyAddr != (i64)mBodyBuffRaw->data())
	{
		bool rslt = rebuildBodyAddr();
		aos_assert_rl(rslt, mLockRaw, false);
	}

	mHeadBuffRaw->gotoEnd();
	mHeadBuffRaw->setInt(offset);
	mHeadBuffRaw->setI64(mBodyAddr);
	if (mNumRcds > 0) mIsSorted = false;
	mNumRcds++;	
	mLockRaw->unlock();

	return true;
}


bool 
AosBuffArrayVar::insertEntry(
		const char *data,
		const int len,
		AosRundata *rdata)
{
	aos_assert_r(data && len >= 0, false);

	mLockRaw->lock();
	aos_assert_rl(mIsSorted, mLockRaw, false);
	if (mBodyBuffRaw->dataLen() + len + sizeof(int) >= eMaxBuffSize)
	{
		mLockRaw->unlock();
		return false;
	}

	bool unique;
	i64 idx = findPosPriv(0, data, len, unique);
	aos_assert_rl(idx >= 0 && idx <= mNumRcds, mLockRaw, false);

	bool rslt = insertEntryPriv(idx, data, len, rdata);
	aos_assert_rl(rslt, mLockRaw, false);

	mLockRaw->unlock();
	return true;
}


bool 
AosBuffArrayVar::insertEntry(
		const i64 &idx,
		const char *data,
		const int len,
		AosRundata *rdata)
{
	aos_assert_r(data && len >= 0, false);

	mLockRaw->lock();
	aos_assert_rl(mIsSorted, mLockRaw, false);
	aos_assert_rl(idx >= 0 && idx <= mNumRcds, mLockRaw, false);
	if (mBodyBuffRaw->dataLen() + len + sizeof(int) >= eMaxBuffSize)
	{
		mLockRaw->unlock();
		return false;
	}

	bool rslt = insertEntryPriv(idx, data, len, rdata);
	aos_assert_rl(rslt, mLockRaw, false);

	mLockRaw->unlock();
	return true;
}


bool 
AosBuffArrayVar::insertEntryPriv(
		const i64 &idx,
		const char *data,
		const int len,
		AosRundata *rdata)
{
	aos_assert_r(data && len >= 0, false);

	if (mHeadBuffRaw->remainingMemSize() <= mHeadSize)
	{
		i64 expect_num = calcHeadNum(mNumRcds);
		bool rslt = mHeadBuffRaw->resize(expect_num * mHeadSize);
		aos_assert_r(rslt, false);
	}

	mBodyBuffRaw->gotoEnd();
	int offset = mBodyBuffRaw->getCrtIdx();
	mBodyBuffRaw->setCharStr(data, len);
	if (mBodyAddr != (i64)mBodyBuffRaw->data())
	{
		bool rslt = rebuildBodyAddr();
		aos_assert_rl(rslt, mLockRaw, false);
	}

	char *offsets = mHeadBuffRaw->data();
	if (idx < mNumRcds)
	{
		memmove(&offsets[(idx + 1) * mHeadSize], &offsets[idx * mHeadSize], (mNumRcds - idx) * mHeadSize);
	}
	mHeadBuffRaw->setCrtIdx(idx * mHeadSize);
	mHeadBuffRaw->setInt(offset);
	mHeadBuffRaw->setI64(mBodyAddr);

	mNumRcds++;
	mHeadBuffRaw->setDataLen(mNumRcds * mHeadSize);

	return true;
}


i64
AosBuffArrayVar::calcHeadNum(const i64 &crt_num)
{
	i64 expect_num = crt_num * 1.2;
	expect_num = (expect_num / 1000 + 1) * 1000;
	if (expect_num - crt_num < 1000) expect_num += 1000;
	return expect_num;
}


i64
AosBuffArrayVar::findPos(
		const i64 &start_idx,
		const char *data,
		const int len,
		bool &unique)
{
	mLockRaw->lock();
	i64 idx = findPosPriv(start_idx, data, len, unique);
	mLockRaw->unlock();
	return idx;
}


i64
AosBuffArrayVar::findPosPriv(
		const i64 &start_idx,
		const char *data,
		const int len,
		bool &unique)
{
	aos_assert_r(start_idx <= mNumRcds, -1);
	aos_assert_r(mCompRaw, -1);

	int rsltInt = 0;
	i64 left = start_idx;
	i64 right = mNumRcds - 1;
	i64 idx = 0;
	unique = true;

	char * offsets = mHeadBuffRaw->data();
	char * body = mBodyBuffRaw->data();
	int crt_offset;
	char * crt_data;
//	int crt_len;

	while (left <= right)
	{
		idx = (left + right) >> 1;

		crt_offset = *(int*)&offsets[idx * mHeadSize];
//		crt_len = *(int*)&body[crt_offset];
		crt_data = &body[crt_offset + sizeof(int)];
		rsltInt = mCompRaw->cmp(data, crt_data);
		if (rsltInt > 0) 
		{
			left = idx + 1;
		}
		else if (rsltInt == 0)
		{
			left = idx + 1;
			unique = false;
		}
		else
		{
			right = idx - 1;
		}
	}

	return left;
}


bool
AosBuffArrayVar::getEntry(
		const i64 &idx,
		char * &data,
		int &len)
{
	data = NULL;
	len = 0;

	mLockRaw->lock();
	if (idx >= mNumRcds)
	{
		mLockRaw->unlock();
		return false;
	}

	int offset = mHeadBuffRaw->getInt(idx * mHeadSize, -1);
	aos_assert_rl(offset >= 0, mLockRaw, false);

	data = mBodyBuffRaw->getCharStr(offset, len);
	aos_assert_rl(data != NULL, mLockRaw, false);

	mLockRaw->unlock();
	return true;
}


bool 
AosBuffArrayVar::modifyEntry(
		const i64 &idx, 
		const char *data,
		const int len,
		AosRundata *rdata)
{
	mLockRaw->lock();
	if (idx >= mNumRcds)
	{
		mLockRaw->unlock();
		return false;
	}

	if (mBodyBuffRaw->dataLen() + sizeof(int) + len >= eMaxBuffSize)
	{
		mLockRaw->unlock();
		return false;
	}

	mBodyBuffRaw->gotoEnd();
	int offset = mHeadBuffRaw->getInt(idx * mHeadSize, -1);
	aos_assert_rl(offset >= 0, mLockRaw, false);

	int old_len = 0;
	char *old_data = mBodyBuffRaw->getCharStr(offset, old_len);
	aos_assert_rl(old_data != NULL, mLockRaw, false);

	if (len <= old_len)
	{
		mBodyBuffRaw->setCrtIdx(offset);
		mBodyBuffRaw->setCharStr(data, len);

		mWasteSize += (old_len - len);
	}
	else
	{
		mBodyBuffRaw->gotoEnd();
		int new_offset = mBodyBuffRaw->getCrtIdx();
		mBodyBuffRaw->setCharStr(data, len);
		if (mBodyAddr != (i64)mBodyBuffRaw->data())
		{
			bool rslt = rebuildBodyAddr();
			aos_assert_rl(rslt, mLockRaw, false);
		}

		mHeadBuffRaw->setCrtIdx(idx * mHeadSize);
		mHeadBuffRaw->setInt(new_offset);
		mHeadBuffRaw->setI64(mBodyAddr);

		mWasteSize += (old_len + sizeof(int));
	}

	if (mWasteSize >= eMaxWasteSize)
	{
		bool rslt = rebuildData();
		aos_assert_rl(rslt, mLockRaw, false);
	}

	if (mNumRcds > 1) mIsSorted = false;

	mLockRaw->unlock();
	return true;
}


bool
AosBuffArrayVar::sort()
{
	mLockRaw->lock();
	if (mIsSorted || mNumRcds <= 1)
	{
		mLockRaw->unlock();
		return true;
	}
	aos_assert_rl(mCompRaw, mLockRaw, false);

	//i64 t1 = OmnGetTimestamp();

	char *offsets = mHeadBuffRaw->data();
	char *first = offsets;
	//char *last = &offsets[mNumRcds * mHeadSize];
	char *last = &offsets[mNumRcds * mCompRaw->size];
	
	if (!mStable)
	{
		char *tmpbuff = OmnNew char[mCompRaw->size];
		AosSort::sort(first, last, mCompRaw, tmpbuff);
		OmnDelete [] tmpbuff;
	}
	else
	{
		char *tmpbuff = OmnNew char[mCompRaw->size];
		AosSort::sort(first, last, mCompRaw, tmpbuff);
		OmnDelete [] tmpbuff;
	}

	mIsSorted = true;
	mLockRaw->unlock();

	//i64 t2 = OmnGetTimestamp();
	//OmnScreen << "Sort time: " << AosTimestampToHumanRead(t2 - t1)
		//<< " Entries: " << mNumRcds << endl;

	return true;
}

//felicia, 2015/08/28 for streaming merge data
bool
AosBuffArrayVar::mergeData()
{
	aos_assert_r(mHeadBuffRaw, false);
	char * entry = mHeadBuffRaw->data();
	char * crt_entry = &entry[mCompRaw->size];
	aos_assert_r(entry, false);

	u32 idx = 0;
	u32 num_entries = mNumRcds;

	char *data = NULL;
	int  len = 0;
	if(!mCompRaw->hasAgrStr())
	{
		for (u32 i=1; i<num_entries; i++)
		{
			if (mCompRaw->cmp(entry, crt_entry) == 0)
			{
				mCompRaw->mergeData(entry, crt_entry, data, len);
			}
			else
			{
				idx++;
				entry = &entry[mCompRaw->size];
				if (entry != crt_entry)
				{
					memcpy(entry, crt_entry, mCompRaw->size);
				}
			}
			crt_entry = &crt_entry[mCompRaw->size];
		}
	}
	else
	{
		mHeadBuffRaw->reset();
		AosBuffPtr newBodyBuff = OmnNew AosBuff(mBodyBuffRaw->buffLen() AosMemoryCheckerArgs);
		memset(newBodyBuff->data(), 0, newBodyBuff->buffLen());
		i64 newBodyAddr = (i64)newBodyBuff->data();
		data = newBodyBuff->data();
		int offset = 0;
		char *data2 = 0;
		int len2 = 0;
		int lastEntryLen = 0;
		bool rslt = false;

		for (u32 i=1; i<num_entries; i++)
		{
			if (mCompRaw->cmp(entry, crt_entry) == 0)
			{
				mCompRaw->mergeData(entry, crt_entry, data+offset, len);
				len += sizeof(u32);
				if (i == 1)
				{
					buildNewHeaderBuff(offset, newBodyAddr, data);
					offset += len;
					lastEntryLen = len;
				}
				else
				{
					memmove(data+offset-lastEntryLen, data+offset, len);
					offset -= lastEntryLen;
					mHeadBuffRaw->setCrtIdx(mHeadBuffRaw->getCrtIdx() - mCompRaw->size);
					buildNewHeaderBuff(offset, newBodyAddr, data);
					offset += len;
					lastEntryLen = len;
				}
			}
			else
			{
				idx++;
				entry = &entry[mCompRaw->size];
				if (entry != crt_entry)
				{
					data2 = (char*)(*(i64*)(crt_entry+sizeof(int))) + (*(int*)crt_entry);
					len2 = *((u32*)data2);
					rslt = AosBuff::decodeRecordBuffLength(len2);
					aos_assert_r(rslt, false);
					len2 += sizeof(u32);
					memcpy(data+offset, data2, len2);

					buildNewHeaderBuff(offset, newBodyAddr, data);
					entry = mHeadBuffRaw->data() + idx * mCompRaw->size;
					offset += len2;
					lastEntryLen = len2;
				}
				else
				{
					if (idx == 1)
					{
						data2 = (char*)(*(i64*)(entry-mCompRaw->size+sizeof(int))) + (*(int*)(entry-mCompRaw->size));
						len2 = *((u32*)data2);
						rslt = AosBuff::decodeRecordBuffLength(len2);
						aos_assert_r(rslt, false);
						len2 += sizeof(u32);
						memcpy(data+offset, data2, len2);
						buildNewHeaderBuff(offset, newBodyAddr, data);
						offset += len2;
					}

					data2 = (char*)(*(i64*)(crt_entry+sizeof(int))) + (*(int*)crt_entry);
					len2 = *((u32*)data2);
					rslt = AosBuff::decodeRecordBuffLength(len2);
					aos_assert_r(rslt, false);
					len2 += sizeof(u32);
					memcpy(data+offset, data2, len2);
					buildNewHeaderBuff(offset, newBodyAddr, data);
					entry = mHeadBuffRaw->data() + idx * mCompRaw->size;
					offset += len2;
					lastEntryLen = len2;
				}
			}
			crt_entry = &crt_entry[mCompRaw->size];
		}
		mBodyBuff = newBodyBuff;
		mBodyBuffRaw = mBodyBuff.getPtr();
		mBodyAddr = newBodyAddr;
		mBodyBuffRaw->setDataLen(offset);
	}
	mHeadBuffRaw->setDataLen((idx + 1) * mCompRaw->size);	
	mNumRcds = idx + 1;
	return true;
}

bool
AosBuffArrayVar::buildNewHeaderBuff(
		const int rcd_offset,
		const i64 new_body_addr,
		char *body)
{
	mHeadBuffRaw->setInt(rcd_offset);
	mHeadBuffRaw->setI64(new_body_addr);
	vector<AosDataFieldType::E> types;
	mComp->getDataFieldsType(types);
	aos_assert_r(types.size() < 65536, false);
	buildHeaderBuff(types, mHeadBuffRaw, body + rcd_offset + sizeof(int));
	return true;
}

bool
AosBuffArrayVar::printBody()
{
	char * data;
	int len;

	for (int i=0; i<mNumRcds; i++)
	{
		bool rslt = getEntry(i, data, len);
		aos_assert_r(rslt, -1);
		OmnScreen  << "record : " << *(u64 *)&data[7] << endl;

	}
	return true;
}

bool
AosBuffArrayVar::rebuildData()
{
	if (mNumRcds <= 1) return true;

	AosBuffPtr headBuff = mHeadBuff;
	AosBuffPtr bodyBuff = mBodyBuff;
	char *offsets = headBuff->data();
	char *md = bodyBuff->data();

	mHeadBuff = OmnNew AosBuff(headBuff->buffLen() AosMemoryCheckerArgs);
	mHeadBuffRaw = mHeadBuff.getPtr();
	mHeadBuffRaw->setDataLen(mNumRcds * mHeadSize);
	mHeadBuffRaw->reset();

	mBodyBuff = OmnNew AosBuff(bodyBuff->buffLen() AosMemoryCheckerArgs);
	mBodyBuffRaw = mBodyBuff.getPtr();
	mBodyBuffRaw->reset();
	mBodyAddr = (i64)mBodyBuffRaw->data();
	
	int offset, new_offset;
	char *data;
	int len;
	for (int i=0; i<mNumRcds; i++)
	{
		offset = offsets[i * mHeadSize];
		len = *(int*)&md[offset];
		data = &md[offset + sizeof(int)];

		new_offset = mBodyBuff->getCrtIdx();
		mBodyBuff->setCharStr(data, len);
		mHeadBuffRaw->setInt(new_offset);
		mHeadBuffRaw->setI64(mBodyAddr);
	}

	if (mBodyBuff->dataLen() + mWasteSize != bodyBuff->dataLen())
	{
		OmnAlarm << "waste mem size error"
			<< ", old datalen:" << bodyBuff->dataLen()
			<< ", new datalen:" << mBodyBuff->dataLen()
			<< ", waste size:" << mWasteSize << enderr;
	}

	mWasteSize = 0;
	mIsSorted = true;

	return true;
}


AosBuffArrayVarPtr
AosBuffArrayVar::split(const AosRundataPtr &rdata)
{
	mLockRaw->lock();
	if (mNumRcds <= 1)
	{
		mLockRaw->unlock();
		return 0;
	}

	char *offsets = mHeadBuff->data();
	char *md = mBodyBuff->data();

	i64 num1 = mNumRcds / 2;
	i64 num2 = mNumRcds - num1;

	AosBuffPtr headBuff1 = OmnNew AosBuff(mHeadBuff->buffLen() AosMemoryCheckerArgs);
	AosBuffPtr headBuff2 = OmnNew AosBuff(mHeadBuff->buffLen() AosMemoryCheckerArgs);
	headBuff1->setDataLen(num1 * mHeadSize);
	headBuff2->setDataLen(num2 * mHeadSize);
	headBuff1->reset();
	headBuff2->reset();

	AosBuffPtr bodyBuff1 = OmnNew AosBuff(mBodyBuff->buffLen() AosMemoryCheckerArgs);
	AosBuffPtr bodyBuff2 = OmnNew AosBuff(mBodyBuff->buffLen() AosMemoryCheckerArgs);
	bodyBuff1->reset();
	bodyBuff2->reset();
	i64 bodyAddr1 = (i64)bodyBuff1->data();
	i64 bodyAddr2 = (i64)bodyBuff2->data();

	int offset, new_offset;
	char *data;
	int len;

	for (int i=0; i<num1; i++)
	{
		offset = *(int*)&offsets[i * mHeadSize];
		len = *(int*)&md[offset];
		data = &md[offset + sizeof(int)];

		new_offset = bodyBuff1->getCrtIdx();
		bodyBuff1->setCharStr(data, len);
		headBuff1->setInt(new_offset);
		headBuff1->setI64(bodyAddr1);
	}

	for (int i=0; i<num2; i++)
	{
		offset = *(int*)&offsets[(num1 + i) * mHeadSize];
		len = *(int*)&md[offset];
		data = &md[offset + sizeof(int)];

		new_offset = bodyBuff2->getCrtIdx();
		bodyBuff2->setCharStr(data, len);
		headBuff2->setInt(new_offset);
		headBuff2->setI64(bodyAddr2);
	}

	if (bodyBuff1->dataLen() + bodyBuff2->dataLen() + mWasteSize != mBodyBuff->dataLen())
	{
		OmnAlarm << "waste mem size error"
			<< ", old datalen:" << mBodyBuff->dataLen()
			<< ", new datalen1:" << bodyBuff1->dataLen()
			<< ", new datalen2:" << bodyBuff2->dataLen()
			<< ", waste size:" << mWasteSize << enderr;
	}

	mWasteSize = 0;
	mIsSorted = true;

	mNumRcds = num1;
	mHeadBuff = headBuff1;
	mHeadBuffRaw = headBuff1.getPtr();
	mBodyBuff = bodyBuff1;
	mBodyBuffRaw = bodyBuff1.getPtr();
	mBodyAddr = bodyAddr1;

	AosBuffArrayVarPtr array = create(mComp, mStable, rdata);
	aos_assert_rl(array, mLockRaw, 0);

	array->mNumRcds = num2;
	array->mHeadBuff = headBuff2;
	array->mHeadBuffRaw = headBuff2.getPtr();
	array->mBodyBuff = bodyBuff2;
	array->mBodyBuffRaw = bodyBuff2.getPtr();
	array->mBodyAddr = bodyAddr2;

	mLockRaw->unlock();
	return array;
}


bool
AosBuffArrayVar::getDataFromBuff(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	mLockRaw->lock();
	mIsSorted = buff->getBool(true);

	mNumRcds = buff->getInt(-1);
	aos_assert_rl(mNumRcds >= 0, mLockRaw, false);
	
	mWasteSize = buff->getInt(-1);
	aos_assert_rl(mWasteSize >= 0, mLockRaw, false);

	i64 olen = buff->getI64(-1);
	aos_assert_rl(olen == mNumRcds * (i64)sizeof(int), mLockRaw, false);

	if (olen > 0)
	{
		mHeadBuff = buff->getBuff(olen, true AosMemoryCheckerArgs);
		mHeadBuffRaw = mHeadBuff.getPtr();
	}

	i64 blen = buff->getI64(-1);
	aos_assert_rl(blen >= 0, mLockRaw, false);

	if (blen > 0)
	{
		mBodyBuff = buff->getBuff(blen, true AosMemoryCheckerArgs);
		mBodyBuffRaw = mBodyBuff.getPtr();
		mBodyAddr = (i64)mBodyBuffRaw->data();
	}
	else
	{
		aos_assert_rl(olen == 0, mLockRaw, false);	
	}
	mLockRaw->unlock();

	return true;
}


bool
AosBuffArrayVar::setDataToBuff(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	mLockRaw->lock();

	bool rslt = buff->setBool(mIsSorted);
	aos_assert_rl(rslt, mLockRaw, false);
	rslt = buff->setInt(mNumRcds);
	aos_assert_rl(rslt, mLockRaw, false);
	rslt = buff->setInt(mWasteSize);
	aos_assert_rl(rslt, mLockRaw, false);

	i64 olen = mHeadBuffRaw->dataLen();
	aos_assert_rl(olen >= 0, mLockRaw, false);
	rslt = buff->setI64(olen);
	aos_assert_rl(rslt, mLockRaw, false);
	if (olen > 0)
	{
		rslt = buff->setBuff(mHeadBuff);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	i64 blen = mBodyBuffRaw->dataLen();
	aos_assert_rl(blen >= 0, mLockRaw, false);
	rslt = buff->setI64(blen);
	aos_assert_rl(rslt, mLockRaw, false);
	if (blen > 0)
	{
		rslt = buff->setBuff(mBodyBuff);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	mLockRaw->unlock();

	return true;
}


bool
AosBuffArrayVar::setBodyToBuff(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	mLockRaw->lock();

	bool rslt;
	int offset;
	char * data;
	int len;

	for (i64 i=0; i<mNumRcds; i++)
	{
		offset = mHeadBuffRaw->getInt(i * mHeadSize, -1);
		aos_assert_rl(offset >= 0, mLockRaw, false);

		data = mBodyBuffRaw->getCharStr(offset, len);
		aos_assert_rl(data != NULL, mLockRaw, false);
		
		rslt = buff->setCharStr(data, len, false);
		aos_assert_rl(rslt, mLockRaw, false);
	}

	mLockRaw->unlock();

	return true;
}


int
AosBuffArrayVar::cmp(
		const AosBuffArrayVarPtr &lhs,
		const i64 &lidx,
		const AosBuffArrayVarPtr &rhs,
		const i64 &ridx)
{
	aos_assert_r(lhs && rhs, -1);
	aos_assert_r(mCompRaw, -1);

	char * ld;
	char * rd;
	int ll, rl;

	bool rslt = lhs->getEntry(lidx, ld, ll);
	aos_assert_r(rslt, -1);

	rslt = rhs->getEntry(ridx, rd, rl);
	aos_assert_r(rslt, -1);

	return mCompRaw->cmp(ld, rd);
}


bool
AosBuffArrayVar::setBodyBuff(
		const AosBuffPtr &buff,
		int &remain_size,
		bool encode)
{
	//encode false: buff record length not encode
	//encode true: buff record length encode
	aos_assert_r(buff, false);

	remain_size = 0;

	mLockRaw->lock();
	mIsSorted = false;
	mNumRcds = 0;
	mWasteSize = 0;

	mHeadBuff = OmnNew AosBuff(eInitHeadSize AosMemoryCheckerArgs);
	mHeadBuffRaw = mHeadBuff.getPtr();
	mHeadBuffRaw->reset();

	mBodyBuff = buff;
	mBodyBuffRaw = mBodyBuff.getPtr();
	mBodyBuffRaw->reset();
	mBodyAddr = (i64)mBodyBuffRaw->data();

	vector<AosDataFieldType::E> types;
	mComp->getDataFieldsType(types);
	aos_assert_r(types.size() < 65536, false);
	int offset = 0;
	int len = 0;
	while (1)
	{
		if (mBodyBuffRaw->remainingReadSize() < (i64)sizeof(int))
		{
			break;
		}

		offset = mBodyBuffRaw->getCrtIdx();
		len = mBodyBuffRaw->getInt(-1);
		int decode_len = len;
		if (encode)
		{
			bool rslt = AosBuff::decodeRecordBuffLength(decode_len);
			aos_assert_r(rslt, false);
		}

		if (mBodyBuffRaw->remainingReadSize() < decode_len)
		{
			mBodyBuffRaw->backInt();
			break;
		}
		
		mHeadBuffRaw->setInt(offset);
		mHeadBuffRaw->setI64(mBodyAddr);
		char *body_buff_ptr  = mBodyBuffRaw->getCrtPos();
		bool rslt = buildHeaderBuff(types, mHeadBuffRaw, body_buff_ptr);
		aos_assert_r(rslt, false);
		//mBodyBuffRaw->setCrtIdx(offset + decode_len + sizeof(int));
		mBodyBuffRaw->setCrtIdx(offset + decode_len + sizeof(int));
		mNumRcds++;
	}

	remain_size = mBodyBuffRaw->remainingReadSize();
	mBodyBuffRaw->removeData(remain_size);
	mLockRaw->unlock();

	return true;
}


bool
AosBuffArrayVar::buildHeaderBuff(
		vector<AosDataFieldType::E> &types,
		AosBuff *header_buff,
		char *body_buff)
{
	int crt_pos = 0, str_len = 0;
	for (size_t i = 0; i < types.size(); i++)
	{
		switch(types[i])
		{
			case AosDataFieldType::eStr:
			case AosDataFieldType::eBuff:
				str_len = *(int *)(body_buff + sizeof(i8));
				crt_pos += sizeof(int) + sizeof(i8);
				header_buff->setU16(crt_pos);
				crt_pos += str_len;
				body_buff = body_buff + sizeof(i8) + sizeof(int) + str_len;  
				break;

			case AosDataFieldType::eBinDouble:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8)+sizeof(double);
				body_buff = body_buff + sizeof(i8)+sizeof(double);
				break;

			case AosDataFieldType::eBinU64:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8)+sizeof(u64);
				body_buff = body_buff + sizeof(i8) + sizeof(u64);
				break;

			case AosDataFieldType::eBinInt64:
			case AosDataFieldType::eBinDateTime:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8)+sizeof(int64_t);
				body_buff = body_buff + sizeof(i8) + sizeof(int64_t);
				break;

			case AosDataFieldType::eBinU32:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8) + sizeof(u32);
				body_buff = body_buff + sizeof(i8) + sizeof(u32);
				break;

			case AosDataFieldType::eBinInt:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8)+sizeof(int);
				body_buff = body_buff + sizeof(i8) + sizeof(int);
				break;

			case AosDataFieldType::eBinChar:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8) + sizeof(char);
				body_buff = body_buff + sizeof(i8) + sizeof(char);
				break;

			case AosDataFieldType::eBinBool:
				header_buff->setU16(crt_pos+sizeof(i8));
				crt_pos += sizeof(i8) + sizeof(bool);
				body_buff = body_buff + sizeof(i8) + sizeof(bool);
				break;

			default:
				OmnAlarm << "error, type: " << AosDataFieldType::toString(types[i]) << enderr;
				break;
		}
	}
	return true;
}


bool
AosBuffArrayVar::remainingBodyBuff(
		const AosBuffPtr &buff,
		int &remain_size)
{
	aos_assert_r(buff, false);

	buff->reset();
	remain_size = 0;
	int len = 0;
	while (1)
	{
		if (buff->remainingReadSize() < (i64)sizeof(int))
		{
			break;
		}

		len = buff->getInt(-1);
		if (buff->remainingReadSize() < len)
		{
			buff->backInt();
			break;
		}

		buff->incIndex(len);
	}

	remain_size = buff->remainingReadSize();
	buff->removeData(remain_size);

	return true;
}


bool
AosBuffArrayVar::rebuildBodyAddr()
{
	mBodyAddr = (i64)mBodyBuffRaw->data();
	mHeadBuffRaw->reset();
	for (int i=0; i<mNumRcds; i++)
	{
		//Jozhi 
		//mHeadBuffRaw->incIndex(sizeof(int));
		mHeadBuffRaw->setCrtIdx((i*mComp->size)+sizeof(int));
		mHeadBuffRaw->setI64(mBodyAddr);
	}

	return true;
}


bool
AosBuffArrayVar::procHeaderBuff(
			const AosCompareFunPtr &comp, 
			const AosBuffPtr &headBuff, 
			AosBuffPtr &bodyBuff,
			set<i64> &bodyAddrSet,
			bool encode)
{
	aos_assert_r(headBuff, false);
	aos_assert_r(headBuff->dataLen() % comp->size == 0, false);
	headBuff->reset();
	i64 num = headBuff->dataLen() / comp->size;
	bodyBuff = OmnNew AosBuff(eMaxBodyBuffSize AosMemoryCheckerArgs);

	bool rslt;
	i64 bodyAddr;
	int offset = -1; int len = -1; int decode_len = -1;
	const char *body;
	const char *data;
	for (i64 i=0; i<num; i++)
	{
		headBuff->setCrtIdx(i*comp->size);
		offset = headBuff->getInt(-1);
		bodyAddr = headBuff->getI64(-1);
		aos_assert_r(offset >= 0 && bodyAddr > 0, false);

		bodyAddrSet.insert(bodyAddr);
		body = (const char*)bodyAddr;
		len = *((int*)(body+offset));
		decode_len = len;
		if (encode)
		{
			rslt = AosBuff::decodeRecordBuffLength(decode_len);
			aos_assert_r(rslt, false);
			data = body+offset+sizeof(int);
			rslt = bodyBuff->setEncodeCharStr(data, decode_len);
			aos_assert_r(rslt, false);
		}
		else
		{
			data = body+offset+sizeof(int);
			rslt = bodyBuff->setCharStr(data, decode_len);
			aos_assert_r(rslt, false);
		}
	}
	headBuff->reset();
	return true;
}


