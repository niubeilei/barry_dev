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
// 2013/09/28 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprNumber_h
#define Aos_JQLExpr_ExprNumber_h

#include "JQLExpr/Expr.h"
#include "SEInterfaces/DataRecordObj.h"

class AosExprNumber : public AosExpr
{
protected:
	int64_t mNumber;

public:
	AosExprNumber() {};
	AosExprNumber(const int64_t number);
	virtual ~AosExprNumber() {};

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eNumber; }	
		
	virtual bool getValue(
				AosRundata *rdata,
				vector<AosDataRecordObj *> &records,
				AosValueRslt &value);

	virtual bool getValue(
				AosRundata *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value);

	virtual bool isConstant() const { return true;}
	virtual bool isUnary() const {return true;} 

	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual AosExprObjPtr cloneExpr() const;                            

	AosExprType::E getType() const {return AosExprType::eNumber;}

	int64_t getNumber() {return mNumber;}
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}
};
#endif

