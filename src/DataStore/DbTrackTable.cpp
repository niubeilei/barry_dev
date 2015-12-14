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
// 12/19/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/DbTrackTable.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "TinyXml/TinyXml.h"
#include "util_c/strutil.h"


const OmnString sgTablename = "table_changes";

AosDbTrackTable::AosDbTrackTable()
{
}

AosDbTrackTable::AosDbTrackTable(
		const AosDbTrackTable::E type, 
		const OmnString &tablename) 
:
mType(type),
mTableName(tablename)
{
}

AosDbTrackTable::~AosDbTrackTable()
{
}


void
AosDbTrackTable::startAssemble()
{
	switch (mType)
	{
	case eAdd:
		 mXml = "<type value=\"add\"/><record>";
		 break;

	case eDelete:
		 mXml = "<type value=\"delete\"/><record>";
		 break;

	case eModify:
		 mXml = "<type value=\"modify\"/><record>";
		 break;

	default:
		 OmnAlarm << "Incorrect type: " << (int)mType << enderr;
		 mXml = "<record>";
	}
}


void
AosDbTrackTable::endAssemble()
{
	mXml << "</record>";
}


void
AosDbTrackTable::addField(const OmnString &fname, const char value)
{
	mXml << "<field name=\"" << fname << "\">" << value << "</field>";
}


void
AosDbTrackTable::addField(const OmnString &fname, const int value)
{
	mXml << "<field name=\"" << fname << "\">" << value << "</field>";
}


void
AosDbTrackTable::addField(const OmnString &fname, const u32 value)
{
	mXml << "<field name=\"" << fname << "\">" << value << "</field>";
}


void
AosDbTrackTable::addField(const OmnString &fname, 
		const OmnString &value, 
		const bool useCDATA)
{
	if (useCDATA)
	{
		mXml << "<field name=\"" << fname << "\"><![CDATA[" << value << "]]></field>";
	}
	else
	{
		mXml << "<field name=\"" << fname << "\">" << value << "</field>";
	}
}


OmnRslt
AosDbTrackTable::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
	mSeqno 		= record->getU32(0, 0, rslt);
	mType		= (E)record->getChar(1, 'I', rslt);
    mTableName 	= record->getStr(2, "NoName", rslt);
    mXml      	= record->getStr(3, "", rslt);

	if (mType <= eTypeMin || mType >= eTypeMax)
	{
		OmnAlarm << "Invalid type: " << mType << enderr;
		mType = eInvalidType;
	}
	return rslt;
}


OmnString
AosDbTrackTable::updateStmt() const
{
    OmnString stmt = "update ";
	stmt << sgTablename << " set ";
	stmt << "change_type = '" << mType 
		 << "', table_name='" << mTableName 
		 << "', xml='" << mXml << "' where seqno =" << mSeqno;

	return stmt;
}


OmnString
AosDbTrackTable::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into ";
	stmt << sgTablename << "("
		"change_type, "
        "table_name, "
        "xml) values ('";

    stmt << mType << "', '"
		 << mTableName << "', '"
         << mXml << "')";

    return stmt;
}


OmnString
AosDbTrackTable::existStmt() const
{
    OmnString stmt = "select ";
    stmt << "change_type, "
		<< "table_name, "
		<< "xml from"
		<< sgTablename << " where seqno =" << mSeqno;
    return stmt;
}


OmnString 
AosDbTrackTable::retrieveStmt() const
{
	OmnString stmt = "select * from ";
	stmt << sgTablename << " where seqno =" << mSeqno;
	return stmt;
}


OmnString
AosDbTrackTable::removeStmt() const
{
	OmnString stmt = "delete from ";
	stmt << sgTablename << " where seqno=" << mSeqno;
	return stmt;
}


OmnString
AosDbTrackTable::removeAllStmt() const
{
	OmnString stmt = "delete from ";
	stmt << sgTablename;
	return stmt;
}


int
AosDbTrackTable::pollChanges(
		const u32 snappoint, 
		const OmnString &tablename)
{
	// It queries the table_changes table to see whether there is a record
	// whose tablename matches 'tablename' and seqno is greater than 
	// 'snappoint'. If yes, it returns the first newer seqno. 
	static OmnDataStorePtr store = OmnStoreMgrSelf->getStore();

	OmnString stmt = "select seqno from ";
	stmt << sgTablename 
		<< " where table_name = '" << tablename 
		<< "' and seqno > " << snappoint;
	OmnDbTablePtr table;
	OmnRslt rslt = store->query(stmt, table);
	aos_assert_r(rslt, -1);
	if (table->entries() <= 0) return 0;

	table->reset();
	OmnDbRecordPtr record = table->next();
	int seqno = record->getInt(0, -1, rslt);
	return seqno;
}


