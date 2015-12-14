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
// This is a utility to select docs.
//
// Modification History:
// 04/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_ValueRslt_h
#define AOS_Util_ValueRslt_h

#include "MultiLang/LangTermIds.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/ValueDefs.h"
#include "Util/DateTime.h"			// Young 2015/06/17
#include "Util/TimeDuration.h"		// Young 2015/06/17
#include "Util/Opr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DataTypes.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"
#include "SEUtil/ArithOpr.h"


class AosValueRslt 
{
private:
	union DataValue
	{
		int					char_value;
		bool 				bool_value;
		i64 				i64_value;
		u64 				u64_value;
		double 				double_value;
		char				str_value[24];
		void				*obj_value;
	};

	enum E
	{
		eDefaultSize = 24 
	};

private:
	DataValue			mValue;
	AosDataType::E		mType;

public:
	AosValueRslt();

	AosValueRslt(const AosValueRslt &rhs);
	explicit AosValueRslt(const bool value);
	explicit AosValueRslt(const u64 value);
	explicit AosValueRslt(const i64 value);
	explicit AosValueRslt(const double value);
	explicit AosValueRslt(const OmnString &value);
	explicit AosValueRslt(const char value);
	explicit AosValueRslt(const AosDateTime &value);
	explicit AosValueRslt(const AosTimeDuration *value);

	~AosValueRslt();

	void reset();
	AosDataType::E getType() const;

	void setNull() {mType = AosDataType::eNull;}
	bool isNull() const {return mType==AosDataType::eNull;}
	static inline OmnString getNullStr() {return "NULL";}

	// Set Value
	void setDouble(const double vv);
	void setU64(const u64 vv);
	void setI64(const i64 vv);
	void setStr(const OmnString &vv);
	void setCStr(const char *vv, int len);
	void setDateTime(const AosDateTime &dt);
	void setBool(const bool vv);
	void setChar(const char vv);
	void setBuff(const AosBuffPtr &buff);

	double 			getDouble() const;
	u64 			getU64() const;
	i64 			getI64() const;
	OmnString 		getStr() const;
	const AosDateTime& 	getDateTime() const;
	bool			getBool() const;
	char			getChar() const;
	AosBuffPtr		getBuff() const;
	char* getCStr(char *dest, int dest_len, int &len);

	AosValueRslt & operator = (const AosValueRslt &rhs);

	bool operator < (const AosValueRslt &rhs) const ;

	static AosValueRslt doArith(
			const ArithOpr::E opr,
			const AosDataType::E return_type,
			const AosValueRslt &lv,
			const AosValueRslt &rv);              

	static bool doComparison(
			const AosOpr opr,
			const AosDataType::E value_type,
			const AosValueRslt &lv,
			const AosValueRslt &rv) ;

private:
	OmnString 	getStrValue() const ;
	const char* getCStrValue() const ;
	void 		deleteMemory();
	
};

#endif

