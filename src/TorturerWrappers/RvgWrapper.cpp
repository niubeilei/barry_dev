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
// 04/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/RvgWrapper.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


AosRvgWrapper::AosRvgWrapper()
:
mStatus('A')
{
}


AosRvgWrapper::~AosRvgWrapper()
{
}


// 
// Description:
// This member function retrieves the named RVG. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosRvgWrapper::getRvg(const OmnString &name)
{
	// 
	// It retrieves the RVG identified by "name" from the database.
	// 
    OmnString stmt = "select * from rvg where ";
    stmt << "name='" << name << "'";

	
	OmnTrace << "To retrieve RVG: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve RVG: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple RVGs with the same name: " << name 
			<< ". Number of objects found: " << table->entries() << enderr;
		return false;
	}

	if (table->entries() == 0)
	{
		// Did not find the RVG
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
AosRvgWrapper::serializeFromRecord(const OmnDbRecordPtr &record)
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
AosRvgWrapper::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from rvg where ";
    stmt << "name='" << mName << "'";

	
	OmnTrace << "To retrieve rvg: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve RVG: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple RVGs with the same name: " << mName << enderr;
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
AosRvgWrapper::updateStmt() const
{
    OmnString stmt = "update rvg set ";
    stmt 
        << "keywords = '" << mKeywords
        << "', description = '" << mDescription
        << "', xml = '" << mXml
		<< "' where name='" << mName << "'";

OmnTrace << "Update statement: " << stmt << endl;
	return stmt;
}


OmnString
AosRvgWrapper::removeStmt() const
{
    OmnString stmt = "delete from rvg where ";
    stmt << "name = '" << mName << "'";

    return stmt;
}


OmnString
AosRvgWrapper::removeAllStmt() const
{
    OmnString stmt = "delete from rvg";

    return stmt;
}


OmnString
AosRvgWrapper::existStmt() const
{
    OmnString stmt = "select * from rvg where ";
    stmt << "name = '" << mName << "'";
    return stmt;
}



OmnString
AosRvgWrapper::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into rvg ("
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
AosRvgWrapper::rvgExist(const OmnString &name)
{
	AosRvgWrapper wp;
	return wp.getRvg(name);
}


OmnString
AosRvgWrapper::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}


