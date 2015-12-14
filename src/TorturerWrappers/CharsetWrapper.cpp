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
// 05/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/CharsetWrapper.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


AosCharsetWrapper::AosCharsetWrapper()
:
mStatus('A')
{
}


AosCharsetWrapper::~AosCharsetWrapper()
{
}


// 
// Description:
// This member function retrieves the named Charset. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosCharsetWrapper::getCharset(const OmnString &name)
{
	// 
	// It retrieves the Charset identified by "name" from the database.
	// 
    OmnString stmt = "select * from charset where ";
    stmt << "name='" << name << "'";

	
	OmnTrace << "To retrieve Charset: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Charset: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple Charsets with the same name: " << name 
			<< ". Number of objects found: " << table->entries() << enderr;
		return false;
	}

	if (table->entries() == 0)
	{
		// Did not find the Charset
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
AosCharsetWrapper::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
    mName 	  	 = record->getStr(0, "NoName", rslt);
    mKeywords 	 = record->getStr(1, "", rslt);
    mDescription = record->getStr(2, "", rslt);
    mXml      	 = record->getStr(3, "", rslt);

	return rslt;
}


OmnRslt
AosCharsetWrapper::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from charset where ";
    stmt << "name='" << mName << "'";

	
	OmnTrace << "To retrieve Charset: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Charset: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple Charsets with the same name: " << mName << enderr;
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
AosCharsetWrapper::updateStmt() const
{
    OmnString stmt = "update charset set ";
    stmt 
        << "keywords = '" << mKeywords
        << "', description = '" << mDescription
        << "', xml = '" << mXml
		<< "' where name='" << mName << "'";

OmnTrace << "Update statement: " << stmt << endl;
	return stmt;
}


OmnString
AosCharsetWrapper::removeStmt() const
{
    OmnString stmt = "delete from charset where ";
    stmt << "name = '" << mName << "'";

    return stmt;
}


OmnString
AosCharsetWrapper::removeAllStmt() const
{
    OmnString stmt = "delete from charset";

    return stmt;
}


OmnString
AosCharsetWrapper::existStmt() const
{
    OmnString stmt = "select * from charset where ";
    stmt << "name = '" << mName << "'";
    return stmt;
}



OmnString
AosCharsetWrapper::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into charset ("
        "name, "
        "keywords, "
		"description, "
        "xml, "
		"status) values ('";

    stmt << mName << "', '"
		 << mKeywords << "', '"
		 << mDescription << "', '"
         << mXml << "', '"
         << mStatus  << "')";

    return stmt;
}


bool	
AosCharsetWrapper::charsetExist(const OmnString &name)
{
	AosCharsetWrapper wp;
	return wp.getCharset(name);
}


OmnString
AosCharsetWrapper::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}
