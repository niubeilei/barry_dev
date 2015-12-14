////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTableSQLite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SQLite/DbTableSQLite.h"

#include "SQLite/DbRcdSQLite.h"
#include "Util/OmnNew.h"


OmnDbTableSQLite::~OmnDbTableSQLite()
{
	if (mDeleteFlag)
	{
		sqlite3_free_table(mDbRslt);
	}
}


void            
OmnDbTableSQLite::reset()
{
	mCrtIndex = 1;
}


bool            
OmnDbTableSQLite::hasMore() const
{
	return mCrtIndex < (mNumRows + 1);
}


OmnDbRecordPtr  
OmnDbTableSQLite::next()
{
	char **row = &mDbRslt[mCrtIndex*mNumFields];
	if (!row)
	{
	    return 0;
	}
	
	OmnDbRecordPtr DbRecordPtr = (OmnDbRecordPtr) OmnNew OmnDbRcdSQLite(mDbRslt, mNumFields, mCrtIndex);
	mCrtIndex++;
	return DbRecordPtr;
}


int             
OmnDbTableSQLite::entries() const
{
	return mNumRows;
}
