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
// 2014/12/15 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Value_ValueU16_h
#define AOS_Value_ValueU16_h

#include "Value/ValueImp.h"

class AosValueU16 : public AosValueImp
{
private:
	u16		mValue;

public:
	AosValueU16()
	:
	mValue(0)
	{}

	AosValueU16(const u16 value)
	:
	mValue(value)
	{
	}

	virtual ~AosValueU16();

	// Value Interface
	virtual OmnString toString() const;
	virtual AosValueImp* getAosValue(const double value);
	virtual bool 	getBool() const;
	virtual char 	getChar() const;
	virtual int 	getInt() const;
	virtual i64 	getInt64() const;
	virtual u32 	getU32() const;
	virtual u64 	getU64() const;
	virtual double 	getDouble() const;
	virtual OmnString getOmnStr() const;
	virtual const char *getCharStr(int &len ) const;
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata ) const;
	virtual AosXmlTagPtr getXml() const;
	virtual i8		getInt8() const;
	virtual i16		getInt16() const;
	virtual u8		getU8() const;
	virtual u16		getU16() const;
	virtual float   getFloat() const;

	virtual bool    setInt8(const i8 vv);
	virtual bool    setInt16(const i16 vv);
	virtual bool    setU8(const u8 vv);
	virtual bool    setU16(const u16 vv);
	virtual bool    setFloat(const float vv);

	virtual bool 	setChar(const char vv);
	virtual bool 	setInt(const int vv);
	virtual bool 	setInt64(const int64_t vv);
	virtual bool 	setU32(const u32 vv);
	virtual bool 	setU64(const u64 vv);
	virtual bool 	setDouble(const double vv);
	virtual bool 	setOmnStr(const OmnString &vv);
	virtual bool 	setCharStr(const char *data, const int len, const bool copy_flag);
	virtual bool 	setXml(const AosXmlTagPtr &vv);
	virtual bool 	setBuff(const AosBuffPtr &buff);

	virtual AosValueImp& operator +(const AosValueImp &rhs);
	virtual AosValueImp& operator -(const AosValueImp &rhs);
	virtual AosValueImp& operator *(const AosValueImp &rhs);
	virtual AosValueImp& operator /(const AosValueImp &rhs);
	virtual bool operator ==(const AosValueImp &rhs);
	virtual bool operator !=(const AosValueImp &rhs);
	virtual bool operator >(const AosValueImp &rhs);
	virtual bool operator >=(const AosValueImp &rhs);
	virtual bool operator <(const AosValueImp &rhs);
	virtual bool operator <=(const AosValueImp &rhs);
	friend ostream & operator<<(ostream &os, const AosValueU16 &rhs);
	friend istream & operator>>(istream &is, const AosValueU16 &rhs);
	virtual bool check(const AosOpr opr, const AosValueImp &rhs) const;
};
#endif

