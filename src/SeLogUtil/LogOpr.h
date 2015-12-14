////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/18/2011	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogUtil_LogOpr_h 
#define AOS_SeLogUtil_LogOpr_h 

#include "Util/String.h"


#define AOSLOGOPR_ADD				"add"
#define AOSLOGOPR_ADDVERSION		"addver"
#define AOSLOGOPR_RETRIEVE			"retrieve"
#define AOSLOGOPR_RETRIEVE_IILID	"retr_iilid"
#define AOSLOGOPR_RETRIEVE_LOGIDS	"retlogids"
#define AOSLOGOPR_RETRIEVE_VERSION	"ver_ret"
#define AOSLOGOPR_DELETE_VERSION	"del_ver"
#define AOSLOGOPR_MARK_VERSION		"mark_ver"
#define AOSLOGOPR_QUERY_LOG			"querylog"
#define AOSLOGOPR_RETR_LOGS			"retr_logs"

class AosLogOpr
{
public:
	enum E
	{
		eInvalid,

		eAddLog,
		eRetrieveLog,
		eRetrieveLogids,
		eRetrieveIILId,
		eQueryLogs,
		eAddVersion,
		eRetrieveVersion,
		eDeleteVersion,
		eMarkVersion,

		eMax
	};

	static E toEnum(const OmnString &name);
	static inline bool isValid(const E code) {return code > eInvalid && code < eMax;}
};

#endif
