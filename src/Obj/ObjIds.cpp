////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjIds.cpp
// Description:
//	This class is used to maintain the maximum object IDs being issued
//  for each class. 
//
//  Object Ids consumed must be stored in database so that next time
//  when the program starts, it can continue from the previous point
//  so that the same object IDs are not reused. In order to reduce
//  database usage, we will get eThreshold number of object IDs 
//  from the database. This is called a batch. These Ids are stored
//  in memory. If they are not all consumed when the program stops, 
//  they will be wasted.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ObjIds.h"

#include "Alarm/Alarm.h"



OmnObjIds::OmnObjIds(const OmnClassId::E classId)
:
OmnObject(classId)
{
}


bool	
OmnObjIds::getBatchIds(const OmnClassId::E classId)
{
	OmnNotImplementedYet;
	return false;
}


/*
OmnRslt
OmnObjIds::serializeFromDb(const OmnObjType::E objType)
{
	OmnString stmt = "select high low from obj-ids where obj-type = " << objType;
    OmnDataStorePtr store = OmnStoreMgrSelf->getStore();

	OmnAssert(!store.isNull(), "No storage available!", false);

    OmnTraceDb << "Retrieve object Ids from db: \n" << stmt << endl;
    

	// 
	// Query it from the database. 
	//
	OmnDbRecordPtr record;
    OmnRslt rslt = database->query(stmt, record);
    if (!rslt || record.isNull())
    {
        OmnAlarm<< "Failed to retrieve object IDs: " << objType << enderr;
		return false;
    }

	// 
	// Found the record. Instantiate it from the record.
	//
	if (serializeFromDbRecord(record))
	{
		// 
		// Failed to construct the object from the record.
		//
		OmnAlarm << "Failed to construct object from record!" << enderr;
		return false;
	}

	// 
	// Update the database
	//

	return true;
}
*/

