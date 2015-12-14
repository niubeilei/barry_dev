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
#include "Book/Webbook.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "TinyXml/tinyxml.h"
#include "util_c/strutil.h"
#include "DataStore/DbTrackTable.h"


const char sgWebbookTypeChar = 'W';
const OmnString sgTablename = "webbook";

const OmnString sgObjTableStmt = "("
	"name bigint auto_increment not null primary key, "
	"xml text)";

const OmnString sgCompTableStmt = "("
	"compid varchar(32) not null primary key, "
	"name varchar(32), "
	"type varchar(8), "
	"descrip text, "
	"xml text)";

AosWebbook::AosWebbook()
:
AosBook(AosBookType::eWebbook, sgTablename)
{
}

AosWebbook::~AosWebbook()
{
}


OmnRslt
AosWebbook::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);
	
	mBookId		 = record->getStr(0, "", rslt);
	mBookType	 = record->getChar(1, AosBookType::eWebbook, rslt);
    mName 	  	 = record->getStr(2, "NoName", rslt);
    mKeywords 	 = record->getStr(3, "", rslt);
    mDesc		 = record->getStr(4, "", rslt);
    mXml      	 = record->getStr(5, "", rslt);

	return rslt;
}


OmnString
AosWebbook::updateStmt() const
{
    OmnString stmt = "update webbook set ";
	stmt << "type = '" << sgWebbookTypeChar << "', name='"
		 << mName << "', keywords='"
		 << mKeywords << "', descrip='"
		 << mDesc << "', xml='"
		 << mXml << "' where bookid='" << mBookId << "'";

	return stmt;
}


OmnString
AosWebbook::insertStmt() const
{
    //
    // It creates a SQL statement to insert a record
	// into the database.
    //
    OmnString stmt = "insert into ";
	stmt << mTablename << "("
		"bookid, "
		"type, "
        "name, "
        "keywords, "
		"descrip, "
        "xml, "
		"objid) values ('";

    stmt << mBookId << "', '"
		 << sgWebbookTypeChar << "', '"
		 << mName << "', '"
		 << mKeywords << "', '"
		 << mDesc << "', '"
         << mXml << "', '"
		 << mObjid << "')";

    return stmt;
}


OmnString
AosWebbook::existStmt() const
{
    OmnString stmt = "select ";
    stmt << "bookid, "
		<< "type, "
		<< "name, "
		<< "keywords, "
		<< "descrip, "
		<< "xml from"
		<< mTablename << " where bookid='" << mBookId << "'";
    return stmt;
}


OmnString 
AosWebbook::retrieveStmt() const
{
	OmnString stmt = "select * from ";
	stmt << mTablename << " where bookid='" << mBookId << "'";
	return stmt;
}


bool		
AosWebbook::createBook(
		TiXmlElement *bookdef, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// It creates a webbook. The parm 'bookdef' shall be in the form:
	// 	<def bookname="xxx"
	// 		 keywords="xxx"
	// 		 description="xxx"
	// 		 ...
	// 	/>
	errcode = eAosXmlInt_SyntaxError;

	aos_assert_r(bookdef, false);

	// Add an entry to the table identified by 'mTablename'
	mBookId = getNewBookId();
	mName = bookdef->Attribute("bookname");
	mKeywords = bookdef->Attribute("keywords");
	mDesc = bookdef->Attribute("description");
	OmnTrace << "To create a book: " << mBookId << endl;
	OmnRslt rslt = addToDb();
	aos_assert_r(rslt, false);

	// Add an change entry to 'table_changes'
	AosDbTrackTable tracktable(AosDbTrackTable::eAdd, sgTablename);
	tracktable.startAssemble();
	tracktable.addField("bookid", mBookId, false);
	tracktable.addField("type", mBookType);
	tracktable.addField("name", mName, false);
	tracktable.addField("keywords", mKeywords, true);
	tracktable.addField("descrip", mDesc, true);
	tracktable.addField("xml", mXml, true);
	tracktable.endAssemble();
	tracktable.addToDb();

	// Create the root of the table name 
	char *parts[2];
	int num = aos_str_split(mBookId.data(), '_', parts, 2);
	aos_assert_r(num, 2);
	OmnString tableroot = parts[0];
	tableroot << parts[1];

	// Get the data store
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);

	// Create the object table
	OmnString stmt = "create table ";
	stmt << tableroot << "_objs" << sgObjTableStmt;
	OmnTrace << "To create the object table: " << stmt << endl;
	rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);
	
	// Create the component table
	stmt = "create table ";
	stmt << tableroot << "_comps" << sgCompTableStmt;
	OmnTrace << "To create the component table: " << stmt << endl;
	rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);	

	errcode = eAosXmlInt_Ok;
	aos_str_split_releasemem(parts, 2);
	return true;
}


bool		
AosWebbook::removeBook(
		TiXmlElement *bookdef, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	// It removes a webbook. The parm 'bookdef' shall be in the form:
	// 	<def bookid="xxx"
	// 		 ...
	// 	/>
	errcode = eAosXmlInt_SyntaxError;
	aos_assert_r(bookdef, false);

	// Get the data store
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	if (!store)
	{
		errmsg = "[";
		errmsg << __FILE__ << ":" << __LINE__ << "] Failed to retrieve"
			<< " the database object (internal error)";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	// Retrieve the book ID
	const char *bid = bookdef->Attribute("bookid");
	if (!bid)
	{
		errmsg = "[";
		errmsg << __FILE__ << ":" << __LINE__ << "] "
			<< "Missing the Book ID (Internal error)"; 
		OmnAlarm << errmsg << enderr;
		return false;
	}
	mBookId = bid;

	// Create the root of the table name 
	char *parts[2];
	int num = aos_str_split(mBookId.data(), '_', parts, 2);
	aos_assert_r(num, 2);
	OmnString tableroot = parts[0];
	tableroot << parts[1];

	// Remove the entry from the table identified by 'mTablename'
	OmnString stmt = "delete from ";
	stmt << mTablename << " where bookid='" << mBookId << "'";
	OmnRslt rslt = store->runSQL(stmt);
	if (!rslt)
	{
		errmsg = "[";
		errmsg << __FILE__ << ":" << __LINE__ << "] "
			<< "Failed to delete the record (Internal Error)"; 
		OmnAlarm << errmsg << enderr;
		aos_str_split_releasemem(parts, 2);
		return false;
	}

	// Add an change entry to 'table_changes'
	AosDbTrackTable tracktable(AosDbTrackTable::eDelete, sgTablename);
	tracktable.startAssemble();
	tracktable.addField("bookid", mBookId, false);
	tracktable.endAssemble();
	tracktable.addToDb();

	// Drop the object table
	OmnString tname = tableroot;
   	tname << "_objs";
	rslt = store->dropTable(tname);
	if (!rslt)
	{
		errmsg = "[";
		errmsg << __FILE__ << ":" << __LINE__ << "] "
			<< "Failed to drop the object table(Internal Error)"; 
		OmnAlarm << errmsg << enderr;
		aos_str_split_releasemem(parts, 2);
		return false;
	}

	// Drop the component table
	tname = tableroot;
	tname << "_comps";
	rslt = store->dropTable(tname);
	if (!rslt)
	{
		errmsg = "[";
		errmsg << __FILE__ << ":" << __LINE__ << "] "
			<< "Failed to drop the component table(Internal Error)"; 
		OmnAlarm << errmsg << enderr;
		aos_str_split_releasemem(parts, 2);
		return false;
	}

	errcode = eAosXmlInt_Ok;
	aos_str_split_releasemem(parts, 2);
	return true;
}

