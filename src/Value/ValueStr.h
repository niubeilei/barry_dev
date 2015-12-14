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
// 2013/12/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Value_ValueStr_h
#define AOS_Value_ValueStr_h

#include "Value/Value.h"

class AosValueStr : public AosValue
{
	OmnDefineRCObject;

private:
	OmnString	mValue;

public:
	AosValueStr(const u32 version)
	:
	AosValue(AosDataType::eString, version),
	mValue(0)
	{
	}

	AosValueStr(const char *data, const int len)
	:
	AosValue(AosDataType::eString, 0)
	{
		// mValue.assign(data, len);
	}

	virtual ~AosValueStr();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// Value Interface
	virtual OmnString toString() const;

	virtual bool 	setU64(const u64 vv);
	virtual bool 	setU32(const u32 vv);
	virtual bool 	setInt64(const int64_t vv);
	virtual bool 	setInt(const int vv);
	virtual bool 	setDouble(const double vv);
	virtual bool 	setStr(const OmnString vv);
	virtual bool 	setCharStr(const char *data, const int len);
	virtual bool 	setXml(const AosXmlTagPtr &vv);
	virtual bool 	setBuff(const AosBuffPtr &buff);

	virtual bool	getU64(u64 &value) const;
	virtual bool 	getU32(u32 &value) const;
	virtual bool	getInt64(int64_t &value) const;
	virtual bool	getInt(int &value) const;
	virtual bool	getDouble(double &value) const;
	virtual bool	getStr(OmnString &value) const;
	virtual bool	getCharStr(char *&data, int &len, const bool need_copy) const;
	virtual bool	getXml(AosXmlTagPtr &value) const;
	virtual bool	getBuff(AosBuffPtr &buff) const;
};
#endif
#endif
