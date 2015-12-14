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
#ifndef AOS_Value_ValueInt8_h
#define AOS_Value_ValueInt8_h

#include "Value/ValueImp.h"

class AosValueInt8 : public AosValueImp
{
private:
	i8		mValue;

public:
	AosValueInt8()
	:
	mValue(0)
	{}

	AosValueInt8(const i8 value)
	:
	mValue(value)
	{
	}

	virtual ~AosValueInt8();

	// Value Interface
	virtual OmnString toString() const;
	virtual AosValueImp *getAosValue(const double value);
	virtual bool 	getBool() const;
	virtual char 	getChar() const;
	virtual int 	getInt() const;
	virtual i64 	getInt64() const;
	virtual u32 	getU32() const;
	virtual u64 	getU64() const;
	virtual double 	getDouble() const;
	virtual OmnString getOmnStr() const;
	virtual const char *getCharStr(int &len) const;
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata) const;
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
	friend ostream & operator<<(ostream &os, const AosValueInt8 &rhs);
	friend istream & operator>>(istream &is, const AosValueInt8 &rhs);
	virtual bool check(const AosOpr opr, const AosValueImp &rhs) const;
};
#endif

