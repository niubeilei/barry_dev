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
#ifndef AOS_Parms_ValueU64_h
#define AOS_Parms_ValueU64_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/DataTypes.h"
#include "XmlUtil/Ptrs.h"


class AosValueU64
{
private:

public:
	virtual ~AosValueU64();

	virtual OmnString toString() const = 0;
	virtual void reset() = 0;
	virtual void setValue(const bool vv) = 0;
	virtual void setValue(const u64 &vv) = 0;
	void setValue(const int64_t &vv);
	void setValue(const double &vv);
	void setValue(const OmnString &vv);
	void setValue(const char *data, const int len);
	void setValue(const AosXmlTagPtr &vv);
	void setValue(const AosQueryRsltObjPtr &vv);
	void setValue(const char vv);
	void setDocid(const u64 &docid);
	virtual AosDataType::E getDataType() const = 0;
	OmnString getTypeStr() const { return AosDataType::getTypeStr(getDataType());}
	bool isStrType() const {return getDataType() == AosDataType::eString;}
	bool isXmlDoc() const {return getDataType() == AosDataType::eXmlDoc;}
	u64 getDocid() const;

	virtual bool 		getU64Value(u64 &value, const AosRundataPtr &rdata) const; 
	virtual u32 		getU32Value(const AosRundataPtr &rdata) const; 
	virtual int 		getIntValue(const AosRundataPtr &rdata) const; 
	virtual bool 		getBoolValue(const AosRundataPtr &rdata) const; 
	virtual int64_t		getInt64Value(const AosRundataPtr &rdata) const;
	virtual double		getDoubleValue(const AosRundataPtr &rdata) const;
	virtual char getChar(const AosRundataPtr &rdata) const;
	virtual AosXmlTagPtr getXmlValue(const AosRundataPtr &rdata) const; 
	virtual AosQueryRsltObjPtr getQueryRslt(const AosRundataPtr &rdata) const; 
	virtual OmnString &getStrValueRef();
	virtual OmnString getValueStr1() const;
	virtual OmnString getValueStr1(const OmnString &dft) const;
	virtual OmnString getValueStr(const OmnString &dft, bool &correct) const;
	const char *getCharStr(int &len) const;
	int getStrLength() const;
	bool isRange(const AosValue &start, 
			const AosValue &end, 
			const AosValue &step, 
			const AosRundataPtr &rdata) const;
 	bool isNull() const;

	// 2011/06/22 Lynch
	AosValue & operator + (const AosValue &rhs);
	AosValue & operator - (const AosValue &rhs);
	AosValue & operator * (const AosValue &rhs);
	AosValue & operator / (const AosValue &rhs);

	// 06/24/2011 Brian Zhang
	bool operator == (const AosValue &rhs);
	bool operator != (const AosValue &rhs);
	bool operator < (const AosValue &rhs);
	bool operator <= (const AosValue &rhs);
	bool operator > (const AosValue &rhs);
	bool operator >= (const AosValue &rhs);

	bool operator == (const OmnString &rhs);
	bool hasMore();
	static OmnString getStringDataType() {return AOSDATATYPE_STRING;}
	bool setCharStr1(const char *data, int len, const bool copy_flag);
	bool setFamily(const char *data, const AosRundataPtr &rdata);
	bool setMember(const char *data, const AosRundataPtr &rdata);
	bool setMember(const char *data, const int idx, const AosRundataPtr &rdata);
	bool setKeyValue(
			const char *key, 
			const int key_len,
			const bool copy_flag,
			const u64 &docid);
	inline bool setKeyValue(
			const OmnString &key, 
			const bool copy_flag,
			const u64 &docid);
	bool setKeyValue(const u64 &key, const u64 &docid);
};

#endif

