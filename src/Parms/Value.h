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
#ifndef AOS_Parms_Value_h
#define AOS_Parms_Value_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"


class AosValue
{
private:

public:
	virtual ~AosValue();

	virtual OmnString toString() const = 0;
	virtual void reset() = 0;
	virtual AosDataType::E getDataType() const = 0;
	OmnString getTypeStr() const { return AosDataType::getTypeStr(getDataType());}
	bool isStrType() const {return getDataType() == AosDataType::eString;}
	bool isXmlDoc() const {return getDataType() == AosDataType::eXmlDoc;}
	u64 getDocid() const;

	virtual void setValue(const bool vv) = 0;
	virtual void setValue(const u64 &vv) = 0;
	virtual void setValue(const int64_t &vv);
	virtual void setValue(const double &vv);
	virtual void setValue(const OmnString &vv);
	virtual void setValue(const char *data, const int len);
	virtual void setValue(const AosXmlTagPtr &vv);
	virtual void setValue(const AosQueryRsltObjPtr &vv);
	virtual void setValue(const char vv);

	virtual void setDocid(const u64 &docid);

	virtual bool 		getU64Value(u64 &value, const AosRundataPtr &rdata) const; 
	virtual u32 		getU32Value(const AosRundataPtr &rdata) const; 
	virtual int 		getIntValue(const AosRundataPtr &rdata) const; 
	virtual bool 		getBoolValue(const AosRundataPtr &rdata) const; 
	virtual int64_t		getInt64Value(const AosRundataPtr &rdata) const;
	virtual double		getDoubleValue(const AosRundataPtr &rdata) const;
	virtual char 		getChar(const AosRundataPtr &rdata) const;
	virtual AosXmlTagPtr getXmlValue(const AosRundataPtr &rdata) const; 
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata) const; 
	virtual OmnString &getStrValueRef();
	virtual OmnString getValueStr1() const;
	virtual OmnString getValueStr1(const OmnString &dft) const;
	virtual OmnString getValueStr(const OmnString &dft, bool &correct) const;
	virtual const char *getCharStr(int &len) const;
	virtual int getStrLength() const;
	virtual bool isRange(const AosValue &start, 
			const AosValue &end, 
			const AosValue &step, 
			const AosRundataPtr &rdata) const;
 	virtual bool isNull() const;

	AosValue & operator + (const AosValue &rhs);
	AosValue & operator - (const AosValue &rhs);
	AosValue & operator * (const AosValue &rhs);
	AosValue & operator / (const AosValue &rhs);

	bool operator == (const AosValue &rhs) const 	{return isEQ(rhs);}
	bool operator != (const AosValue &rhs); const 	{return isNE(rhs);}
	bool operator < (const AosValue &rhs); const 	{return isEqual(rhs);}
	bool operator <= (const AosValue &rhs); const {return isEqual(rhs);}
	bool operator > (const AosValue &rhs); const {return isEqual(rhs);}
	bool operator >= (const AosValue &rhs); const {return isEqual(rhs);}
	bool operator == (const OmnString &rhs); const {return isEqual(rhs);}

	virtual bool hasMore() = 0;
	virtual bool setCharStr1(const char *data, int len, const bool copy_flag) = 0;
	virtual bool setFamily(const char *data, const AosRundataPtr &rdata) = 0;
	virtual bool setMember(const char *data, const AosRundataPtr &rdata) = 0;
	virtual bool setMember(const char *data, const int idx, const AosRundataPtr &rdata) = 0;
	virtual bool setKeyValue(
			const char *key, 
			const int key_len,
			const bool copy_flag,
			const u64 &docid) = 0;
	virtual bool setKeyValue(
			const OmnString &key, 
			const bool copy_flag,
			const u64 &docid) = 0;
	virtual bool setKeyValue(const u64 &key, const u64 &docid) = 0;
};

#endif

