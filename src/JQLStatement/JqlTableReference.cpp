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
#include "JQLStatement/JqlTableReference.h"
#include "JQLStatement/JqlTableFactor.h"
#include "JQLStatement/JqlJoinStatTable.h"

#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"

AosJqlTableReference::AosJqlTableReference()
{
}

AosJqlTableReference::~AosJqlTableReference()
{
}

bool
AosJqlTableReference::init(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select)
{
	if (mTableFactor)
		return mTableFactor->init(rdata, select);

	if (!mJoinTable)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return mJoinTable->init(rdata, select);
}


OmnString
AosJqlTableReference::generateTableConf(const AosRundataPtr &rdata)
{
	if (mTableFactor)
		return mTableFactor->generateTableConf(rdata);

	if (!mJoinTable)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	return mJoinTable->generateTableConf(rdata);
}


vector<AosJqlSelectFieldPtr> 
AosJqlTableReference::getTableFieldList(
		const OmnString &table_name,
		bool &rslt)
{
	vector<AosJqlSelectFieldPtr>  field_list;
	if (mTableFactor)
	{
		field_list = mTableFactor->getTableFieldList(table_name, rslt);
		return field_list;
	}

	if (!mJoinTable)
	{
		//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return field_list;
	}
	field_list = mJoinTable->getTableFieldList(table_name, rslt);
	return field_list;
}

bool
AosJqlTableReference::getAllField(
		const AosRundataPtr &rdata, 
		const AosJqlSelectPtr &select, 
		vector<AosJqlSelectFieldPtr> *&fields)
{
	if (mTableFactor)
		return mTableFactor->getAllField(rdata, select, fields);
	if (!mJoinTable)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return mJoinTable->getAllField(rdata, select, fields);
}

bool
AosJqlTableReference::getAllTableDef(
		const AosRundataPtr &rdata, 
		vector<AosXmlTagPtr> &table_defs)
{
	if (mTableFactor)
	{
		return mTableFactor->getAllTableDef(rdata, table_defs);
	}
	else
	{
		if (mJoinTable)
		{
			return mJoinTable->getAllTableDef(rdata, table_defs);
		}
	}
	return false;
}


AosJqlTableReference*
AosJqlTableReference::clone(const AosRundataPtr &rdata) const
{
	try
	{
		AosJqlTableReference *v = new AosJqlTableReference();
		if (mTableFactor)
		{
			v->mTableFactor = mTableFactor->clone(rdata);  
		}
		if (mJoinTable)
		{
			//v->mJoinTable = mJoinTable->clone(rdata);  
		}
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

void
AosJqlTableReference::setTableFactor(AosJqlTableFactor *table_factor)
{
	mTableFactor = table_factor;
}

void
AosJqlTableReference::setJqlJoinTable(AosJqlJoinTable *join_table)
{
	mJoinTable = join_table;
}

AosJqlTableFactorPtr 
AosJqlTableReference::getTableFactor()
{
	return mTableFactor;
}

AosJqlJoinTablePtr
AosJqlTableReference::getJoinTable()
{
	return mJoinTable;
}

void
AosJqlTableReference::setSubQuery()
{
	if (mTableFactor)
		mTableFactor->setSubQuery();
	if (mJoinTable)
		mJoinTable->setSubQuery();
}
