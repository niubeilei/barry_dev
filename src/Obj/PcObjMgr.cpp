////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PcObjMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/PcObjMgr.h"


OmnPcObjMgr::OmnPcObjMgr(const OmnClassId::E classId)
:
OmnObject(classId),
OmnObjMgr(classId)
{
}


OmnPcObjMgr::~OmnPcObjMgr()
{
}


