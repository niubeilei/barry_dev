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
// 05/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/DialogWrapper.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


AosDialogWrapper::AosDialogWrapper()
{
}


AosDialogWrapper::~AosDialogWrapper()
{
}


// 
// Description:
// This member function retrieves the named Object. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosDialogWrapper::getObject(const OmnString &objid)
{
	// 
	// It retrieves the RVG identified by "name" from the database.
	// 
    OmnString stmt = "select * from dialog where ";
    stmt << "name='" << objid << "'";

	
	OmnTrace << "To retrieve Object: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Object: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple Objects with the same name: " << objid 
			<< ". Number of objects found: " << table->entries() << enderr;
		return false;
	}

	if (table->entries() == 0)
	{
		// Did not find the Object 
		return false;
	}

	table->reset();
	OmnDbRecordPtr record = table->next();
	aos_assert_r(record, false);

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnRslt
AosDialogWrapper::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
    mName 	  	 = record->getStr(0, "NoName", rslt);
    mDesc      	 = record->getStr(1, "", rslt);
    mXml      	 = record->getStr(2, "", rslt);

	return rslt;
}


OmnRslt
AosDialogWrapper::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from dialog where ";
    stmt << "name='" << mName << "'";

	
	OmnTrace << "To retrieve Object: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Object: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple Objects with the same name: " << mName << enderr;
		return false;
	}

	table->reset();
	OmnDbRecordPtr record = table->next();
	aos_assert_r(record, false);

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
AosDialogWrapper::updateStmt() const
{
    OmnString stmt = "update dialog set ";
    stmt 
        << "xml = '" << mXml
		<< "', description='" << mDesc
		<< "' where name='" << mName << "'";

	return stmt;
}


OmnString
AosDialogWrapper::removeStmt() const
{
    OmnString stmt = "delete from dialog where ";
    stmt << "name = '" << mName << "'";

    return stmt;
}


OmnString
AosDialogWrapper::removeAllStmt() const
{
    OmnString stmt = "delete from dialog";

    return stmt;
}


OmnString
AosDialogWrapper::existStmt() const
{
    OmnString stmt = "select * from dialog where ";
    stmt << "name = '" << mName << "'";
    return stmt;
}



OmnString
AosDialogWrapper::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into dialog ("
        "name, "
		"description, "
        "xml) values ('";

    stmt << mName << "', '"
		 << mDesc << "', '"
         << mXml << "')";

    return stmt;
}


bool	
AosDialogWrapper::objectExist(const OmnString &objid)
{
	AosDialogWrapper wp;
	return wp.getObject(objid);
}


OmnString
AosDialogWrapper::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}

