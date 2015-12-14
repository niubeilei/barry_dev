////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjPersist.h
// Description:
//	A Persistent object is an object that can be stored in a media, 
//  such as local or remote files, local or remote databases, 
//  data centers, data portals, etc. 
//  
//  This class defines an interface. Any object that wants to be persistent
//  should derive from this class. 
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_PersistObj_h
#define Omn_Obj_PersistObj_h

#include "Obj/Obj.h"


class OmnPersistObj : virtual public OmnObj
{
public:
	OmnPersistObj();
	virtual ~OmnPersistObj();

	virtual bool	serializeTo() const = 0;
    virtual bool    serializeFrom() = 0;
	virtual bool	serializeTo(const OmnString &storeId) const = 0;
    virtual bool    serializeFrom() = 0;
};
#endif
