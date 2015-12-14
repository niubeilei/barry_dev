////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NotInDbObj.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_NonExistObj_h
#define Omn_Obj_NonExistObj_h

#include "Obj/MgdDbObj.h"

class OmnNotInDbObj : public OmnMgdDbObj
{
private:
	int		mTimeCreated;

public:
	OmnNotInDbObj();
	virtual ~OmnNotInDbObj();

	// 
	// OmnObject interface
	//
	virtual OmnClassId::E		getClassId() const;

	// 
	// OmnMgdDbObj interface
	//
	virtual OmnMgdDbObjPtr	clone() const;

	virtual OmnString		toString() const;
};
#endif
