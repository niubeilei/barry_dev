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
// 08/03/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Book/Container.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Book/TagMgr.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "TinyXml/tinyxml.h"
#include "util_c/strutil.h"
#include "Util/File.h"

const OmnString sgSelectStmt = 
	"select tname,name,tags,description,creator,"
   	"unix_timestamp(create_time) from containers";


AosContainer::AosContainer()
{
	mType = "table";
}


// 
// Description:
// This member function retrieves the named container definition. 
// If not found or
// if there are errors, it returns false. Otherwise, it returns
// true and the object is retrieved to 'this'.
//
bool
AosContainer::getContainer(
		const u32 id, 
		AosXmlRc &errcode, 
		OmnString &errmsg)
{
	errcode = eAosXmlInt_General;

    OmnString stmt =  sgSelectStmt;
	stmt << " where seqno=";
	stmt << id;
	
	OmnTrace << "To retrieve Container: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		errmsg = "Failed to retrieve container: ";
		errmsg << stmt << " from db!";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	rslt = serializeFromRecord(record);
	if (rslt)
	{
		errcode = eAosXmlInt_Ok;
		return true;
	}
	return false;
}


OmnRslt
AosContainer::serializeFromRecord(const OmnDbRecordPtr &record)
{
	aos_assert_r(record, false);
	OmnRslt rslt(true);

	mDataid 		= record->getStr(0, "", rslt);
	mName 			= record->getStr(1, "", rslt);
	mTags 			= record->getStr(2, "", rslt);
	mDesc 			= record->getStr(3, "", rslt);
	mCreator 		= record->getStr(4, "", rslt);
	mCreateTime 	= record->getU32(5, 0, rslt);
	return rslt;
}


OmnRslt
AosContainer::serializeFromDb()
{
	// 
	// It retrieves a user from the database.
	// 
    OmnString stmt = sgSelectStmt;
	stmt << " where tname=\"" << mDataid << "\"";
	
	OmnTrace << "To retrieve Container: " << stmt << endl;

	OmnDbRecordPtr record;
	OmnRslt rslt = retrieveRecord(stmt, record);
	if (!rslt)
	{
		OmnAlarm << "Failed to retrieve container: " 
			<< stmt << " from db!" << enderr;
		return false;
	}

	aos_assert_r(record, false);

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


bool
AosContainer::createContainer()
{
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);

	u64 start;
	store->getDbId("tablename_seqno", start);
	mDataid = "_tb_";
	mDataid << start;	

	OmnString stmt = "insert into containers (tname, name, tags, "
		"description, creator, create_time) values (\"";
	stmt << mDataid << "\", \""
		<< mName << "\", \""
		<< mTags << "\", \""
		<< mDesc << "\", \""
		<< mCreator << "\", now())";

	OmnTrace << "To create container entry: " << stmt << endl;
	OmnRslt rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);

	getCreateTime();
	return true;
}


bool
AosContainer::getCreateTime()
{
	OmnString stmt = "select unix_timestamp(create_time) from containers "
		"where tname=\"";
	stmt << mDataid << "\"";

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);
	OmnDbRecordPtr record;
	OmnRslt rslt = store->query(stmt, record);
	aos_assert_r(rslt, false);
	aos_assert_r(record, false);
	mCreateTime = record->getU32(0, 0, rslt);
	return true;
}


const OmnString sgCreateTable = "("
	"seqno bigint auto_increment primary key,"
    "dataid varchar(32),"
	"value text,"
	"name text,"
	"description text)";	

bool
AosContainer::createContainer(
		const OmnString &name, 
		const OmnString &tags,
		const OmnString &desc,
		const OmnString &creator, 
		AosXmlRc &errcode, 
		OmnString &errmsg) 
{
	errcode = eAosXmlInt_General;

	mName = name;
	mTags = tags;
	mDesc = desc;
	mCreator = creator;

	// 1. Create an entry in the container table
	bool rr = createContainer();
	aos_assert_r(rr, false);

	// 2. Create the table
	OmnString stmt = "create table ";
	stmt << mDataid << sgCreateTable;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	aos_assert_r(store, false);

	OmnRslt rslt = store->runSQL(stmt);
	aos_assert_r(rslt, false);

	// 3. Add entries to tag_table
	AosTagMgr tag_mgr;
	tag_mgr.procTags(tags, *this);

	errcode = eAosXmlInt_Ok;
	return true;
}

