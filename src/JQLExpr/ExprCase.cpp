////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description:
// Test Case 1:
// select case when f1 = 2 then f1 else 0;
//
// create a table with "f1";
// [1]
// [2]
// [1]
//
// select 
// 	case f1 
// 		when '1' then 'male'
// 		when '2' then 'female'
// 		else 'unknown' 
// 	end "Sex"
// 	from t1;
//
//	Sex
//	---------
//	male
//	female
//	male
//	--------
//
// select f1 from t1
// where
//     (case f1 
//     	when '1' then 'male'
// 		when '2' then 'female'
// 		else 'unknown' 
// 	   end) = 'male';
//
// Modification History:
// 2013/10/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprCase.h"
//#include "Util/OmnNew.h"


AosExprCase::AosExprCase(
		const AosExprObjPtr	&search,
		const AosExprObjPtr	&simple,
		const AosExprObjPtr	&expr)
:
mSearchPtr(search),
mSearch(mSearchPtr.getPtr()),
mSimplePtr(simple),
mSimple(mSimplePtr.getPtr()),
mElsePtr(expr),
mElse(mElsePtr.getPtr())
{
	if (!search && !simple)
	{
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		AosSetErrorUser(rdata, __func__)
	    	<< "Missing case clause" << enderr;
		OmnAlarm << __func__ << enderr;
		return;
	}
}


AosExprCase::AosExprCase(const AosExprCase &rhs)
:
mSearch(0),
mSimple(0),
mElse(0)
{
	if (rhs.mSearch)
	{
		mSearchPtr = rhs.mSearch->cloneExpr();
		mSearch = mSearchPtr.getPtr();
	}

	if (rhs.mSimple)
	{
		mSimplePtr = rhs.mSimple->cloneExpr();
		mSimple = mSimplePtr.getPtr();
	}

	if (rhs.mElse)
	{
		mElsePtr = rhs.mElse->cloneExpr();
		mElse = mElsePtr.getPtr();
	}
}


AosExprCase::~AosExprCase()
{
}

AosDataType::E 
AosExprCase::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	//arvin 2015.11.05
	//JIMODB-1130
	if(mSimple)
		return mSimple->getDataType(rdata,record);
	if(mElse)
		return mElse->getDataType(rdata,record);
}


bool
AosExprCase::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	bool rslt;
	if (mSearch)
	{
		rslt= mSearch->getValue(rdata, record, value);
		aos_assert_rr(rslt, rdata, false);
		if (!value.isNull()) return true;

		if (mElse)
		{
			rslt= mElse->getValue(rdata, record, value);
			aos_assert_rr(rslt, rdata, false);
			return true;
		}
		return true;
	}

	aos_assert_rr(mSimple, rdata, false);
	rslt= mSimple->getValue(rdata, record, value);
	aos_assert_rr(rslt, rdata, false);
	if (!value.isNull()) return true;

	if (mElse)
	{
		rslt= mElse->getValue(rdata, record, value);
		aos_assert_rr(rslt, rdata, false);
		return true;
	}
	return true;
}


AosExprObjPtr
AosExprCase::cloneExpr() const                            
{
	return OmnNew AosExprCase(*this);
}


int
AosExprCase::getNumFieldTerms() const
{
	return 0;
}

	
bool		
AosExprCase::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	if (mSearch) mSearch->getFields(rdata, fields);
	if (mSimple) mSimple->getFields(rdata, fields);
	if (mElse) mElse->getFields(rdata, fields);
	return true;
}


bool		
AosExprCase::getFieldsByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	if (mSearch) mSearch->getFieldsByStat(rdata, fields);
	if (mSimple) mSimple->getFieldsByStat(rdata, fields);
	if (mElse) mElse->getFieldsByStat(rdata, fields);
	return true;
}

	
bool
AosExprCase::createConds(
		const AosRundataPtr &rdata, 
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{
	OmnShouldNeverComeHere;
	return false;
}


OmnString 
AosExprCase::dump() const
{
	OmnString str;
	str << " case ";
	if (mSearch) str << mSearch->dump();
	if (mSimple) str << mSimple->dump() << "" ;
	if (mElse)
	{
		str << " else ";
		str <<  mElse->dump();
	}
	str << " end ";
	return str;
}


OmnString 
AosExprCase::dumpByNoEscape() const
{
	OmnString str;
	str << " case ";
	if (mSearch) str << mSearch->dumpByNoEscape();
	if (mSimple) str << mSimple->dumpByNoEscape() << "" ;
	if (mElse)
	{
		str << " else ";
		str <<  mElse->dumpByNoEscape();
	}
	str << " end ";
	return str;
}

OmnString 
AosExprCase::dumpByStat() const
{
	OmnString str;
	str << " case ";
	if (mSearch) str << mSearch->dumpByStat();
	if (mSimple) str << mSimple->dumpByStat() << "" ;
	if (mElse)
	{
		str << " else ";
		str <<  mElse->dumpByStat();
	}
	str << " end ";
	return str;
}

