////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbTableMySql.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "MySQL/DbTableMySql.h"

#include "MySQL/DbRcdMySql.h"
#include "Util/OmnNew.h"


OmnDbTableMySql::OmnDbTableMySql(MYSQL_RES *dbrslt)
:
mDbRslt(dbrslt),
mCrtIndex(0),
mNumFields(0),
mDeleteFlag(true)
{
	if (dbrslt) mNumFields = mysql_num_fields(mDbRslt);
}

OmnDbTableMySql::~OmnDbTableMySql()
{
	if (mDeleteFlag)
	{
		mysql_free_result(mDbRslt);
	}
}


void            
OmnDbTableMySql::reset()
{
	mCrtIndex = 0;
	if (!mDbRslt) return;
	mysql_data_seek(mDbRslt, 0);
}


bool            
OmnDbTableMySql::hasMore() const
{
	if (!mDbRslt) return false;
	return mCrtIndex < mysql_num_rows(mDbRslt);
}


OmnDbRecordPtr  
OmnDbTableMySql::getRecord(const int idx)
{
	if (!mDbRslt) return 0;
	mysql_data_seek(mDbRslt, idx);
	MYSQL_ROW row = mysql_fetch_row(mDbRslt);
	if (!row)
	{
		return 0;
	}

	OmnDbTablePtr thisPtr(this, false);
	return OmnNew OmnDbRcdMySql(row, mNumFields, thisPtr);
}


OmnDbRecordPtr  
OmnDbTableMySql::next()
{
	if (!mDbRslt) return 0;
	MYSQL_ROW row = mysql_fetch_row(mDbRslt);
	if (!row)
	{
		return 0;
	}

	mCrtIndex++;

	OmnDbTablePtr thisPtr(this, false);
	return OmnNew OmnDbRcdMySql(row, mNumFields, thisPtr);
}


OmnDbRecordPtr  
OmnDbTableMySql::next2()
{
	if (!mDbRslt) return 0;
	if (!mMySql) return 0;
	MYSQL_ROW row = mysql_fetch_row(mDbRslt);
	if (!row)
	{
		return 0;
	}
	mysql_use_result(mMySql);
	mCrtIndex++;
	OmnDbTablePtr thisPtr(this, false);
	return OmnNew OmnDbRcdMySql(row, mNumFields, thisPtr);
}


bool
OmnDbTableMySql::readDataFinish()
{
	if (mDbRslt)
	{
		mysql_free_result(mDbRslt);
		return true;
	}
	return false;
}

int             
OmnDbTableMySql::entries() const
{
	// Chen Ding, 05/02/2008
	// return mNumFields;
	if (!mDbRslt) return 0;
	return mysql_num_rows(mDbRslt);
}

