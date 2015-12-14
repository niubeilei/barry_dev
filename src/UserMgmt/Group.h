////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 8/18/2010: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_UserMgmt_Group_h
#define Aos_UserMgmt_Group_h


#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/StrParser.h"
#include "Rundata/Ptrs.h"

class AosGroup : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:


public:
    AosGroup();
    ~AosGroup();

	bool	deleteGroups(const AosRundataPtr &rdata);
	bool	createGroups(const AosRundataPtr &rdata);
	bool 	modifyGroups(const AosRundataPtr &rdata);
	bool 	addGroup(const AosRundataPtr &rdata, const OmnString &meNew);
	bool 	delGroup(const AosRundataPtr &rdata, const OmnString &meOld );
	OmnString removeSame(const OmnString &str1, const OmnString &str2);

private:

};
#endif

