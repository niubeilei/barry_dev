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
#include "JQLStatement/JqlStmtDeleteItem.h"

#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLExpr/Expr.h"

#include "SEUtil/IILName.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "QueryUtil/QrUtil.h"

#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

#include "Rundata/Rundata.h"
#include "Debug/Debug.h"

#include "DataRecord/RecordXmlSimple.h"

AosJqlStmtDeleteItem::AosJqlStmtDeleteItem(const OmnString &errmsg)
:
mPDelete(0),
mPExprList(0),
mPTable(0),
mOptWhere(0),
mErrmsg(errmsg)
{
}


AosJqlStmtDeleteItem::AosJqlStmtDeleteItem(AosJqlStmtDelete *del)
{
	aos_assert(del);
	mPDelete = del;

	mPTable = del->table;
	aos_assert(mPTable);
	//mOptWhere = dynamic_cast<AosExpr*>(del->opt_where);
	mOptWhere = del->opt_where;
}


AosJqlStmtDeleteItem::AosJqlStmtDeleteItem(const AosJqlStmtDeleteItem &rhs)
:
mPDelete(0),
mPExprList(0),
mPTable(0),
mOptWhere(0)
{
}


AosJqlStmtDeleteItem::~AosJqlStmtDeleteItem()
{
	delete mPDelete;
	//delete mOptWhere;
}


bool
AosJqlStmtDeleteItem::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
	}

	OmnString table_name, req,name , value;
	AosExprObjPtr expr;
	table_name = mPTable->getValue(rdata.getPtrNoLock());
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata); 
	if (!table_doc)
	{
		OmnString error;
		error << "The base table doesn't exist: " << table_name;
		rdata->setJqlMsg(error);

		OmnCout << "The base table doesn't exist: " << table_name << endl;
		return false;
	}

	vector<QueryCondtion> query_conds;
	if (mOptWhere)
	{
		if (mOptWhere->isHasOr())
		{
			return false;
		}
		if (mOptWhere->isJoin())
		{
			return false;
		}

		OmnString iil_name, value, opr;
		vector<AosJqlQueryWhereCondPtr>	conds;
		bool rslt = mOptWhere->createConds(rdata, conds, table_name);
		aos_assert_r(rslt, false);
		for (u32 i = 0; i < conds.size(); i++)
		{
			iil_name = AosIILName::composeCtnrMemberListing(table_name, conds[i]->mLHS);
			QueryCondtion cond(iil_name, AosOpr_toEnum(conds[i]->mOpr), conds[i]->mRHS);
			query_conds.push_back(cond);
		}
	}
	else
	{
		OmnString iil_name = AosIILName::composeCtnrMemberListing("", AOSTAG_PARENTC);
		QueryCondtion cond(iil_name, AosOpr_toEnum("="), table_name);
		query_conds.push_back(cond);
	}

	AosBitmapObjPtr bitmap;
	bitmap = querySafe(rdata, query_conds);
	aos_assert_r(bitmap, false);

	u64 docid;
	while (bitmap->nextDocid(docid))
	{
		bool rslt = deleteDocByDocid(rdata, docid);
		if (!rslt)	
		{
			AosSetErrorUser(rdata,  __func__) << "delete error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	req = "Doc deleted successfully.";
	rdata->setJqlMsg(req);
	rdata->setResults(req);
	rdata->setOk();
	return true;
}


AosJqlStatement *
AosJqlStmtDeleteItem::clone()
{
	return OmnNew AosJqlStmtDeleteItem(*this);
}


void 
AosJqlStmtDeleteItem::dump()
{
}

