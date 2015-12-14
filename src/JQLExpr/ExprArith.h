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
#ifndef Aos_JQLExpr_ExprArith_h
#define Aos_JQLExpr_ExprArith_h

#include "SEInterfaces/DataRecordObj.h"
#include "JQLExpr/ExprBinary.h"
#include "SEUtil/ArithOpr.h"

class AosExprArith : public AosExprBinary
{

public:
	enum Operator
	{
		eInvalid,

		eAdd,
		eMinus,
		eMultiply,
		eDivide,
		eMod,
		eConcat,

		eMaxEntry
	};

private:
	Operator 	mOpr;
	AosDataType mReturnType;

public:
	AosExprArith() {};
	AosExprArith(AosExprObjPtr lhs, Operator opr, AosExprObjPtr rhs);
	virtual ~AosExprArith();

	virtual AosDataType::E getDataType(
					AosRundata *rdata, 
					AosDataRecordObj *record);

	virtual bool getValue(
					AosRundata       *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool mergeTerms(
					const AosRundataPtr &rdata, 
					bool &merged, 
					AosExprObjPtr &merged_term);

	OmnString getEnumStr() const ;

	virtual OmnString dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual AosExprObjPtr cloneExpr() const;
	virtual AosExprType::E	getType() const {return AosExprType::eArith;}
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return false;}
	virtual bool fieldExprWithAggr() const {return true;};

	static AosValueRslt doArith(const ArithOpr::E opr,
					const AosDataType::E return_type,
					const AosValueRslt &lv,
					const AosValueRslt &rv);

private:
	static inline bool isValidOpr(Operator opr)
	{
		return opr > eInvalid && opr < eMaxEntry;
	}
};
#endif

