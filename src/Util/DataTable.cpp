////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/DataTable.h"

#include "Util/TableFixed.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static AosDataTable*	sgTables[AosTableType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;

AosDataTable::AosDataTable(
		const OmnString &name, 
		const AosTableType::E type, 
		const bool flag)
:
mType(type),
mStartIdx(-1),
mPageSize(-1)
{
	if (flag)
	{
		if (!registerTable(name, this))
		{
			OmnThrowException("failed register table");
		}
	}
}


AosDataTable::~AosDataTable()
{
}


AosDataTablePtr 
AosDataTable::getTable(const AosXmlTagPtr &def, AosRundata *rdata)
{
	if(!sgInited) staticInit();
	aos_assert_rr(def, rdata, 0);
	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosTableType::E type = AosTableType::toEnum(ss);
	aos_assert_rr(AosTableType::isValid(type), rdata, 0);
	AosDataTable *table = sgTables[type];
	if (!table)
	{
		AosSetErrorU(rdata, "table_not_registered") << ss << enderr;
		return 0;
	}

	return table->clone(def, rdata);
}


bool
AosDataTable::staticInit()
{
	if(sgInited)    return true;

	static AosTableFixed	AosTableFixed(true);

	sgLock.lock();
	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosDataTable::registerTable(const OmnString &name, AosDataTable *table)
{
	sgLock.lock();
	if (!AosTableType::isValid(table->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect id: ";
		errmsg << table->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgTables[table->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Already registered: ";
		errmsg << table->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgTables[table->mType] = table;
	sgLock.unlock();
	return true;
}


/*
AosDataTablePtr 
AosDataTable::getTable(
		const int fieldlen1, 
		const int fieldlen2, 
		const int start_idx,
		const int pagesize, 
		AosRundata *rdata)
{
	return OmnNew AosTableFixed(fieldlen1, fieldlen2, 
			start_idx, pagesize, rdata);
}
*/


