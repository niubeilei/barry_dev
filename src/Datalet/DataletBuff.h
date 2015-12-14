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
// 2013/03/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Datalet_DataletBuff_h
#define Aos_Datalet_DataletBuff_h

#include "Datalet/Datalet.h"
#include "Message/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;

class AosDataletBuff : public AosDatalet
{
	OmnDefineRCObject;

protected:
	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str>::iterator mapitr_t;

	enum
	{
		eU64ByName,
		eU64ByNameId
	};

	OmnMutexPtr		mLock;
	AosBuffPtr		mBuff;
	map_t			mFieldMap;
	OmnString		mObjid;
	bool			mIsLocal;
	AosSNamePtr		mNameMap;
	vector<int64_t>	mPosMap;

public:
	AosDataletBuff();
	AosDataletBuff(
		const AosRundataPtr &rdata, 
		const OmnString &objid, 
		const bool isLocal);
	virtual ~AosDataletBuff();

	// Datalet Interface
	virtual AosDataletType::E getType();
	virtual AosRundataPtr getRundata();
	virtual AosBuffPtr getBuff() const;
	virtual AosDataletPtr clone(const AosRundataPtr &rdata);

	virtual AosDatalet &operator [](const OmnString &id);
	virtual AosDatalet &operator = (const u64 &value);

	virtual bool set(const OmnString &name, const AosValue &value);
	virtual AosValue get(const OmnString &name);

	virtual bool setU64(const OmnString &name, const u64 &value);
	virtual u64  getU64(const OmnString &name, const u64 &dft);

	// virtual bool set(const AosSID::E field, const AosValue &value);
	// virtual bool set(const u32 field_idx, const AOsValue &value);
	// virtual AosValue get(const AosSID::E field);
	// virtual AosValue get(const u32 field_idx);
	// virtual bool setU64(const AosDict::E field, const u64 &value);
	// virtual bool set(const u32 field_idx, const u64 &value);
	// virtual u64  getU64(const AosDict::E field, const u64 &dft);
	// virtual u64  getU64(const int field_idx, const u64 &dft);
	// virtual bool setU32(const AosDict::E field, const u32 value);
	// virtual bool setU16(const AosDict::E field, const u16 value);
	// virtual bool setU8(const AosDict::E field, const u8 value);
	// virtual bool setInt64(const AosDict::E field, const int64_t &value);
	// virtual bool setInt(const AosDict::E field, const int value);
	// virtual bool setInt16(const AosDict::E field, const int16_t value);
	// virtual bool setChar(const AosDict::E field, const char value);
	// virtual bool setFloat(const AosDict::E field, const float &value);
	// virtual bool setDouble(const AosDict::E field, const double &value);
	// virtual bool setStr(const AosDict::E field, const OmnString &value);
	// virtual bool setBuff(const AosDict::E field, const AosBuffPtr &buff);

	// virtual bool setU32(const OmnString &name, const u32 value);
	// virtual bool setU16(const OmnString &name, const u16 value);
	// virtual bool setU8(const OmnString &name, const u8 value);
	// virtual bool setInt64(const OmnString &name, const int64_t &value);
	// virtual bool setInt(const OmnString &name, const int value);
	// virtual bool setInt16(const OmnString &name, const int16_t value);
	// virtual bool setChar(const OmnString &name, const char value);
	// virtual bool setFloat(const OmnString &name, const float &value);
	// virtual bool setDouble(const OmnString &name, const double &value);
	// virtual bool setStr(const OmnString &name, const OmnString &value);
	// virtual bool setBuff(const OmnString &name, const AosBuffPtr &buff);

	// virtual bool set(const u32 field_idx, const u32 value);
	// virtual bool set(const u32 field_idx, const u16 value);
	// virtual bool set(const u32 field_idx, const u8 value);
	// virtual bool set(const u32 field_idx, const int64_t &value);
	// virtual bool set(const u32 field_idx, const int value);
	// virtual bool set(const u32 field_idx, const int16_t value);
	// virtual bool set(const u32 field_idx, const char value);
	// virtual bool set(const u32 field_idx, const float &value);
	// virtual bool set(const u32 field_idx, const double &value);
	// virtual bool set(const u32 field_idx, const OmnString &value);
	// virtual bool set(const u32 field_idx, const AosBuffPtr &buff);

	// virtual u32 		getU32(const AosDict::E field, const u32 dft);
	// virtual u16 		getU16(const AosDict::E field, const u16 dft);
	// virtual u8  		getU8(const AosDict::E field, const u8 dft);
	// virtual int64_t 	getInt64(const AosDict::E field, const int64_t &dft);
	// virtual int 		getInt(const AosDict::E field, const int dft);
	// virtual int16_t 	getInt16(const AosDict::E field, const int16_t dft);
	// virtual char 		getChar(const AosDict::E field, const char dft);
	// virtual float 		getFloat(const AosDict::E field, const float dft);
	// virtual double		getDouble(const AosDict::E field, const double &dft);
	// virtual OmnString 	getStr(const AosDict::E field, const OmnString &dft);
	// virtual AosBuffPtr 	getBuff(const AosDict::E field, const bool copy);

	// virtual u32 		getU32(const OmnString &name, const u32 dft);
	// virtual u16 		getU16(const OmnString &name, const u16 dft);
	// virtual u8  		getU8(const OmnString &name, const u8 dft);
	// virtual int64_t 	getInt64(const OmnString &name, const int64_t &dft);
	// virtual int 		getInt(const OmnString &name, const int dft);
	// virtual int16_t 	getInt16(const OmnString &name, const int16_t dft);
	// virtual char 		getChar(const OmnString &name, const char dft);
	// virtual float 		getFloat(const OmnString &name, const float dft);
	// virtual double		getDouble(const OmnString &name, const double &dft);
	// virtual OmnString 	getStr(const OmnString &name, const OmnString &dft);
	// virtual AosBuffPtr 	getBuff(const OmnString &name, const bool copy);

	// virtual u32 		getU32(const int field_idx, const u32 dft);
	// virtual u16 		getU16(const int field_idx, const u16 dft);
	// virtual u8 			getU8(const int field_idx, const u8 dft);
	// virtual int64_t 	getInt64(const int field_idx, const int64_t &dft);
	// virtual int 		getInt(const int field_idx, const int dft);
	// virtual int16_t 	getInt16(const int field_idx, const int16_t dft);
	// virtual char 		getChar(const int field_idx, const char dft);
	// virtual float 		getFloat(const int field_idx, const float dft);
	// virtual double 		getDouble(const int field_idx, const double &dft);
	// virtual OmnString 	getStr(const int field_idx, const OmnString &dft);
	// virtual AosBuffPtr 	getBuff(const int field_idx, const bool copy);
};
#endif
