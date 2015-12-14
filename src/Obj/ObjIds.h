////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjIds.h
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

#ifndef Omn_Obj_ObjIds_h
#define Omn_Obj_ObjIds_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Util/Object.h"
#include "Obj/ObjId.h"


class OmnObjIds : virtual public OmnObject
{
private:
	struct Entry
	{
		OmnObjId	mStart;
		int			mIndex;
	};

	enum 
	{
		eThreshold = 100
	};

	Entry		mObjIds[OmnClassId::eLastValidEntry];

public:
	OmnObjIds(const OmnClassId::E classId);
	virtual ~OmnObjIds() {}

	OmnObjId	nextId(const OmnClassId::E classId)
	{
		OmnCheckAReturn(OmnClassId::isValid(classId), 
			OmnString("Invalid Class ID: ") << classId, OmnObjId::getInvalidObjId());

		if (mObjIds[classId].mIndex >= eThreshold)
		{
			// 
			// Time to get another batch of the IDs.
			//
			if (!getBatchIds(classId))
			{
				// 
				// Failed to retrieve object IDs.
				//
				OmnAlarm << "Failed to retrieve object IDs: " << classId << enderr;
				return OmnObjId::getInvalidObjId();
			}

			mObjIds[classId].mIndex = 0;
		}

		return mObjIds[classId].mStart + mObjIds[classId].mIndex++;
	}

private:
	bool	getBatchIds(const OmnClassId::E classId);
};
#endif
