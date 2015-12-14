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
#ifndef Aos_JQLExpr_ExprDouble_h
#define Aos_JQLExpr_ExprDouble_h

#include "JQLExpr/Expr.h"
#include "Util/ValueRslt.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"

class AosExprDouble : public AosExpr
{
protected:
	double mNumber;

public:
	AosExprDouble() {};
	AosExprDouble(double number);
	virtual ~AosExprDouble() {};

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eNumber; }

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual AosExprObjPtr cloneExpr() const;
	virtual bool isUnary() const {return true;}
	virtual bool isConstant() const {return true;} 
	AosExprType::E getType() const {return AosExprType::eDouble;}
};
#endif

