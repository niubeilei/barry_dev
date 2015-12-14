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
#ifndef AOS_Value_ValueXml_h
#define AOS_Value_ValueXml_h

#include "Value/Value.h"

class AosValueXml : public AosValue
{
private:
	AosXmlTagPtr	mValue;

public:
	AosValueXml(const AosXmlTagPtr &value)
	:
	mValue(value)
	{
		mIsNull = false;
	}

	virtual ~AosValueXml();

	// Value Interface
	virtual OmnString toString() const;

	virtual bool 	getBool(AosErrCode &errcode) const;
	virtual char 	getChar(AosErrCode &errcode) const;
	virtual int 	getInt(AosErrCode &errcode) const;
	virtual i64 	getInt64(AosErrCode &errcode) const;
	virtual u32 	getU32(AosErrCode &errcode) const;
	virtual u64 	getU64(AosErrCode &errcode) const;
	virtual double 	getDouble(AosErrCode &errcode) const;
	virtual OmnString getOmnStr(AosErrCode &errcode) const;
	virtual const char *getCharStr(int &len, AosErrCode &errcode) const;
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata, AosErrCode &errcode) const;
	virtual AosXmlTagPtr getXml(AosErrCode &errcode) const;

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

	virtual AosValue *operator +(const AosValueRslt &rhs);
	virtual AosValue *operator -(const AosValueRslt &rhs);
	virtual AosValue *operator *(const AosValueRslt &rhs);
	virtual AosValue *operator /(const AosValueRslt &rhs);
	virtual bool operator ==(const AosValueRslt &rhs);
	virtual bool operator !=(const AosValueRslt &rhs);
	virtual bool operator >(const AosValueRslt &rhs);
	virtual bool operator >=(const AosValueRslt &rhs);
	virtual bool operator <(const AosValueRslt &rhs);
	virtual bool operator <=(const AosValueRslt &rhs);
	virtual bool check(const AosOpr opr, const AosValueRslt &rhs) const;
};
#endif
*/
