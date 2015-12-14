////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmGetIds.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Message/SmGetIds.h"

/* 
 * Commented out by Chen Ding, 08/23/2005
 * This class should be moved to SCVS.
 *
#include "Alarm/Alarm.h"
#include "DNS/Ptrs.h"
#include "RepRealm/Ptrs.h"
#include "Scvs/SmScvsCreateLde.h"
#include "Util/OmnNew.h"
#include "Util/SerialFrom.h"
#include "Util/SerialTo.h"


OmnMsgPtr		
OmnSmGetIds::createInst(OmnSerialFrom &s)
{
	OmnSmGetIds *msg = OmnNew OmnSmGetIds();
	msg->serializeFrom(s);
	return msg;
}


OmnString		
OmnSmGetIds::toString() const
{
    OmnString str;
	str << "\nSmGetIds Message: "
		<< "\n---------------"
		<< "\n    IdType:     " << mIdType
		<< "\n    IdStarting: " << mIdStarting
		<< "\n    NumIds:     " << mNumIds
		<< OmnMsg::toString();
	return str;
}


OmnRslt			
OmnSmGetIds::serializeFrom(OmnSerialFrom &s)
{
	try
	{
		s >> mIdType;
		s >> mIdStarting;
		s >> mNumIds;

		if (mIdType <= eFirstValidEntry || mIdType >= eLastValidEntry)
		{
			OmnAlarm << "Invalid IdType: " << mIdType << enderr;
			mIdType = eInvalid;
		}

		OmnMsg::serializeFrom(s);
	}
	
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

	return true;
}


OmnRslt			
OmnSmGetIds::serializeTo(OmnSerialTo &s) const
{
	try
	{
		s << OmnMsgId::eSmGetIds
		  << mIdType
		  << mIdStarting
		  << mNumIds;

		OmnMsg::serializeTo(s);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}
	
	return true;
}


OmnMsgPtr		
OmnSmGetIds::clone() const
{
	OmnSmGetIdsPtr msg = OmnNew OmnSmGetIds();
	msg.setDelFlag(false);
	OmnMsg::clone(msg.getPtr());

	msg->mIdType = mIdType;
	msg->mIdStarting = mIdStarting;
	msg->mNumIds = mNumIds;
	return msg.getPtr();
}
*/


