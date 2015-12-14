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
//  This class simulates the arithmatic condition. The class identify
//  two fields in the CLI command and checks whether the condition
//  is true.
//
// Modification History:
// 06/29/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/CondCheckTable.h"

#include "Alarm/Alarm.h"
#include "alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Parms/Util.h"
#include "TorturerConds/CondData.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"


AosTortCondCheckTable::AosTortCondCheckTable(
				const AosGenTablePtr &table, 
				AosTableCheckFunc func, 
				void *data, 
				const int numEntries)
:
mTable(table),
mFunc(func),
mData(data),
mNumEntries(numEntries)
{
	aos_assert(table);
	aos_assert(func);
	aos_assert(numEntries >= 0);
	aos_assert(data);
}


AosTortCondCheckTable::~AosTortCondCheckTable()
{
}


bool	
AosTortCondCheckTable::check(const AosCondData &data, 
						bool &rslt, 
						OmnString &errmsg) const
{
	//
	// Reset the flags for all the records
	//
	aos_assert_r(mTable, false);
	aos_assert_r(mNumEntries >= 0, false);
	aos_assert_r(mData, false);
	aos_assert_r(mFunc, false);

OmnTrace << "Check table: " << mTable->getName() << ":" << mNumEntries << endl;
	mTable->resetFlags(0);
	errmsg = "";
	rslt = true;

	if (mTable->entries() != mNumEntries)
	{
		rslt = false;
		errmsg << "Number of elements does not match: "
			<< "Number of entries in table: " 
			<< mTable->entries()
			<< ". Number of elements expected: " 
			<< mNumEntries;
		return true;
	}

	if (mNumEntries == 0) return true;

	AosGenRecordPtr record = mTable->createRecord();
	aos_assert_r(record, false);
	AosGenRecordPtr rec;
	for (int i=0; i<mNumEntries; i++)
	{
		if (!mFunc(record, mData, i))
		{
			rslt = false;
			errmsg << "Failed to set record: " << i;
			return true;
		}

		if ((rec = mTable->recordExist(record, false)).isNull())
		{
			rslt = false;
			errmsg << "Record not found in table. Record: " 
				<< record->toString() 
				<< "\nTable: " << mTable->toString();
			return true;
		}

		if (rec->getFlag() != 0) 
		{
			rslt = false;
			errmsg << "Record found flag != 0: " 
				<< record->toString()
				<< "\nTable: \n"
				<< mTable->toString();
			return true;
		}

		rec->setFlag(1);
	}

	if (mTable->anyRecordNotMarked(1))
	{
		rslt = false;
		errmsg << "Some records exist in table but not in app. \n"
			<< mTable->dumpRecordsWithNotFlagged(1);
		return true;
	}

	rslt = true;
	return true;
}

