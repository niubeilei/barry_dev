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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlTableFactor.h"
#include "JQLStatement/JqlSubQueryTable.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"

AosJqlTableFactor::AosJqlTableFactor()       
{                         
	mIndexHint = NULL;
	mQuery = NULL;
}                         

AosJqlTableFactor::~AosJqlTableFactor()      
{                         
} 

bool
AosJqlTableFactor::init(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select)
{
	if (mVirtualTable) {
		return mVirtualTable->init(rdata, select);
	}
	if (!mTable) {
		mTable = AosJqlSingleTable::createStatic(rdata, select, mTableName, mAliasName, mIndexHint);
		//mTable->mTableName = mTableName;
		//mTable->mType = mType;
		//mTable->mAliasName = mAliasName;
		return mTable->init(rdata, select);
	}
	return mTable->init(rdata, select);
}


OmnString
AosJqlTableFactor::generateTableConf(const AosRundataPtr &rdata)
{
	// 1. If mTable is a Container:
	//    1.1 If mTable indicates "zky_manual_index" (bool), use the container's 
	//        index list to determine whether we can run the query using
	//        fast query or normaly;
	//    1.2 If mTable indicates not "zky_manual_index", it always use
	//        fast query.
	// 2. If mTable is a table (created by CREATE TABLE statement)
	//    mTable should have defined all the indexes (by running CREATE INDEX ...)

	if (mTable)
		return mTable->generateTableConf(rdata);

	if (!mVirtualTable)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	return mVirtualTable->generateTableConf(rdata);
}


vector<AosJqlSelectFieldPtr> 
AosJqlTableFactor::getTableFieldList(
		const OmnString &table_name,
		bool &rslt)
{
	vector<AosJqlSelectFieldPtr>  field_list;
	if (mTable)
	{
		field_list = mTable->getTableFieldList(table_name, rslt);
		return field_list;
	}

	if (!mVirtualTable)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return field_list;
	}
	field_list = mVirtualTable->getTableFieldList(table_name, rslt);
	return field_list;
}


bool
AosJqlTableFactor::getAllField(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select, 
		vector<AosJqlSelectFieldPtr> *&fields)
{
	if (mVirtualTable) {
		return mVirtualTable->getAllField(rdata, select, fields);
	}

	if (!mTable) {
		mTable = AosJqlSingleTable::createStatic(rdata, select, mTableName, mAliasName, mIndexHint);
		return mTable->getAllField(rdata, fields);
	}
	return mTable->getAllField(rdata, fields);
}


bool
AosJqlTableFactor::getAllTableDef(
		const AosRundataPtr &rdata, 
		vector<AosXmlTagPtr> &table_def)
{
	OmnString name = mTableName->getValue(rdata.getPtrNoLock());
	if (name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr doc = AosJqlStmtTable::getTable(name, rdata);
	if (doc) table_def.push_back(doc);
	return true; 
}


void 
AosJqlTableFactor::setVirtualTable(AosJqlSubQueryTable* virtual_table)
{
	mVirtualTable = virtual_table;
}


AosJqlTableFactor*
AosJqlTableFactor::clone(const AosRundataPtr &rdata) const
{
	try
	{
		AosJqlTableFactor *v = new AosJqlTableFactor();
		if (mTable)
		{
			v->mTable = mTable->clone(rdata);  
		}
		if (mVirtualTable)
		{
	//		v->mVirtualTable = mVirtualTable->clone(rdata);  
		}
		v->mIndexHint = mIndexHint;
		return v;
	}
	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}
	OmnShouldNeverComeHere;                                                    
	return 0;  
}


AosJqlSubQueryTablePtr
AosJqlTableFactor::getVirtualTbale()
{
	return mVirtualTable;
}


void 
AosJqlTableFactor::setSubQuery()
{
	mTable->mIsSubQuery = true;
}


void 
AosJqlTableFactor::IsStatTable(bool &isStat)
{
	if (mQuery)
	{
		AosTableReferences *tableRef = mQuery->getSelect()->mTableReferencesPtr;
		aos_assert(tableRef);
		AosJqlTableFactorPtr tableFactor = tableRef[0][0]->mTableFactor;
		aos_assert(tableFactor);

		tableFactor->IsStatTable(isStat);
	}
	else if (mTable)
	{
		mTable->IsStatTable(isStat);
	}

}
