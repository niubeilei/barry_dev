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
//
// Modification History:
// 2015/01/21 Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtIF.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"

#include "JQLParser/JQLParser.h"                     
using namespace std;                                 
extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosJqlStmtIF::AosJqlStmtIF(const OmnString &errmsg)
:
mIsNot(false),
mErrmsg(errmsg)
{
}

AosJqlStmtIF::AosJqlStmtIF()
{
}

AosJqlStmtIF::~AosJqlStmtIF()
{
}


bool
AosJqlStmtIF::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eRun) return execution(rdata);
	AosSetEntityError(rdata, "JQL_schema_run_err", "JqlStatemnt IF ", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool 
AosJqlStmtIF::execution(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosValueRslt vv;
	bool rslt = mCond->getValue(rdata.getPtr(), NULL, vv);
	bool cond = vv.getBool();
	if (mIsNot) cond = !cond; 
	if (cond)
	{
		for (u32 i = 0; i < mThen.size(); i++)
		{
			rslt = mThen[i]->run(rdata, 0);
			if (!rslt)
			{
				AosSetErrorUser(rdata,__func__)
					<< "Field to run statement."
					<< enderr;
				return false;
			}
		}
	}
	else
	{
		for (u32 i = 0; i < mElse.size(); i++)
		{
			rslt = mElse[i]->run(rdata, 0);
			if (!rslt)
			{
				AosSetErrorUser(rdata,__func__)
					<< "Field to run statement."
					<< enderr;
				return false;
			}
		}
	}

	msg = "Run IF statement successfully.";
	rdata->setJqlMsg(msg);
	return true;
}

void
AosJqlStmtIF::setNot(bool flag)
{
	mIsNot = flag;
}

void 
AosJqlStmtIF::setCond(const AosExprObjPtr &cond)
{
	aos_assert(cond);
	mCond = cond;
}

void 
AosJqlStmtIF::setThen(AosJqlStatementList *then)
{
	mThen.clear();
	if (then)
	{
		mThen.insert(mThen.end(), then->begin(), then->end());
	}
}

void 
AosJqlStmtIF::setElse(AosJqlStatementList *elsex)
{
	mElse.clear();
	if (elsex)
	{
		mElse.insert(mElse.end(), elsex->begin(), elsex->end());
	}
}

AosJqlStatement *
AosJqlStmtIF::clone()
{
	return OmnNew AosJqlStmtIF(*this);
}


void 
AosJqlStmtIF::dump()
{
}


