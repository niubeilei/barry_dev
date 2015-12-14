////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ClassId.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ClassId.h"


void
OmnClassId::initNames()
{
	for (int i=0; i<eLastValidEntry; i++)
	{
		mClassIds[i][0] = 0;
	}
	mNamesInit = true;
}


