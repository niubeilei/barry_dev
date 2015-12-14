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
#ifndef Aos_JQLExpr_ExprSimpleCase_h
#define Aos_JQLExpr_ExprSimpleCase_h

#include "JQLExpr/Expr.h"
#include "JQLExpr/Ptrs.h"

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprSimpleCase : public AosExpr
{
public:
	class AosJqlWhenCaseExpr : public OmnRCObject
	{
		OmnDefineRCObject;
		
	public:
		AosExprObjPtr mWhenExpr;
		AosExprObjPtr mThenExpr;

	public:
		AosJqlWhenCaseExpr(const AosExprObjPtr &expr1, const AosExprObjPtr &expr2)
		:
		mWhenExpr(expr1),
		mThenExpr(expr2)
		{}

		AosJqlWhenCaseExpr(const AosJqlWhenCaseExpr &rhs)
		{
			if (rhs.mWhenExpr)
				mWhenExpr = rhs.mWhenExpr->cloneExpr();
			if (rhs.mThenExpr)
				mThenExpr = rhs.mThenExpr->cloneExpr();
		}

		AosJqlWhenCaseExpr(){}
		~ AosJqlWhenCaseExpr(){}

		AosJqlWhenCaseExpr* clone()
		{
			return OmnNew AosJqlWhenCaseExpr(*this);
		}
	};

protected:
	AosExprObjPtr				mExpr;
	//vector<AosExprObjPtr>		mExprs;
	vector<AosJqlWhenCaseExpr*>	mExprs;

public:

	AosExprSimpleCase();
	AosExprSimpleCase(const AosExprSimpleCasePtr &rhs);
	virtual ~AosExprSimpleCase();

	bool setCaseExpr(const AosExprObjPtr &expr);
	//bool appendPair(const AosExprObjPtr &expr1, const AosExprObjPtr &expr2);
	bool appendWhenCase(AosJqlWhenCaseExpr * whencase);
	vector<AosJqlWhenCaseExpr*> getExprs() const {return mExprs;}

	virtual AosExprObjPtr cloneExpr() const;
	virtual OmnString 	dump() const ;
	virtual OmnString 	dumpByNoEscape() const ;
	virtual OmnString 	dumpByStat() const ;

	virtual bool getValue(
					AosRundata *rdata, 
					AosDataRecordObj *record,
					AosValueRslt &value);

	virtual bool getFields( const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool getFieldsByStat( const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields);

	virtual bool createConds(
					const AosRundataPtr &rdata, 
					vector<AosJqlQueryWhereCondPtr>  &conds,
					const OmnString &tableName);

	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}
};
#endif

