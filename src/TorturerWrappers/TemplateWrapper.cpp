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
// 05/12/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerWrappers/TemplateWrapper.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Debug/Debug.h"


AosTemplateWrapper::AosTemplateWrapper()
{
}


AosTemplateWrapper::~AosTemplateWrapper()
{
}


// 
// Description:
// This member function retrieves the named template. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosTemplateWrapper::getTemplate(const OmnString &name)
{
	// 
	// It retrieves the RVG identified by "name" from the database.
	// 
    OmnString stmt = "select * from template where ";
    stmt << "name='" << name << "'";

	
	OmnTrace << "To retrieve template: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve template: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple templates with the same name: " << name 
			<< ". Number of objects found: " << table->entries() << enderr;
		return false;
	}

	if (table->entries() == 0)
	{
		// Did not find the template 
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
AosTemplateWrapper::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
    mName 	  	 = record->getStr(0, "NoName", rslt);
    mDesc      	 = record->getStr(1, "", rslt);
    mXml      	 = record->getStr(2, "", rslt);

	return rslt;
}


OmnRslt
AosTemplateWrapper::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from template where ";
    stmt << "name='" << mName << "'";

	
	OmnTrace << "To retrieve template: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve template: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple templates with the same name: " << mName << enderr;
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
AosTemplateWrapper::updateStmt() const
{
    OmnString stmt = "update template set ";
    stmt 
        << "xml = '" << mXml
		<< "', description='" << mDesc
		<< "' where name='" << mName << "'";

	return stmt;
}


OmnString
AosTemplateWrapper::removeStmt() const
{
    OmnString stmt = "delete from template where ";
    stmt << "name = '" << mName << "'";

    return stmt;
}


OmnString
AosTemplateWrapper::removeAllStmt() const
{
    OmnString stmt = "delete from template";

    return stmt;
}


OmnString
AosTemplateWrapper::existStmt() const
{
    OmnString stmt = "select * from template where ";
    stmt << "name = '" << mName << "'";
    return stmt;
}



OmnString
AosTemplateWrapper::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into template ("
        "name, "
		"description, "
        "xml) values ('";

    stmt << mName << "', '"
		 << mDesc << "', '"
         << mXml << "')";

    return stmt;
}


bool	
AosTemplateWrapper::templateExist(const OmnString &name)
{
	AosTemplateWrapper wp;
	return wp.getTemplate(name);
}


OmnString
AosTemplateWrapper::retrieveStmt() const
{
	OmnNotImplementedYet;
	return "";
}



