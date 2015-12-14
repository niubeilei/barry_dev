////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DbRecord.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Database/DbRecord.h"

#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"



OmnDbRecord::OmnDbRecord()
{
}


OmnDbRecord::~OmnDbRecord()
{
}


bool
OmnDbRecord::retrieveRecord(const OmnString &stmt, OmnDbRecordPtr &record) 
{
	// Retrieve the data store.
    //OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
    OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	if (store.isNull())
	{
		OmnAlarm << "Failed retrieve store" << enderr;
		return false;
	}
		
  	OmnScreen << "Query database: \n" << stmt 
		<< " from store: " << store->getName() << endl;

	// Query the database. 
    OmnRslt rslt = store->query(stmt, record);
    if (!rslt)
	{
		// Failed to retrieve. 
		return rslt;
	}
	
	return true;
}

