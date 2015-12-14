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
// 2013/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ExprObj_h
#define Aos_SEInterfaces_ExprObj_h

#include "JQLStatement/Ptrs.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprType.h"
#include "SEInterfaces/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"
#include "Rundata/Ptrs.h"
#include "Jimo/Jimo.h"
#include <map>


typedef std::vector<AosExprObjPtr> AosExprList;

class AosExprObj : virtual public AosJimo
{
	// !!!!!!!!!! Important: subclasses should not contain 
	// this statement.
	// !!!!!!!!!!
	OmnDefineRCObject;

private:
	static AosExprObj* 		smExpr;
	static AosExprObjPtr 	smExprPtr;

public:

	virtual AosDataType::E getDataType(
						AosRundata *rdata, 
						AosDataRecordObj *record) = 0;
	virtual OmnString getValue(AosRundata* rdata);

	virtual bool 	getValue(
						AosRundata *rdata,
						AosDataRecordObj *record,
						AosValueRslt &value) = 0;

	virtual bool 	getValue(
						AosRundata *rdata,
						const AosValueRslt &key,
						AosValueRslt &value) = 0;

	virtual bool	getValue(
						AosRundata *rdata,
						vector<AosDataRecordObj *> &records,
						AosValueRslt &value) = 0;

	virtual OmnString 	getStatFieldName()  = 0;

	virtual bool	setIsParsedFieldIdx(bool prasedFieldIdx) = 0;  

	virtual bool 	getFields(
						const AosRundataPtr &rdata,
						vector<AosJqlSelectFieldPtr> &field) = 0;

	virtual bool 	getFieldsByStat(
						const AosRundataPtr &rdata,
						vector<AosJqlSelectFieldPtr> &fields) = 0;

	virtual bool 	createConds(
						const AosRundataPtr &rdata, 
						vector<AosJqlQueryWhereCondPtr> &conds,
						const OmnString &tableName) = 0;

	virtual bool 	mergeTerms(
						const AosRundataPtr &rdata,
						bool &merged,
						AosExprObjPtr &merged_term) = 0;

	virtual bool 	isUnary() const = 0; 
	virtual bool	isHasOr() const = 0;
	virtual bool	isExprCond() const = 0;
	virtual bool	isJoin() const  = 0;
	virtual bool 	isConstant() const = 0;
	virtual bool 	isFieldName() const = 0;
	virtual bool 	isMemberOpt() const = 0; 
	virtual bool 	isAlwaysFalse() = 0; 
	virtual int 	getNumFieldTerms() const = 0;
	virtual bool 	fieldExprWithAggr() const = 0;
	virtual bool 	isAlwaysTrueCondition() const = 0;
	virtual bool 	isAlwaysFalseCondition() const = 0;

	virtual AosExprType::E getType() const = 0;
	virtual AosExprObjPtr cloneExpr() const = 0; 

	virtual OmnString dump() const = 0;
	virtual OmnString dumpByNoEscape() const = 0;
	virtual OmnString dumpByStat() const = 0;
	virtual OmnString dumpByNoQuote()const = 0;

	virtual bool    setExpr(AosExprObjPtr expr, int key) = 0;
	virtual AosExprObjPtr getExpr(int key) = 0;
	virtual OmnString getXpathName() const = 0;
	virtual OmnString getFuctName() const = 0;
	virtual AosExprList* getParmList() const = 0;
	virtual OmnString getMember2() const = 0;
	virtual OmnString getMember3() const = 0;
	virtual vector<AosExprObjPtr> getExprList() = 0;
	virtual OmnString getName() const = 0;
	virtual AosExprObjPtr getValueAsExpr() const = 0;

	// Chen Ding, 2015/05/25
	virtual AosExprObjPtr createNameValueExpr(
					const OmnString &name, 
					const AosExprObjPtr &value) = 0;

	virtual AosExprObjPtr createArrayExpr(
					const vector<AosExprObjPtr> &exprs) = 0;

	virtual AosExprObjPtr createStrExpr(const OmnString &value) = 0;

	static AosExprObj* getExprStatic() {return smExpr;}
	static void setExprStatic(const AosExprObjPtr &expr) {smExprPtr = expr; smExpr = expr.getPtr();}

	//arvin ,2015.08.16
	//JIMODB-422
	virtual AosExprObjPtr getRHS() const = 0;
	virtual AosExprObjPtr getLHS() const = 0;
};
#endif

