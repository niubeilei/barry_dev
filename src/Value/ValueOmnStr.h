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
/*
#ifndef AOS_Value_ValueOmnStr_h
#define AOS_Value_ValueOmnStr_h

#include "Value/ValueImp.h"

class AosValueOmnStr : public AosValueImp
{
private:
	OmnString mValue;

public:
	AosValueOmnStr(const OmnString &value)
	:
	mValue(value)
	{
		mIsNull = false;
	}

	virtual ~AosValueOmnStr();

	// Value Interface
	virtual OmnString toString() const;

	virtual bool 	getBool() const;
	virtual char 	getChar() const;
	virtual i8	 	getInt8() const;
	virtual i16 	getInt16() const;
	virtual u8 		getU8() const;
	virtual u16 	getU16() const;
	virtual float 	getFloat() const;
	virtual int 	getInt() const;
	virtual i64 	getInt64() const;
	virtual u32 	getU32() const;
	virtual u64 	getU64() const;
	virtual double 	getDouble() const;
	virtual OmnString getOmnStr() const;
	virtual const char *getCharStr(int &len ) const;
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata ) const;
	virtual AosXmlTagPtr getXml() const;

	virtual bool 	setOmnStr(const OmnString &vv);

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
	virtual bool check(const AosOpr opr, const AosValueImp &rhs) const;
};
#endif*/

