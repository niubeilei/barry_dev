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
//
// Modification History:
// 12/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/ClassDef.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"


AosClassDef::AosClassDef()
{
}

AosClassDef::~AosClassDef()
{
}


OmnRslt
AosClassDef::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
	mObjTablename 	= record->getStr(0, "", rslt);
	mClassDef	 	= record->getStr(1, "", rslt);

	return rslt;
}


OmnString
AosClassDef::updateStmt() const
{
    OmnString stmt = "update ";
	stmt << mTablename << " set class_def='"
		 << mClassDef << "' where tablename = '" << mObjTablename;
	return stmt;
}


OmnString
AosClassDef::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into ";
	stmt << mTablename << "("
		"tablename, class_def) values ('";
    stmt << mObjTablename << "', '"
		 << mClassDef << "')";
    return stmt;
}


OmnString
AosClassDef::existStmt() const
{
    OmnString stmt = "select * from ";
    stmt << mTablename << " where tablename ='" << mObjTablename << "'";
    return stmt;
}


OmnString 
AosClassDef::retrieveStmt() const
{
	OmnString stmt = "select * from ";
	stmt << mTablename << " where tablename='" << mObjTablename << "'";
	return stmt;
}


OmnString
AosClassDef::removeStmt() const
{
	OmnString stmt = "delete from ";
	stmt << mTablename << " where tablename='" << mObjTablename << "'";
	return stmt;
}


OmnString
AosClassDef::removeAllStmt() const
{
	OmnString stmt = "delete from ";
	stmt << mTablename;
	return stmt;
}

