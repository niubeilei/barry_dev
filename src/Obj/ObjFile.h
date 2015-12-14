////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjFile.h
// Description:
//	A file object is an object that can be stored in a file.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_FileObj_h
#define Omn_Obj_FileObj_h

#include "Obj/Obj.h"


class OmnFileObj : virtual public OmnObj
{
public:
	OmnFileObj();
	virtual ~OmnFileObj();

    virtual bool	serializeToFile(const OmnString &dir,
									const OmnString &filename) const = 0;
	virtual bool	serializeToFile() const = 0;
    virtual bool    serializeFromFile(const OmnString &dir,
									const OmnString &filename) = 0;
    virtual bool    serializeFromFile() = 0;
};
#endif
