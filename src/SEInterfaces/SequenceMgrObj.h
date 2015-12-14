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
//
// Modification History:
// 2015/02/11 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SequenceMgrObj_h
#define Aos_SEInterfaces_SequenceMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSequenceMgrObj : virtual public OmnRCObject
{
	static AosSequenceMgrObjPtr smSequenceMgr;

public:
	static AosSequenceMgrObjPtr getSequenceMgr() {return smSequenceMgr;}
    static void setSequenceMgr(const AosSequenceMgrObjPtr &rhs) {smSequenceMgr = rhs;}

	virtual bool	getNextValue(
						const AosRundataPtr &rdata,
						const OmnString &name,
						int &value) = 0;

	virtual bool	getCrtValue(
						const AosRundataPtr &rdata,
						const OmnString &name,
						int &value) = 0;

	virtual bool	checkSequenceExist(
						const AosRundataPtr &rdata,
						const OmnString &name,
						const OmnString &func) = 0;

};

#endif

