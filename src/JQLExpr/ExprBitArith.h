////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2013/09/25 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprBitArith_h
#define Aos_JQLExpr_ExprBitArith_h

#include "SEInterfaces/DataRecordObj.h"
#include "JQLExpr/ExprBinary.h"

class AosExprBitArith : public AosExprBinary
{
public:
	enum Operator
	{
		eInvalid,

		eBitAnd,
		eBitOr,
		eBitXor,

		eMaxEntry
	};

private:
	Operator 	mOpr;

public:
	AosExprBitArith() {};
	AosExprBitArith(AosExprObjPtr lhs, Operator opr, AosExprObjPtr rhs);
	virtual ~AosExprBitArith(); 

	virtual bool getValue(
				AosRundata       *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value);
	
	virtual AosExprObjPtr cloneExpr() const;
	virtual AosExprType::E	getType() const {return AosExprType::eBitArith;}
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return  prasedFieldIdx;}

private:
	static inline bool isValidOpr(Operator opr)
	{
		return opr > eInvalid && opr < eMaxEntry;
	}
};
#endif

