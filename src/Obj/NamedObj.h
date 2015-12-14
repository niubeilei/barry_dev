////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NamedObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_NamedObj_h
#define Omn_Obj_NamedObj_h

#include "Obj/MngdObj.h"


class OmnNamedObj : public OmnMngdObj
{
public:
	virtual OmnString	getObjName() const = 0;
};
#endif
