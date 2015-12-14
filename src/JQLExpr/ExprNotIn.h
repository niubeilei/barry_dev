////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Modification History:
// 2014/12/02 Created by Barry Niu 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprNotIn_h
#define Aos_JQLExpr_ExprNotIn_h

#include "JQLExpr/Expr.h"

#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/ValueRslt.h"

class AosExprNotIn : public AosExpr
{
protected:
	AosExprObjPtr 			mFieldPtr;
	AosExprObj				*mField;

	AosExprList 			*mValuesPtr;
	vector<AosExprObj*>		mValues;

public:
	AosExprNotIn();
	AosExprNotIn(AosExprObjPtr field, AosExprList *value_list);
	virtual ~AosExprNotIn();

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eBool; }

	virtual bool	 getValue(
						AosRundata *rdata, 
						AosDataRecordObj *record,
						AosValueRslt &value);

	virtual bool 	getValue(
						AosRundata *rdata, 
						const AosValueRslt &key, 
						AosValueRslt &value);

	virtual bool createConds(
					const AosRundataPtr& rdata,
					vector<AosJqlQueryWhereCondPtr> &conds,
					const OmnString &tableName);

	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	virtual AosExprObjPtr cloneExpr() const;
	AosExprType::E getType() const {return AosExprType::eNotIn;}

};
#endif

