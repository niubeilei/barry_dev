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
// 2015/12/04 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtJoinSyncher.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

AosJqlStmtJoinSyncher::AosJqlStmtJoinSyncher(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
}


AosJqlStmtJoinSyncher::AosJqlStmtJoinSyncher()
{
}


AosJqlStmtJoinSyncher::~AosJqlStmtJoinSyncher()
{
}


bool
AosJqlStmtJoinSyncher::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if(mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		
		return false;
	}

	if (mOp == JQLTypes::eCreate) return createSyncher(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL Map", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtJoinSyncher::createSyncher(const AosRundataPtr &rdata)
{
	OmnString dbname = getCurDatabase(rdata);
	OmnString conf = "";
	conf << "<join zky_database=\"" << dbname << "\" ><jointable>";
	for (size_t i = 0; i < mJoinTables.size(); i++)
	{
		conf << "<column>" << mJoinTables[i] << "</column>";
	}
	conf << "</jointable>";
	conf << "<joinindex>";
	for (size_t i = 0; i < mJoinIndexs.size(); i++)
	{
		conf << "<column>" << mJoinIndexs[i] << "</column>";
	}
	conf << "</joinindex>";
	conf << "<jointype>" << mJoinType << "</jointype>";
	conf << "<cond>" << mCond << "</cond>";
	conf << "</join>";

	for (size_t i = 0; i < mJoinTables.size(); i++)
	{
		OmnString tbl_name = mJoinTables[i];

		AosXmlTagPtr tbl_doc = getDoc(rdata, JQLTypes::eTableDoc, tbl_name);
		aos_assert_r(tbl_doc, false);

		AosXmlTagPtr joinsNode = tbl_doc->getFirstChild("joins");
		if (!joinsNode)
		{
			OmnString confstr; 
			confstr << "<joins>" << conf << "</joins>";
			AosXmlTagPtr def = AosXmlParser::parse(confstr AosMemoryCheckerArgs);
			aos_assert_r(def, false);
			tbl_doc->addNode(def);
		}
		else 
		{
			AosXmlTagPtr def = AosXmlParser::parse(conf AosMemoryCheckerArgs);
			aos_assert_r(def, false);
			joinsNode->addNode(def);
		}

		bool rslt = modifyDoc(rdata, tbl_doc);
		aos_assert_r(rslt, false);
	}

	rdata->setJqlMsg("Join created successfully.");
	return true;
}


AosJqlStatement *
AosJqlStmtJoinSyncher::clone()
{
	return OmnNew AosJqlStmtJoinSyncher(*this);
}


void 
AosJqlStmtJoinSyncher::setJoinType(OmnString name)
{
	mJoinType = name;
}


void 
AosJqlStmtJoinSyncher::setJoinTables(AosExprList *key_list)
{
	aos_assert(key_list);
	for (size_t i = 0; i < key_list->size(); i++)
	{
		mJoinTables.push_back((*key_list)[i]->dumpByNoEscape());
	}
}


void 
AosJqlStmtJoinSyncher::setJoinIndexs(AosExprList *key_list)
{
	aos_assert(key_list);
	for (size_t i = 0; i < key_list->size(); i++)
	{
		mJoinIndexs.push_back((*key_list)[i]->dumpByNoEscape());
	}
}


void 
AosJqlStmtJoinSyncher::setCondition(AosExprObj *value)
{
	mCond = value->dumpByNoEscape();
}
