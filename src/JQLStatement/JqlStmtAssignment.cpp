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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtAssignment.h"

#include "Debug/Debug.h"
#include "JQLParser/JQLParser.h"
#include "JQLExpr/ExprUserVar.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"

extern AosJQLParser gAosJQLParser;

AosJqlStmtAssignment::AosJqlStmtAssignment()
:
mIsParms(false),
mParmsIdx(-1)
{
}


AosJqlStmtAssignment::AosJqlStmtAssignment(const AosJqlStmtAssignment &rhs)
:
mIsParms(rhs.mIsParms),
mParmsIdx(rhs.mParmsIdx),
mName(rhs.mName),
mValue(rhs.mValue->cloneExpr()),
mFieldTypeInfo(rhs.mFieldTypeInfo)
{
}


AosJqlStmtAssignment::~AosJqlStmtAssignment()
{
}


bool
AosJqlStmtAssignment::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOp == JQLTypes::eCreate)
	{
		if (mName == "")
		{
			AosSetErrorUser(rdata, __func__)
				<< "The uservar name is NULL." << enderr;
			OmnAlarm << enderr;
			return false;
		}
		if (mIsParms)
		{
			mValue = gAosJQLParser.getSourceParm(mParmsIdx);
			if (!mValue)
			{
				AosSetErrorUser(rdata, __func__)
					<< "SYSTEM_ARGS[" << mParmsIdx << "] is NULL." << enderr;
				OmnAlarm << enderr;
				return false;
			}
		}

		AosExprUserVarPtr var;
		AosExprObjPtr expr = gAosJQLParser.getUserVar(mName);
		if (!expr)
		{
			var = OmnNew AosExprUserVar();
		}
		else
		{
			var = dynamic_cast<AosExprUserVar *>(expr.getPtr());
		}
		var->setName(mName);

		if (mFieldTypeInfo)
		{
			var->setType(mFieldTypeInfo);
		}
		if (!mFieldTypeInfo)
		{
			mFieldTypeInfo = var->getFieldTypeInfo();
			if (!mFieldTypeInfo)
			{
				mFieldTypeInfo =  OmnNew AosJQLDataFieldTypeInfo();
				mFieldTypeInfo->setType("string");
				var->setType(mFieldTypeInfo);
			}
		}

		if (mValue)
		{
			AosValueRslt vv;
			bool rslt  = mValue->getValue(rdata.getPtr(), 0, vv);
			aos_assert_r(rslt, false);
			//if (!(mFieldTypeInfo->getType() == "number" && vv.isNumericType())
				//&& !(mFieldTypeInfo->getType() == "string" && vv.isCharacterType()))
			if (!(AosDataType::isNumericType(vv.getType()) || AosDataType::isCharacterType(vv.getType())))
			{
				OmnString error;
				error << "ERROR: Data type mismatch. UserVar Name is \'" << mName << "\'"; 
				rdata->setJqlMsg(error);
				return false;
			}
			var->setValue(vv);
			OmnString msg = "";
			msg << "var " << mName << " = (" << mFieldTypeInfo->getType() << ")" << vv.getStr();
			rdata->setJqlMsg(msg);
		}
		else
		{
			OmnString msg = "";
			msg << "var " << mName << " create sucessfully";
			rdata->setJqlMsg(msg);
		}

		gAosJQLParser.setUserVar(mName, var);
		return true;
	}

	AosSetEntityError(rdata, "JQL_Assignment_run_err", "JQL Assignment", "")
		<< "opr is undefined!" << enderr;                               
	return false;
}


AosJqlStatement *
AosJqlStmtAssignment::clone()
{
	return OmnNew AosJqlStmtAssignment(*this);
}


void 
AosJqlStmtAssignment::dump()
{
}

void
AosJqlStmtAssignment::setName(const OmnString &name)
{
	mName = name;
}

void 
AosJqlStmtAssignment::setValue(const AosExprObjPtr &value)
{
	mValue = value;
}

void 
AosJqlStmtAssignment::setType(const AosJQLDataFieldTypeInfoPtr &type)
{
	mFieldTypeInfo = type;
}

void 
AosJqlStmtAssignment::setIsParms(const bool &flag)
{
	mIsParms = flag;
}

void 
AosJqlStmtAssignment::setParms(const int &index)
{
	mParmsIdx = index;
}


AosExprObjPtr
AosJqlStmtAssignment::getUserVar(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata, __func__)
			<< "The uservar name is NULL." << enderr;
		OmnAlarm << enderr;
		return NULL;
	}

	AosExprObjPtr expr = gAosJQLParser.getUserVar(mName);
	if (!expr)
	{
		AosSetErrorUser(rdata, __func__)
			<< "The uservar is undefined. Name is " << mName  << enderr;
		OmnAlarm << enderr;
		return NULL;
	}
	return expr;
}

