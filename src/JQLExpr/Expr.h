//////////////////////////////////////////////////////////////////////////// //
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
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_Expr_h
#define Aos_JQLExpr_Expr_h

#include "JQLExpr/Ptrs.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/JqlQueryWhereCond.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "SEInterfaces/ExprType.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"
#include <map>

class AosExpr : public AosExprObj
{
protected:

	map<int, AosExprObjPtr>		mExprs;
	AosExprObjPtr				mAliasExpr;

public:
	AosExpr();
	virtual ~AosExpr() {};
	//jimo interface
	virtual AosJimoPtr cloneJimo() const;

	virtual bool 	    isUnary() const ;         
	virtual bool		isHasOr() const;
	virtual bool		isJoin() const;
	virtual bool		isExprCond() const;
	virtual bool 	    isConstant() const ; 
	virtual bool 	    isFieldName() const ;     
	virtual bool		isMemberOpt() const; 
	virtual bool		isAlwaysTrueCondition() const ;
	virtual bool	 	isAlwaysFalseCondition() const ;
	virtual bool 		isAlwaysFalse() ; 
	virtual int 		getNumFieldTerms() const;  
	virtual OmnString	getExprOptStr() const{return "";};
	virtual OmnString 	dump() const;
	virtual OmnString 	dumpByNoEscape() const ;
	virtual OmnString 	dumpByStat() const ;

	virtual bool fieldExprWithAggr() const {return false;};
	static bool checkIsAgrFunc(AosExprObjPtr expr);

	virtual OmnString 	getStatFieldName();

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record);

	virtual bool 		getValue(
							AosRundata *rdata,
							const AosValueRslt &key,
							AosValueRslt &value);

	virtual bool		getValue(
							AosRundata *rdata,
							vector<AosDataRecordObj *> &records,
							AosValueRslt &value);

	virtual bool   		getFieldName(
					        const AosRundataPtr &rdata,
							vector<AosExprObjPtr> &fields) {return false;};

	virtual bool		getFields(
					        const AosRundataPtr &rdata,
							vector<AosJqlSelectFieldPtr> &fields);

	virtual bool 		getFieldsByStat(
							const AosRundataPtr &rdata,
							vector<AosJqlSelectFieldPtr> &fields);

	virtual bool 		createConds(
							const AosRundataPtr &rdata, 
							vector<AosJqlQueryWhereCondPtr>  &conds,
							const OmnString &tableName);

	virtual bool 		mergeTerms( 
							const AosRundataPtr &rdata, 
							bool &merged, 
							AosExprObjPtr &merged_term);

	virtual AosExprType::E	getType() const {return AosExprType::eInvalid;}


	// Young, 2014/11/05
	virtual bool 		setExpr(AosExprObjPtr expr, int key);
	virtual AosExprObjPtr getExpr(int key);

	bool 				setAlias(AosExprObjPtr expr);
	AosExprObjPtr 		getAlias();
	// arvin ,2015/04/15
	virtual AosExprList* getParmList() const;
	virtual OmnString getMember2() const;
	virtual OmnString getMember3() const;
	virtual vector<AosExprObjPtr> getExprList();
	virtual OmnString getName() const;
	virtual AosExprObjPtr getValueAsExpr() const;
	virtual OmnString getXpathName() const;
	virtual OmnString getFuctName() const;
	virtual OmnString dumpByNoQuote()const;
	
	// Chen Ding, 2015/05/25
	virtual AosExprObjPtr createArrayExpr(
					const vector<AosExprObjPtr> &exprs);

	virtual AosExprObjPtr createStrExpr(const OmnString &value);

	virtual AosExprObjPtr createNameValueExpr(
					const OmnString &name, 
					const AosExprObjPtr &value);

	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}

	//arvin 2015.08.17
	//JIMODB-422
	virtual AosExprObjPtr getRHS() const;
	virtual AosExprObjPtr getLHS() const;
};
#endif

