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
#ifndef Aos_JQLExpr_ExprLogic_h
#define Aos_JQLExpr_ExprLogic_h

#include "SEInterfaces/DataRecordObj.h"
#include "JQLExpr/ExprBinary.h"

class AosExprLogic : public AosExprBinary
{
public:
	enum Operator
	{
		eInvalid,

		eAnd,
		eOr,

		eMaxEntry
	};

private:
	Operator 	mOpr;

public:
	AosExprLogic() {};
	AosExprLogic(AosExprObjPtr lhs, Operator opr, AosExprObjPtr rhs);
	virtual ~AosExprLogic(); 

	virtual bool	getValue(
						AosRundata *rdata,
						vector<AosDataRecordObj *> &records,
						AosValueRslt &value);

	virtual bool 	getValue(
						AosRundata       *rdata, 
						AosDataRecordObj *record,
						AosValueRslt &value);

	virtual bool 	getValue(
						AosRundata  *rdata, 
						const AosValueRslt &key,
						AosValueRslt &value);

	virtual bool 	moveTerms(               
						const AosRundataPtr &rdata, 
						bool &moved,
						AosExprObjPtr &term);

	virtual	bool 	createConds(              
						const AosRundataPtr &rdata,
						vector<AosJqlQueryWhereCondPtr> &conds,
						const OmnString &tableName); 

	virtual bool isHasOr() const;
	virtual AosExprObjPtr cloneExpr() const;
	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	AosExprType::E getType() const {return AosExprType::eLogic;}
	OmnString getEnumStr();

private:
	static inline bool isValidOpr(Operator opr)
	{
		return opr > eInvalid && opr < eMaxEntry;
	}
};

#endif
