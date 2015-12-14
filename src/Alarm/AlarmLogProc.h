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
// 12/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Alarm_AlarmLogProc_h
#define Omn_Alarm_AlarmLogProc_h

#include "Rundata/Rundata.h"
#include "Util/Object.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class OmnAlarmLogProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool addLog(const OmnString &ctnr_objid, 
						const AosXmlTagPtr &log) = 0;
};

#endif
