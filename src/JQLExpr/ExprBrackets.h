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
#ifndef Aos_JQLExpr_ExprBrackets_h
#define Aos_JQLExpr_ExprBrackets_h

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprBracketsObj.h"
#include "JQLExpr/Expr.h"

class AosExprBrackets : public AosExpr,
						public AosExprBracketsObj
{

private:
	AosExprObjPtr 	mExprPtr;
	AosExprObj		*mExpr;

	vector<AosExprObjPtr>	mExprList;

public:
	AosExprBrackets(const AosExprObjPtr &expr);
	AosExprBrackets(const vector<AosExprObjPtr> &expr_list);

	virtual ~AosExprBrackets();

	// AosJimo interface
	virtual AosJimoPtr cloneJimo() const;

	// AosExprBracketsObj interface
	virtual vector<AosExprObjPtr> getExprList(AosRundata *rdata)
	{
		if(mExprList.size() <= 0 && mExpr)
			mExprList.push_back(mExpr);
		return mExprList;
	}

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

	virtual bool createConds(
					const AosRundataPtr &rdata, 
					vector<AosJqlQueryWhereCondPtr> &conds,
					const OmnString &tableName)
	{
		return mExpr->createConds(rdata, conds, tableName);
	}

	virtual bool getFields(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields)
	{
		return mExpr->getFields(rdata, fields);
	}

	virtual bool getFieldsByStat(
					const AosRundataPtr &rdata,
					vector<AosJqlSelectFieldPtr> &fields)
	{
		return mExpr->getFieldsByStat(rdata, fields);
	}

	virtual OmnString dump() const ;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;
	virtual OmnString 	getStatFieldName() {return mExpr->getStatFieldName();};

	virtual AosExprObjPtr cloneExpr() const;

	// Chen Ding, 2015/04/13
	// virtual AosExprType::E	getType() const {return mExpr->getType();}
	virtual AosExprType::E	getType() const {return AosExprType::eBrackets;}
	
	virtual bool fieldExprWithAggr() const ;
	virtual int getNumFieldTerms() const;
	virtual bool 	isUnary() const {return mExpr->isUnary();}
	virtual bool	isHasOr() const {return mExpr->isHasOr();};
	virtual bool	isJoin() const {return mExpr->isJoin();};
	virtual bool 	isConstant() const {return mExpr->isConstant();}
	virtual bool 	isFieldName() const {return mExpr->isFieldName();}
	virtual bool 	isMemberOpt() const {return mExpr->isMemberOpt();}; 
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}

	virtual vector<AosExprObjPtr> getExprList() 
	{
		if(mExprList.size() <= 0 && mExpr)
			mExprList.push_back(mExpr);
		return mExprList;
	}
};
#endif

