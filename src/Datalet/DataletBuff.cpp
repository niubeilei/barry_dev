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
// 2013/03/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Datalet/DataletBuff.h"

#include "Alarm/Alarm.h"
#include "Datalet/SName.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/DataTypes.h"

AosDataletBuff::AosDataletBuff()
:
AosDatalet(),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mIsLocal(false)
{
}


AosDataletBuff::AosDataletBuff(
		const AosRundataPtr &rdata, 
		const OmnString &objid, 
		const bool isLocal)
:
AosDatalet(),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin)),
mObjid(objid),
mIsLocal(isLocal)
{
}


AosDataletBuff::~AosDataletBuff()
{
}


AosDatalet &
AosDataletBuff::operator [](const OmnString &id)
{
	OmnNotImplementedYet;
	return *this;
}


AosDatalet &
AosDataletBuff::operator = (const u64 &value)
{
	OmnNotImplementedYet;
	return *this;
}


AosDataletType::E 
AosDataletBuff::getType()
{
	return AosDataletType::eBuffDatalet;
}


AosRundataPtr 
AosDataletBuff::getRundata()
{
	OmnNotImplementedYet;
	return 0;
}


AosBuffPtr 
AosDataletBuff::getBuff() const
{
	OmnNotImplementedYet;
	return 0;
}


AosDataletPtr 
AosDataletBuff::clone(const AosRundataPtr &rdata)
{
	return OmnNew AosDataletBuff();
}


/*
bool 
AosDataletBuff::setU64(const AosSID::E field, const u64 &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU64ById);
	mBuff->setU16(field);
	mBuff->setU64(value);
	return true;
}


bool 
AosDataletBuff::setU64(const u32 idx, const u64 &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU64ByIdx);
	mBuff->setU32(idx);
	mBuff->setU64(value);
	return true;
}
*/

bool 
AosDataletBuff::set(const OmnString &name, const AosValue &value)
{
	OmnNotImplementedYet;
	return false;
}


AosValue 
AosDataletBuff::get(const OmnString &name)
{
	OmnNotImplementedYet;
	return AosValue();
}


bool 
AosDataletBuff::setU64(const OmnString &name, const u64 &value)
{
	aos_assert_r(mBuff, false);
	mLock->lock();
	if (mNameMap)
	{
		u32 idx = mNameMap->getNameIdx(name);
		if (AosSName::isValidIdx(idx))
		{
			int64_t pos = mBuff->getCrtIdx();
			mBuff->setU8(eU64ByNameId);
			mBuff->setU32(idx);
			mBuff->setU64(value);
			if (idx >= mPosMap.size())
			{
				mPosMap.resize(idx);
			}
			mPosMap[idx] = pos;
			mLock->unlock();
			return true;
		}
	}

	mBuff->setU8(eU64ByName);
	mBuff->setOmnStr(name);
	mBuff->setU64(value);
	mLock->unlock();
	return true;
}


u64 	
AosDataletBuff::getU64(const OmnString &name, const u64 &dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getU64(dft);
}



/*
u64 
AosDataletBuff::getU64(const AosSID::E field, const u64 &dft)
{
	// Fields are identified by 'AosSID'. There is a map that 
	// maps SID to the location in which the field is defined.
	// The map is created when the message is parsed.
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU64, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU64(dft);
}


bool 
AosDataletBuff::segFieldU32(const AosMsgFieldId::E field, const u32 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU32);
	mBuff->setU32(value);
	return true;
}


bool 
AosDataletBuff::segFieldU16(const AosMsgFieldId::E field, const u16 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU16);
	mBuff->setU16(value);
	return true;
}


bool 
AosDataletBuff::segFieldU8(const AosMsgFieldId::E field, const u8 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eU8);
	mBuff->setU8(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt64(const AosMsgFieldId::E field, const int64_t &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt64);
	mBuff->setInt64(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt(const AosMsgFieldId::E field, const int value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt32);
	mBuff->setInt(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt16(const AosMsgFieldId::E field, const int16_t value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eInt16);
	mBuff->setInt16(value);
	return true;
}


bool 
AosDataletBuff::segFieldChar(const AosMsgFieldId::E field, const char value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eChar);
	mBuff->setChar(value);
	return true;
}


bool 
AosDataletBuff::segFieldFloat(const AosMsgFieldId::E field, const float &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eFloat);
	mBuff->setFloat(value);
	return true;
}


bool 
AosDataletBuff::segFieldDouble(const AosMsgFieldId::E field, const double &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eDouble);
	mBuff->setDouble(value);
	return true;
}


bool 
AosDataletBuff::segFieldStr(const AosMsgFieldId::E field, const OmnString &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eString);
	mBuff->setOmnStr(value);
	return true;
}


bool 
AosDataletBuff::segFieldBuff(const AosMsgFieldId::E field, const AosBuffPtr &buff)
{
	aos_assert_r(mBuff, false);
	mBuff->setU16(field);
	mBuff->setU8(AosDataType::eBuff);
	mBuff->setBuff(buff);
	return true;
}


u32 
AosDataletBuff::getFieldU32(const AosMsgFieldId::E field, const u32 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU32, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU32(dft);
}


u16
AosDataletBuff::getFieldU16(const AosMsgFieldId::E field, const u16 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU16, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU16(dft);
}


u8
AosDataletBuff::getFieldU8(const AosMsgFieldId::E field, const u8 dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eU8, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getU8(dft);
}


int64_t
AosDataletBuff::getFieldInt64(const AosMsgFieldId::E field, const int64_t &dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt64, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getI64(dft);
}


int 
AosDataletBuff::getFieldInt(const AosMsgFieldId::E field, const int dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt32, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getInt(dft);
}


int16_t 
AosDataletBuff::getFieldInt16(const AosMsgFieldId::E field, const int16_t dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eInt16, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getI16(dft);
}


char
AosDataletBuff::getFieldChar(const AosMsgFieldId::E field, const char dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eChar, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getChar(dft);
}


OmnString
AosDataletBuff::getFieldStr(const AosMsgFieldId::E field, const OmnString &dft)
{
	aos_assert_r(mBuff, dft);
	aos_assert_r(field < mFieldIdx.size(), dft);
	aos_assert_r(mFieldType[field] == AosDataType::eString, dft);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getOmnStr(dft);
}


AosBuffPtr
AosDataletBuff::getFieldBuff(const AosMsgFieldId::E field, const bool copy)
{
	aos_assert_r(mBuff, 0);
	aos_assert_r(field < mFieldIdx.size(), 0);
	aos_assert_r(mFieldType[field] == AosDataType::eString, 0);
	mBuff->setCrtIdx(mFieldIdx[field]);
	return mBuff->getBuffAsBinary(copy);
}


bool 
AosDataletBuff::segFieldU32(const OmnString &name, const u32 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU32);
	mBuff->setOmnStr(name);
	mBuff->setU32(value);
	return true;
}


bool 
AosDataletBuff::segFieldU16(const OmnString &name, const u16 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU16);
	mBuff->setOmnStr(name);
	mBuff->setU16(value);
	return true;
}


bool 
AosDataletBuff::segFieldU8(const OmnString &name, const u8 value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eU8);
	mBuff->setOmnStr(name);
	mBuff->setU8(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt64(const OmnString &name, const int64_t &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt64);
	mBuff->setOmnStr(name);
	mBuff->setInt64(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt(const OmnString &name, const int value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt32);
	mBuff->setOmnStr(name);
	mBuff->setInt(value);
	return true;
}


bool 
AosDataletBuff::segFieldInt16(const OmnString &name, const int16_t value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eInt16);
	mBuff->setOmnStr(name);
	mBuff->setInt16(value);
	return true;
}


bool 
AosDataletBuff::segFieldChar(const OmnString &name, const char value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eChar);
	mBuff->setOmnStr(name);
	mBuff->setChar(value);
	return true;
}


bool 
AosDataletBuff::segFieldFloat(const OmnString &name, const float &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eFloat);
	mBuff->setOmnStr(name);
	mBuff->setFloat(value);
	return true;
}


bool 
AosDataletBuff::segFieldDouble(const OmnString &name, const double &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eDouble);
	mBuff->setOmnStr(name);
	mBuff->setDouble(value);
	return true;
}


bool 
AosDataletBuff::segFieldStr(const OmnString &name, const OmnString &value)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eString);
	mBuff->setOmnStr(name);
	mBuff->setOmnStr(value);
	return true;
}


bool 
AosDataletBuff::segFieldBuff(const OmnString &name, const AosBuffPtr &buff)
{
	aos_assert_r(mBuff, false);
	mBuff->setU8(AosDataType::eBuff);
	mBuff->setOmnStr(name);
	mBuff->appendBuff(buff);
	return true;
}


u32 	
AosDataletBuff::getFieldU32(const OmnString &name, const u32 dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getU32(dft);
}


u16 	
AosDataletBuff::getFieldU16(const OmnString &name, const u16 dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getU16(dft);
}


u8  	
AosDataletBuff::getFieldU8(const OmnString &name, const u8 dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getU8(dft);
}


int64_t 
AosDataletBuff::getFieldInt64(const OmnString &name, const int64_t &dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getI64(dft);
}


int 	
AosDataletBuff::getFieldInt(const OmnString &name, const int dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getInt(dft);
}


int16_t 
AosDataletBuff::getFieldInt16(const OmnString &name, const int16_t dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getI16(dft);
}


char 	
AosDataletBuff::getFieldChar(const OmnString &name, const char dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getChar(dft);
}


float 	
AosDataletBuff::getFieldFloat(const OmnString &name, const float dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getFloat(dft);
}


double	
AosDataletBuff::getFieldDouble(const OmnString &name, const double &dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getDouble(dft);
}


OmnString 
AosDataletBuff::getFieldStr(const OmnString &name, const OmnString &dft)
{
	aos_assert_r(name != "", dft);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return dft;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getOmnStr(dft);
}


AosBuffPtr 
AosDataletBuff::getFieldBuff(const OmnString &name, const bool copy)
{
	aos_assert_r(name != "", 0);
	mapitr_t itr = mFieldMap.find(name);
	if (itr == mFieldMap.end()) return 0;
	mBuff->setCrtIdx(itr->second);
	return mBuff->getBuffAsBinary(copy);
}


bool 
AosDataletBuff::segFieldU64(const u32 field_idx, const u64 &value)
{
	return appendFieldU64(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldU32(const u32 field_idx, const u32 value)
{
	return appendFieldU32(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldU16(const u32 field_idx, const u16 value)
{
	return appendFieldU16(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldU8(const u32 field_idx, const u8 value)
{
	return appendFieldU8(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldInt64(const u32 field_idx, const int64_t &value)
{
	return appendFieldInt64(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldInt(const u32 field_idx, const int value)
{
	return appendFieldInt(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldInt16(const u32 field_idx, const int16_t value)
{
	return appendFieldInt16(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldChar(const u32 field_idx, const char value)
{
	return appendFieldChar(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldFloat(const u32 field_idx, const float &value)
{
	return appendFieldFloat(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldDouble(const u32 field_idx, const double &value)
{
	return appendFieldDouble(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldStr(const u32 field_idx, const OmnString &value)
{
	return appendFieldStr(AosMsgFieldId::getUDF() + field_idx, value);
}


bool 
AosDataletBuff::segFieldBuff(const u32 field_idx, const AosBuffPtr &buff)
{
	return appendFieldBuff(AosMsgFieldId::getUDF() + field_idx, buff);
}


u64 
AosDataletBuff::getFieldU64(const int field_idx, const u64 &dft)
{
	return getFieldU64(AosMsgFieldId::getUDF() + field_idx, dft);
}


u32 
AosDataletBuff::getFieldU32(const int field_idx, const u32 dft)
{
	return getFieldU32(AosMsgFieldId::getUDF() + field_idx, dft);
}

u16 
AosDataletBuff::getFieldU16(const int field_idx, const u16 dft)
{
	return getFieldU16(AosMsgFieldId::getUDF() + field_idx, dft);
}


u8 
AosDataletBuff::getFieldU8(const int field_idx, const u8 dft)
{
	return getFieldU8(AosMsgFieldId::getUDF() + field_idx, dft);
}


int64_t 
AosDataletBuff::getFieldInt64(const int field_idx, const int64_t &dft)
{
	return getFieldInt64(AosMsgFieldId::getUDF() + field_idx, dft);
}


int 
AosDataletBuff::getFieldInt(const int field_idx, const int dft)
{
	return getFieldInt(AosMsgFieldId::getUDF() + field_idx, dft);
}


int16_t 
AosDataletBuff::getFieldInt16(const int field_idx, const int16_t dft)
{
	return getFieldInt16(AosMsgFieldId::getUDF() + field_idx, dft);
}


char 
AosDataletBuff::getFieldChar(const int field_idx, const char dft)
{
	return getFieldChar(AosMsgFieldId::getUDF() + field_idx, dft);
}


float 
AosDataletBuff::getFieldFloat(const int field_idx, const float dft)
{
	return getFieldFloat(AosMsgFieldId::getUDF() + field_idx, dft);
}


double 
AosDataletBuff::getFieldDouble(const int field_idx, const double &dft)
{
	return getFieldDouble(AosMsgFieldId::getUDF() + field_idx, dft);
}


OmnString 
AosDataletBuff::getFieldStr(const int field_idx, const OmnString &dft)
{
	return getFieldStr(AosMsgFieldId::getUDF() + field_idx, dft);
}


AosBuffPtr 
AosDataletBuff::getFieldBuff(const int field_idx, const bool copy)
{
	return getFieldBuff(AosMsgFieldId::getUDF() + field_idx, copy);
}
*/
#endif
