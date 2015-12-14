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
#include "Book/Book.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "TinyXml/tinyxml.h"
#include "util_c/strutil.h"
#include "Util/File.h"



AosBook::AosBook(const char type, const OmnString &tname) 
:
mBookType(type),
mObjid(eDefaultObjid),
mCrtDbId(1),
mCrtDbIdMax(0),
mBlockSize(5),
mTablename(tname)
{
}


// 
// Description:
// This member function retrieves the named book. If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosBook::getBook(const OmnString &bookid)
{
	// 
	// It retrieves the book identified by "bookid" from the database.
	// Note that bookid shall be in the form:
	// 	<tablename>_nnn
	// 
	char *parts[2];
	const char *data = bookid.data();
	int num_parts = aos_str_split(data, '_', parts, 2);

    OmnString stmt = "select * from ";
	stmt << parts[0] << " where ";
    stmt << "bookid='" << bookid << "'";
	aos_str_split_releasemem(parts, num_parts);
	
	OmnTrace << "To retrieve Book: " << stmt << endl;

	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve Book: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(table, false);
	if (table->entries() > 1)
	{
		OmnAlarm << "Found multiple books with the same bookid: " << bookid 
			<< ". Number of objects found: " << table->entries() << enderr;
		return false;
	}

	if (table->entries() == 0)
	{
		// Did not find the book 
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
AosBook::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = "select * from ";
    stmt <<	mTablename 
		 << " where bookid='"
    	 << mBookId << "'";
	
	OmnTrace << "To retrieve Book: " << stmt << endl;

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

	if (table->entries() == 0)
	{
		// Did not find the book 
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
AosBook::removeAllStmt() const
{
    OmnString stmt = "delete from ";
	stmt << mTablename;
    return stmt;
}



OmnString
AosBook::getNewAttrName()
{
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore(OmnStoreId::eGeneral);
	u64 id;
	store->getDbId("bookid", id);	
	OmnString aname = "a";
	aname << id;
	return aname;
}


OmnString
AosBook::getNewBookId()
{
	// It objects a new book id from the database. It assumes there is a 
	// table whose name is "bookid". The table has two fields:
	// 	id:		value = "1"
	// 	bookid:	bigint
	// Book IDs are in the form: 
	// 	tablename_nnnn
	// where 'tablename' is the name of the table that stores book entries
	// and 'nnnn' is the integer retrieved by this function.
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore(OmnStoreId::eGeneral);
	aos_assert_r(store, "");
	u64 bookid;
	bool rslt = store->getDbId("bookid", bookid);
	aos_assert_r(rslt, 0);
	OmnString bkid = mTablename;
	bkid << "_" << bookid;
	return bkid;
}


OmnString
AosBook::getObjid()
{
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore(OmnStoreId::eGeneral);
	aos_assert_r(store, "");

	OmnString str = "obj";
	u64 id;
	store->getDbId("bookid", id);
	str << id;
	return str;
}


bool		
AosBook::addObjects(TiXmlElement *obj, 
					TiXmlElement *viewer, 
					TiXmlElement *editor, 
					AosXmlRc &errcode, 
					OmnString &errmsg)
{
	OmnTrace << "To add objects: " << endl;

	// Book IDs are always in the form:
	// 	xxxx_yyyy
	// where 'xxxx' is the book information table. There shall be one and only
	// one record in that table. It contains the book's information. Each book
	// is further defined by the following tables:
	// 	xxxxyyyy_objs	Contains all the objects for the book
	// 	xxxxyyyy_comps	Contains all the components for the book

	errcode = eAosXmlInt_Ok;

	// Get the store
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore(OmnStoreId::eGeneral);
	aos_assert_r(store, false);

	// Set the object IDs for 'viewer' and 'editor'
	OmnString viewer_id = getObjid();
	OmnString editor_id = getObjid();
	viewer->SetAttribute("objid", viewer_id.data());
	editor->SetAttribute("objid", editor_id.data());
	obj->SetAttribute("viewer_vpd", viewer_id.data());
	obj->SetAttribute("editor_vpd", editor_id.data());
	obj->GetDocument()->Print();
	viewer->GetDocument()->Print();
	editor->GetDocument()->Print();

	char *parts[2];
	aos_str_split(mBookId.data(), '_', parts, 2);

	// Save 'viewer'
	OmnString comps_tablename = parts[0];
	comps_tablename << parts[1] << "_comps";
	viewer->GetDocument()->SaveFile(mBookId.data());
	OmnFile ff1(mBookId.data(), OmnFile::eReadOnly);
	OmnString viewer_contents;
	ff1.readAll(viewer_contents);

	OmnString stmt = "insert into ";
	stmt << comps_tablename << " (name, type, xml) values ('"
		<< viewer_id << "', 'V', '" << viewer_contents << "')";
	OmnRslt rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);

	// Save 'editor'
	editor->GetDocument()->SaveFile(mBookId.data());
	OmnFile ff2(mBookId.data(), OmnFile::eReadOnly);
	OmnString editor_contents;
	ff2.readAll(editor_contents);

	stmt = "insert into ";
	stmt << comps_tablename << " (name, type, xml) values ('"
		<< editor_id << "', 'E', '" << editor_contents << "')";
	rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);
	
	// Save 'obj'
	OmnString obj_tablename = parts[0];
	obj_tablename << parts[1] << "_objs";
	obj->GetDocument()->SaveFile(mBookId.data());
	OmnFile ff(mBookId.data(), OmnFile::eReadOnly);
	OmnString obj_contents;
	ff.readAll(obj_contents);

	stmt = "insert into ";
	stmt << obj_tablename << " (xml) values ('" << obj_contents << "')";
	rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);

	aos_str_split_releasemem(parts, 2);
	return true;
}


OmnString
AosBook::removeStmt() const
{
    OmnString stmt = "delete from ";
	stmt << mTablename << " where bookid ='" << mBookId << "'";
    return stmt;
}


bool
AosBook::updateXml(const OmnString &contents, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	if (mBookId == "")
	{
		errcode = eAosXmlInt_General;
		errmsg = "Missing book ID";
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	mXml = contents;
	OmnString stmt = "update ";
	stmt << mTablename << " set xml='" << contents 
		<< "' where bookid='" << mBookId << "'";
	OmnRslt rslt = updateToDb(stmt);
	if (!rslt)
	{
		errcode = eAosXmlInt_General;
		errmsg = "Failed to update to the database";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return true;
}


bool
AosBook::createHomepage(
		const OmnString &vpd, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	return updateXml(vpd, errcode, errmsg);
}

