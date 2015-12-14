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
// 2014/12/14 Created by Cheng Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Value_ValueImp_h
#define AOS_Value_ValueImp_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Opr.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include "Value/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosValue;

class AosValueImp : public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosValueImp() 
	{
	}

	virtual ~AosValueImp();

	virtual double 	getDouble() const = 0;
	virtual i8 		getInt8() const = 0;
	virtual i16 	getInt16() const = 0;
	virtual u8 		getU8() const = 0;
	virtual u16 	getU16() const = 0;
	virtual u64     getU64() const = 0;
	virtual float	getFloat() const = 0;
	virtual u32 	getU32() const = 0;
	virtual int		getInt() const = 0;
	virtual i64 	getInt64() const = 0;
	virtual char 	getChar() const = 0;
	virtual bool 	getBool() const = 0;
	virtual OmnString getOmnStr() const = 0;
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata) const = 0;
	virtual const char *getCharStr(int &len) const = 0;
	virtual AosXmlTagPtr getXml() const = 0;

	virtual bool 	setU8(const u8 vv) = 0;
	virtual bool 	setU16(const u16 vv) = 0;
	virtual bool 	setInt8(const i8 vv) = 0;
	virtual bool 	setInt16(const i16 vv) = 0;
	virtual bool 	setFloat(const float vv) = 0;
	virtual bool 	setChar(const char vv) = 0;
	virtual bool 	setU64(const u64 vv) = 0;
	virtual bool 	setU32(const u32 vv) = 0;
	virtual bool 	setInt64(const i64 vv) = 0;
	virtual bool 	setInt(const int vv) = 0;
	virtual bool 	setDouble(const double vv) = 0;
	virtual bool 	setOmnStr(const OmnString &vv) = 0;
	virtual bool 	setCharStr(const char *data, const int len, const bool copy_flag) = 0;
	virtual bool 	setXml(const AosXmlTagPtr &vv) = 0;
	virtual bool 	setBuff(const AosBuffPtr &buff) = 0;
	virtual OmnString	toString(const bool is_null) const = 0;

	virtual AosValueImp &add(const AosValue &rhs) = 0;
	virtual AosValueImp &minus(const AosValue &rhs) = 0;
	virtual AosValueImp &multiply(const AosValue &rhs) = 0;
	virtual AosValueImp &divide(const AosValue &rhs) = 0;

	virtual bool checkEQ(const AosValue &rhs) = 0;
	virtual bool checkNE(const AosValue &rhs) = 0;
	virtual bool checkGT(const AosValue &rhs) = 0;
	virtual bool checkGE(const AosValue &rhs) = 0;
	virtual bool checkLT(const AosValue &rhs) = 0;
	virtual bool checkLE(const AosValue &rhs) = 0;

	virtual AosValueImpPtr clone() = 0;
};
#endif

