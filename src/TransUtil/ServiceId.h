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
// 07/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransUtil_ServiceId_h
#define Aos_TransUtil_ServiceId_h

#define AOSSERVICEID_DOC_SERVER				"docserver"
#define AOSSERVICEID_IIL_SERVER				"iilserver"
#define AOSSERVICEID_BIGIIL_SERVER			"bigiilserver"
#define AOSSERVICEID_LOG_SERVER				"logserver"
#define AOSSERVICEID_STAT_SERVER			"statserver"
#define AOSSERVICEID_MSG_SERVER				"msgserver"
#define AOSSERVICEID_SHORTMSG_SERVER		"shortmsgserver"
#define AOSSERVICEID_TASK_SERVER			"taskserver"
#define AOSSERVICEID_SQL_SERVER				"sqlserver"
#define AOSSERVICEID_EMAIL_SERVER			"emailserver"

class AosServiceId
{
public:
	enum E
	{
		eInvalid,

		ePublicServer,
		eDocServer,
		eIILServer,
		eBigIILServer,
		eLogServer,
		eStatServer,
		eMessageServer,
		eShortMsgServer,
		eTaskServer,
		eSqlServer,
		eEmailServer,

		eMax
	};
};
#endif
