////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSys.h
// Description:
//	This is the super class for all SysObj. Any persistent data and 
//  data that are visible from ther applications must be implemented
//  as a SysObj, i.e., derived from this class. All such data are
//  managed by OmnDataMgr.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_SysObj_h
#define Omn_Obj_SysObj_h

#include "Util/Object.h"

class OmnSerialFrom;
class OmnSerialTo;


class OmnSysObj : virtual public OmnObject
{
public:
	OmnSysObj(const OmnClassId::E classId);
	virtual ~OmnSysObj();

    virtual bool 	    serializeTo(OmnSerialTo &s) const = 0;
    virtual bool        serializeFrom(OmnSerialFrom &s) = 0;

	static OmnClassId::E	getMsgId(const unsigned char* data);

};
#endif
