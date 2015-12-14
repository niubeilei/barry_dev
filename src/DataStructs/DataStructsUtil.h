////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 08/14/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_DataStructsUtil_h
#define AOS_DataStructs_DataStructsUtil_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"

class AosDataStructsUtil
{
public:
	static int  getVirtualIdByStatid(
					const u64 &stat_id,
					const int docs_per);

	static int  getVirtualIdByGroupid(const u64 &stat_id);

	static AosXmlTagPtr pickStructProcJimoDoc(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &def);

	static u64  nextSysDocid(
					const AosRundataPtr &rdata,
					const int vid);
};
#endif
