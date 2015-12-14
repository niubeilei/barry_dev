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
#include "JQLStatement/JqlJoinTable.h"
#include "JQLStatement/JqlTableReference.h"
#include "JQLStatement/JqlJoinStatTable.h"

#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"


AosJqlJoinTable::AosJqlJoinTable()
{
}

AosJqlJoinTable::~AosJqlJoinTable()
{
}

/*

bool 
AosJqlJoinTable::getAllTable(const AosRundataPtr &rdata, vector<AosExprObjPtr> &tables)
{
	bool rslt;
	AosJqlTableReferencePtr table_reference;
	for (u32 i = 0; i<mTableReferencesPtr->size(); i++)
	{ 
		table_reference = (*mTableReferencesPtr)[i];
		rslt = table_reference->getAllTable(rdata, tables);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	} 
	return true; 
}

*/

bool
AosJqlJoinTable::getAllTableDef(const AosRundataPtr &rdata, vector<AosXmlTagPtr> &table_defs)
{
	OmnNotImplementedYet;
	return false; 
}
/*
AosSelectTablePtr
AosJqlJoinTable::clone(const AosRundataPtr &rdata)const
{
	if (!0)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	return 0;
}
*/
void
AosJqlJoinTable::setSubQuery()
{
	mIsSubQuery = true;
}
