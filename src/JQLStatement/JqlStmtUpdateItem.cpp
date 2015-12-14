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
#include "JQLStatement/JqlStmtUpdateItem.h"

#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLExpr/Expr.h"

#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/BitmapObj.h"

#include "SEUtil/IILName.h"
#include "QueryUtil/QrUtil.h"
#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

#include "Rundata/Rundata.h"
#include "Debug/Debug.h"


AosJqlStmtUpdateItem::AosJqlStmtUpdateItem(AosJqlStmtUpdate *update)
{
	aos_assert(update);
	mPUpdate = update;

	mPExprList = update->update_expr_list;
	aos_assert(mPExprList);

	for (u32 i = 0; i < mPExprList->size(); i++)
	{
		AosExpr *pExpr = dynamic_cast<AosExpr*>((*mPExprList)[i].getPtr());
		aos_assert(pExpr);
		AosExprObjPtr pExprL = pExpr->getExpr(1);
		aos_assert(pExprL);
		AosExprObjPtr pExprR = pExpr->getExpr(2);
		aos_assert(pExprR);
		OmnString name, value;
		name = pExprL->getValue(0);
		value = pExprR->getValue(0);
		mValues[name] = value;
	}

	mPTable = update->table;
	aos_assert(mPTable);
	mOptWhere = dynamic_cast<AosExpr*>(update->opt_where);
}

AosJqlStmtUpdateItem::AosJqlStmtUpdateItem(const OmnString &errmsg)
:
mPUpdate(0),
mPExprList(0),
mPTable(0),
mOptWhere(0),
mErrmsg(errmsg)
{
}


AosJqlStmtUpdateItem::AosJqlStmtUpdateItem()
:
mPUpdate(0),
mPExprList(0),
mPTable(0),
mOptWhere(0)
{
}


AosJqlStmtUpdateItem::~AosJqlStmtUpdateItem()
{
	delete mPUpdate;
}


bool
AosJqlStmtUpdateItem::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	bool rslt;
	OmnString name, value, req;
	AosExprObjPtr expr;

	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		return false;
	}

	mTableName = mPTable->getValue(rdata.getPtr());                 
	vector<QueryCondtion> query_conds;
	if (mOptWhere)
	{
		if (mOptWhere->isHasOr())
		{
			// error
			return false;
		}
		if (mOptWhere->isJoin())
		{
			return false;
		}

		OmnString iil_name, value, opr;
		vector<AosJqlQueryWhereCondPtr>	conds;
		rslt = mOptWhere->createConds(rdata, conds, mTableName);
		aos_assert_r(rslt, false);
		for (u32 i = 0; i < conds.size(); i++)
		{
			iil_name = AosIILName::composeCtnrMemberListing(mTableName, conds[i]->mLHS);
			QueryCondtion cond(iil_name, AosOpr_toEnum(conds[i]->mOpr), conds[i]->mRHS);
			query_conds.push_back(cond);
		}
	}
	else
	{
		OmnString iil_name = AosIILName::composeCtnrMemberListing("", AOSTAG_PARENTC);
		QueryCondtion cond(iil_name, AosOpr_toEnum("="), mTableName);
		query_conds.push_back(cond);
	}

	AosBitmapObjPtr bitmap;
	bitmap = querySafe(rdata, query_conds);
	aos_assert_r(bitmap, false);

	u64 docid;                                                  
	while (bitmap->nextDocid(docid))
	{                                                           
		AosXmlTagPtr doc = getDocByDocid(rdata, docid);
		if (!doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		rslt = modifyDoc(rdata, doc);
		if (!rslt)
		{
			AosSetErrorUser(rdata, __func__)
				<< "Failed to update doc." << enderr;
			OmnAlarm << enderr;
			return false;
		}
	}

	req = "Update is successful.";
	rdata->setResults(req);
	//rdata->setJqlUpdateCount(docids.size());
	rdata->setJqlMsg(req);
	rdata->setOk();
	OmnCout << " Update is successful!" << endl;
	return true;
}


bool
AosJqlStmtUpdateItem::modifyDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc)
{
	OmnString error;
	map<OmnString, OmnString>::iterator itr;
	for (itr = mValues.begin(); itr != mValues.end(); itr++)
	{
		OmnString name = itr->first;
		OmnString value = itr->second;
		bool rslt = false;
		if (value == "") 
		{
			doc->removeAttr(name);
		}
		else
		{
			doc->xpathSetAttr(name, value, true);
		}

		rslt = AosJqlStatement::modifyDoc(rdata, doc);
		if (!rslt) 
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}
	return true;
}


AosJqlStatement *
AosJqlStmtUpdateItem::clone()
{
	return 0;
}


void 
AosJqlStmtUpdateItem::dump()
{
}

bool
AosJqlStmtUpdateItem::updateData(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		map<OmnString, OmnString> *fieldValueMap )
{
	if (!fieldValueMap)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	map<OmnString, OmnString>::iterator itr = fieldValueMap->begin();
	while (itr != fieldValueMap->end())
	{
		bool rslt = doc->xpathSetAttr(itr->first, itr->second, true);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		itr++;
	}

	bool rslt = AosModifyDoc(doc, rdata);    
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnTagFuncInfo << "Modify a doc. Doc is: " << doc->toString() << endl;
	return rslt;
}
