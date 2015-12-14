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
#ifndef Aos_JQLExpr_ExprSearchCase_h
#define Aos_JQLExpr_ExprSearchCase_h

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprSimpleCase.h"

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprSearchCase : public AosExpr
{
protected:
	//vector<AosExprObjPtr> 		mExprs;
	vector<AosExprSimpleCase::AosJqlWhenCaseExpr*> 	mExprs;

public:
	AosExprSearchCase(const AosExprSimpleCasePtr &simple);
	AosExprSearchCase(const AosExprSearchCase &rhs);
	virtual ~AosExprSearchCase();

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual AosExprObjPtr cloneExpr() const;

	virtual bool getFields( 
					const AosRundataPtr &rdata, 
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool getFieldsByStat( 
					const AosRundataPtr &rdata, 
					vector<AosJqlSelectFieldPtr> &fields);

	virtual OmnString dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}
};
#endif

