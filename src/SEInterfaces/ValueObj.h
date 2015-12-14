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
// 2013/12/31 Created by Phil
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Value_ValueObj_h
#define AOS_Value_ValueObj_h

#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/DataTypes.h"
#include "Util/Ptrs.h"
#include "Value/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosValueObj : virtual public AosJimo
{
protected:
	bool			mIsNull;
	AosDataType::E	mDataType;

public:
	AosValueObj(const AosDataType::E type, const int version);
	~AosValueObj();

	inline bool isStrType() const {return mDataType == AosDataType::eString;}
	inline bool isXmlDoc() const {return mDataType == AosDataType::eXmlDoc;}
	inline bool isNull() const {return mIsNull;}
	inline void setNull(bool b) {mIsNull = b;}
	inline AosDataType::E getDataType() const {return mDataType;}

	// ValueObj Interface
	virtual OmnString toString() const = 0;

	virtual bool 	setU64(const u64 vv) = 0;
	virtual bool 	setU32(const u32 vv) = 0;
	virtual bool 	setInt64(const int64_t vv) = 0;
	virtual bool 	setInt(const int vv) = 0;
	virtual bool 	setDouble(const double vv) = 0;
	virtual bool 	setStr(const OmnString vv) = 0;
	virtual bool 	setCharStr(const char *data, const int len) = 0;
	virtual bool 	setXml(const AosXmlTagPtr &vv) = 0;
	virtual bool 	setBuff(const AosBuffPtr &buff) = 0;

	virtual bool	getU64(u64 &value) const = 0;
	virtual bool 	getU32(u32 &value) const = 0;
	virtual bool	getInt64(int64_t &value) const = 0;
	virtual bool	getInt(int &value) const = 0;
	virtual bool	getDouble(double &value) const = 0;
	virtual bool	getStr(OmnString &value) const = 0;
	virtual bool	getXml(AosXmlTagPtr &value) const = 0;
	virtual bool	getBuff(AosBuffPtr &buff) const = 0;

	static AosValueObjPtr createValue(
				AosRundata *rdata, 
				const AosXmlTagPtr &worker_doc);
};
#endif

