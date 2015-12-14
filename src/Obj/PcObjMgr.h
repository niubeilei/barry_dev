////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PcObjMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Obj_PcObjMgr_h
#define Omn_Obj_PcObjMgr_h

#include "Obj/ObjMgr.h"



class OmnPcObjMgr : virtual public OmnObjMgr
{
public:
	OmnPcObjMgr(const OmnClassId::E classId);
	~OmnPcObjMgr();

	// 
	// OmnObject interface
	//
	virtual OmnClassId::E	getClassId() const {return OmnClassId::eOmnPcObjMgr;}
};
#endif
