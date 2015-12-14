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
// 01/10/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "ObjSet/ObjSetBasic.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Event/EventObjCreated.h"
#include "Event/EventObjDeleted.h"
#include "Event/EventObjModified.h"
#include "Event/EventObjRenamed.h"
#include "Event/EventObjRecreated.h"
#include "RVG/Table.h"
#include "Util/OmnNew.h"

AosObjSetBasic::AosObjSetBasic(const std::string &className, 
							   AosRecordCreateFunc creationFunc, 
							   const u32 max)
:
mClassName(className),
mRecordCreator(creationFunc),
mMax(max),
mDeleteFlag(eDefaultDeleteFlag),
mRecreateFlag(eDefaultRecreateFlag),
mModifyFlag(eDefaultModifyFlag)
{
	aos_assert(mClassName.length());
	aos_assert(mRecordCreator);
	aos_assert(max);

	mTable = OmnNew AosTable();
	aos_assert(mTable);
}


AosObjSetBasic::~AosObjSetBasic()
{
}


AosEventRc::E	
AosObjSetBasic::procEvent(const AosEventPtr &event)
{
	switch (event->getEventId())
	{
	case AosEventId::eObjCreated: 
		 return objCreated(dynamic_cast<AosEventObjCreated *>(event.getPtr()));

	case AosEventId::eObjDeleted: 
		 return objDeleted(dynamic_cast<AosEventObjDeleted *>(event.getPtr()));

	case AosEventId::eObjRenamed: 
		 return objRenamed(dynamic_cast<AosEventObjRenamed *>(event.getPtr()));

	case AosEventId::eObjRecreated: 
		 return objRecreated(dynamic_cast<AosEventObjRecreated *>(event.getPtr()));

	case AosEventId::eObjModified: 
		 return objModified(dynamic_cast<AosEventObjModified *>(event.getPtr()));

	default:
		 OmnAlarm << "Event not regiestered: " << event->toString() << enderr;
		 return AosEventRc::eContinue;
	}

	aos_should_never_come_here;
	return AosEventRc::eContinue;
}


// 
// 1. If an object is not already in the object set, add the object. 
//    If the object set has a max limitation and the max is reached,
//    it is an error.
// 2. If it is a recreated object and if mRecreateFlag == true, the 
//    object is added. Otherwise, if an instance of the object is 
//    already in the object set, this object replaces that object.
//
AosEventRc::E
AosObjSetBasic::objCreated(const AosEventObjCreatedPtr &event)
{
	if (mTable->isFull())
	{
		OmnAlarm << "Too many objects: " << mTable->getMax() << enderr;
		return AosEventRc::eContinue;
	}

	aos_assert_r(event, AosEventRc::eStop);
	aos_assert_r(event->getClassName() == mClassName, AosEventRc::eStop);
	aos_assert_r(mRecordCreator, AosEventRc::eContinue);	
	AosRecord record;
   	mRecordCreator(event->getEventData(), record);

	// 
	// Note: currently we simply append the record. In the future,
	// we may have to consider the order.
	//
	if (mRecreateFlag)
	{
		// Just add the object.
		if (!mTable->addRecord(record))
		{
			// 
			// There are too many records.
			//
			OmnAlarm << "Failed to add record" << enderr;
			return AosEventRc::eContinue;
		}
	}
	else
	{
		if (!mTable->replace(record, true))
		{
			OmnAlarm << "Failed to append the record" << enderr;
			return AosEventRc::eContinue;
		}
	}

	return AosEventRc::eContinue;
}


// 
// 1. If mDeleteFlag == true, the object is not removed from the
//    object set. Otherwise, it is removed.
//
AosEventRc::E
AosObjSetBasic::objDeleted(const AosEventObjDeletedPtr &event)
{
	if (!mDeleteFlag) return AosEventRc::eContinue;

	aos_assert_r(event, AosEventRc::eStop);
	aos_assert_r(event->getClassName() == mClassName, AosEventRc::eStop);
	aos_assert_r(mRecordCreator, AosEventRc::eContinue);	
	AosRecord record;
   	mRecordCreator(event->getEventData(), record);

	if (!mTable->removeRecord(record))
	{
		// 
		// The object is not in the table
		//
		OmnAlarm << "To remove a record but not in the table" << enderr;
	}

	return AosEventRc::eContinue;
}


// 
// 1. If mModifyFlag == true, just add the modified object.
// 2. If mModifyFlag == false, remove the old object and 
//    add the new one.
//
AosEventRc::E
AosObjSetBasic::objModified(const AosEventObjModifiedPtr &event)
{
	aos_assert_r(event, AosEventRc::eStop);
	aos_assert_r(event->getClassName() == mClassName, AosEventRc::eStop);
	aos_assert_r(mRecordCreator, AosEventRc::eContinue);	

	if (mModifyFlag)
	{
		// 
		// Just add the modified object
		//
		AosRecord modifiedObj;
   		mRecordCreator(event->getModifiedObj(), modifiedObj);
		if (!mTable->addRecord(modifiedObj))
		{
			// 
			// The object is not in the table
			//
			OmnAlarm << "To remove a record but not in the table" << enderr;
		}
	}
	else
	{
		// 
		// Replace the object
		//
		AosRecord oldObj, modifiedObj;
   		mRecordCreator(event->getEventData(), oldObj);
   		mRecordCreator(event->getModifiedObj(), modifiedObj);
		aos_assert_r(mTable->removeRecord(oldObj), AosEventRc::eContinue);
		aos_assert_r(mTable->addRecord(modifiedObj), AosEventRc::eContinue);
	}

	return AosEventRc::eContinue;
}

