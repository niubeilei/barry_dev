////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSync.h
// Description:
//	A synchronizationable object is an object that can be synchronized
//  with other applications and/or data store.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_SyncObj_h
#define Omn_Obj_SyncObj_h

#include "Util/Object.h"


class OmnSyncObj : virtual public OmnObject
{
public:
	OmnSyncObj(const OmnClassId::E classId);
	virtual ~OmnSyncObj();

    virtual bool	syncTo() const = 0;
	virtual bool	syncFrom() const = 0;
};
#endif
