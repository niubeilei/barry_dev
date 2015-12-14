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
#ifndef Aos_JQLExpr_ExprLike_h
#define Aos_JQLExpr_ExprLike_h

#include "SEInterfaces/DataRecordObj.h"
#include "JQLExpr/ExprBinary.h"

#include <boost/regex.hpp>

using boost::regex;

class AosExprLike : public AosExprBinary
{
private:
	boost::regex 	mReg;
	bool			mNotLike;	

public:
	AosExprLike(){};
	AosExprLike(AosExprObjPtr lhs, AosExprObjPtr rhs, const bool not_like = false);
	virtual ~AosExprLike(); 

	virtual AosDataType::E getDataType(AosRundata *rdata, AosDataRecordObj *record) { return AosDataType::eBool; }

	virtual bool 	getValue(
						AosRundata       *rdata, 
						AosDataRecordObj *record,
						AosValueRslt &value);

	virtual bool 	getValue(
						AosRundata  *rdata, 
						const AosValueRslt &key,
						AosValueRslt &value);

	virtual bool 	getValue(
						AosRundata *rdata,
						vector<AosDataRecordObj *> &records,
						AosValueRslt &value);

	virtual bool 	moveTerms(               
						const AosRundataPtr &rdata, 
						bool &moved,
						AosExprObjPtr &term);

	virtual	bool 	createConds(              
						const AosRundataPtr &rdata,
						vector<AosJqlQueryWhereCondPtr> &conds,
						const OmnString &tableName); 

	virtual AosExprObjPtr cloneExpr() const;
	virtual OmnString dump() const;
	virtual OmnString dumpByNoEscape() const ;
	virtual OmnString dumpByStat() const ;

	AosExprType::E getType() const {return AosExprType::eLike;}
	OmnString convertPatternFromSqlToBoost(const OmnString str) const;
};
#endif

