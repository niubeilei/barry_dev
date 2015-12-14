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
// 2013/03/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Value_Value_h
#define AOS_Value_Value_h

#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include "Value/ValueImp.h"
#include "Value/Ptrs.h"
#include "XmlUtil/Ptrs.h"



class AosValue
{
protected:
	bool			mIsNull;
	AosDataType::E	mDataType;
	AosValueImpPtr	mValue;
	AosValueImp *	mValueRaw;

public:
	AosValue(const bool is_null, const AosDataType::E type, const AosValueImpPtr &value); 
	AosValue(const u64 value); 
	~AosValue();

	inline AosDataType::E getDataType() const
	{
		return mDataType;
	}

	inline bool isNull() const
	{
		return mIsNull;
	}

	inline double getDouble() const
	{
	 	if(mIsNull || !mValue) return 0;
		return mValue->getDouble();
	}

	inline float getFloat() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getFloat();
	 }
	 inline i8 getInt8() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getInt8();
	 }
	 inline i16 getInt16() const
	 {
	 	if(mIsNull) return 0;
		return mValue->getInt16();
	 }
	 inline int getInt() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getInt();
	 }
	 inline i64 getInt64() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getInt64();
	 }
	 inline u8 getU8() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getU8();
	 }
	 inline u16 getU16() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getU16();
	 }
	 inline u64 getU64() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getU64();
	 }
	 inline u32 getU32() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getU32();
	 }
	 inline char getChar() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getChar();
	 }
	 inline bool getBool() const
	 {
	 	if(mIsNull || !mValue)
			return 0;
		return mValue->getBool();
	 }
	 inline OmnString getOmnStr() const
	{
		if(mIsNull || !mValue)
			return "";
		return mValue->getOmnStr();
	}
	double sqrt() const;
	double power(double s) const;
	const char *getCharStr(int &len) const;
	bool setU8(const u8 vv);
	bool setU16(const u16 vv);
	bool setU64(const u64 vv);
	bool setU32(const u32 vv);
	bool setInt8(const i8 vv);
	bool setInt16(const i16 vv);
	bool setInt64(const int64_t vv);
	bool setInt(const int vv);
	bool setFloat(const float vv);
	bool setDouble(const double vv);
	bool setChar(const char vv);
	bool setCharStr(const char *data, const int len, const bool copy_flag);
	bool setXml(const AosXmlTagPtr &vv);
	bool setBuff(const AosBuffPtr &buff);
	bool setQueryRslt(const AosQueryRsltObjPtr &vv);
	OmnString toString() const;

	AosValue &operator = (const AosValue &rhs);
	AosValue operator +(const AosValue &rhs);
	AosValue operator -(const AosValue &rhs);
	AosValue operator *(const AosValue &rhs);
	AosValue operator /(const AosValue &rhs);
	AosValue &operator +=(const AosValue &rhs);
	AosValue &operator -=(const AosValue &rhs);
	AosValue &operator *=(const AosValue &rhs);
	AosValue &operator /=(const AosValue &rhs);
	AosValue &operator ++();
	AosValue &operator --();

	bool operator ==(const AosValue &rhs);
	bool operator !=(const AosValue &rhs);
	bool operator >(const AosValue &rhs);
	bool operator >=(const AosValue &rhs);
	bool operator <(const AosValue &rhs);
	bool operator <=(const AosValue &rhs);

	friend ostream& operator<<(ostream& os,const AosValue& rhs);
	friend istream& operator>>(istream& is,const AosValue& rhs);
};
#endif

