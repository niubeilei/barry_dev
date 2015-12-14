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
// 2015/05/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_NameValueDoc_h
#define Aos_Util_NameValueDoc_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "JimoAPI/JimoExpr.h"
#include "Rundata/Rundata.h" 
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ExprObj.h"


class AosNameValueDoc 
{
public:
	enum Type
	{
		eNullDoc,

		eStrValueDoc,
		eTupleDoc,
		eArrayDoc,
		eDoc           
	};

private:
	enum DocValueType
	{
		eInvalidValue,

		eStrValue = eStrValueDoc,  //name:value  
		eTupleValue = eTupleDoc,
		eArrayValue = eArrayDoc,
		eDocValue = eDoc
	};

	Type					mType;
	AosBuff					mBuff;		// Save values for eDoc
	OmnString				mStrValue;	// Save values for eStrValueDoc
	vector<AosNameValueDoc>	mDocValues;	// Save values for eTupleDoc or eArrayDoc
	u64						mPoison;

public:
	AosNameValueDoc();
	AosNameValueDoc(const Type type);
	AosNameValueDoc(
			const Type type,
			const char *data, 
			const int len);
	AosNameValueDoc(AosBuff &buff);
	AosNameValueDoc(const AosNameValueDoc &rhs);
	
	~AosNameValueDoc();

	bool clear();
	void reset();
	bool hasMore();
	u32 size()const { return mDocValues.size();}; //Not implement
	bool setType(Type type) 
	{
		mType = type;
		return true;
	};
	
	bool nextField(
			AosRundata* rdata,
			AosExprObjPtr &expr);
	
	// bool appendField(const OmnString &name, const OmnString &value);
	bool appendField(const OmnString &name, const AosNameValueDoc &doc);
	bool appendValue(const AosNameValueDoc &doc);
	bool setStrValue(const OmnString &value);
	bool serialize(AosBuff &buff) const;
	
	OmnString getValue(
			const OmnString &name, 
			const OmnString &dft,
			AosBuff &buff);

	bool getValue(
			const OmnString &name, 
			AosNameValueDoc &docs, 
			bool &found);

	bool parseArrayValue(AosBuff &buff);

	OmnString dump();

	Type getType() const{return mType;};

private:
	bool setBuff(const AosBuff& buff)
	{
		mBuff.appendBuff(buff);
		return true;
	}

	bool initValue(AosBuff &buff);

	bool skipValue(
			const u8 type,
			AosBuff &buff);
	
	AosExprObjPtr convertToExpr(
			AosRundata *rdata, 
			const OmnString &name,
			AosBuff &buff);

	bool dump(OmnString &str, const int level);
	bool dumpTupleDoc(OmnString &str, const int level);
	bool dumpArrayDoc(OmnString &str, const int level);
	bool dumpNormalDoc(OmnString &str, const int level);
	bool dumpDoc(OmnString &str, const int level);
	bool dumpValue(OmnString &str, const int level, const bool with_comma);
};

#endif

