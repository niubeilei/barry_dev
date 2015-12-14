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
// 2013/09/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JQLExpr_ExprComparison_h
#define Aos_JQLExpr_ExprComparison_h

#include "JQLExpr/ExprBinary.h"
#include "Util/ValueRslt.h"
#include "Util/Value.h"

class AosExprComparison : public AosExprBinary
{
public:
	enum Operator
	{
		eInvalid,

		eEqual,
		eNotEqual,
		eLarger,
		eLess,
		eLargerEqual,
		eLessEqual,

		eMaxEntry
	};


protected:
	Operator 		mOpr;

public:
	AosExprComparison(){} 
	AosExprComparison(AosExprObjPtr lhs, Operator opr, AosExprObjPtr rhs);
	~AosExprComparison();

	virtual AosDataType::E getDataType(
			AosRundata *rdata, 
			AosDataRecordObj *record);

	virtual bool getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> &records,
		AosValueRslt &value);

	virtual bool getValue(
				AosRundata       *rdata, 
				AosDataRecordObj *record,
				AosValueRslt &value);

	virtual bool getValue(
				AosRundata  *rdata, 
				const AosValueRslt &key,
				AosValueRslt &value);

	//virtual bool getValue(
	//			AosRundata *rdata,
	//			vector<AosDataRecordObj *> &records,
	//			AosValueRslt &value);

	virtual	bool createConds(
				const AosRundataPtr &rdata,
				vector<AosJqlQueryWhereCondPtr> &conds,
				const OmnString &tableName);

	virtual bool		isJoin() const;
	virtual bool 		isAlwaysFalse() ;

	virtual OmnString   getExprOptStr() const;
	virtual OmnString   getEnumStr() const;
	virtual OmnString 	dump() const;
	virtual OmnString 	dumpByNoEscape() const ;

	virtual OmnString 	dumpByStat() const ;

	AosExprType::E getType() const {return AosExprType::eComparison;}
	AosExprObjPtr cloneExpr() const;
	
	virtual bool setIsParsedFieldIdx(bool prasedFieldIdx) {return prasedFieldIdx;}

private:
	static inline bool isValidOpr(Operator opr)
	{
		return opr > eInvalid && opr < eMaxEntry;
	}

};
#endif

