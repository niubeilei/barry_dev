////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSys.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ObjSys.h"

#include "Alarm/Alarm.h"

OmnSysObj::OmnSysObj(const OmnClassId::E classId)
:
OmnObject(classId)
{
}


OmnSysObj::~OmnSysObj()
{
}


OmnClassId::E			
OmnSysObj::getMsgId(const unsigned char* data)
{
	// 
	// It assumes all SysObj is encoded as:
	// 	:dddxxxxxxxxxxxxx
	// where ':ddd' denotes the message ID. 
	//		
	if (!data)
	{
		OmnWarnProgError << "Null data" << enderr;
		return OmnClassId::eOmnInvalid;
	}

	if (data[0] != ':')
	{
		OmnWarnProgError << "Incorrect SysObj" << enderr;
		return OmnClassId::eOmnInvalid;
	}

	unsigned int id = data[1] + (data[2]<<8) + (data[3]<<16);

	if (OmnClassId::isValidMsg((OmnClassId::E)id))
	{
		OmnAlarm << OmnErrId::eProgError 
			<< "Invalid SysObjID: " << id 
			<< "max endty is " << OmnClassId::eLastValidEntry
			<< enderr;
		return OmnClassId::eOmnInvalid;
	}

	return (OmnClassId::E)id;
}
