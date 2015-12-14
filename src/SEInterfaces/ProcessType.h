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
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_ProcessType_h
#define AOS_SEInterfaces_ProcessType_h

#include "Util/String.h"

// 0 is for svrProxy. 1 is for admin proc.
#define AOSTAG_SVRPROXY_PID		0
#define AOSTAG_ADMIN_PID		1
#define AOSTAG_LOGIC_PID_START		2


#define	 AOSPROCESS_CUBE 		"cube"
#define	 AOSPROCESS_BKPCUBE		"bkpcube"
#define	 AOSPROCESS_TASK		"task"
#define	 AOSPROCESS_SERVICE		"service"
#define	 AOSPROCESS_FRONTEND	"frontend"
#define	 AOSPROCESS_ADMIN		"admin"
#define	 AOSPROCESS_TORTURER	"torturer"
#define	 AOSPROCESS_MSG			"msg"
#define	 AOSPROCESS_INDEXENGINE	"indexengine"
#define	 AOSPROCESS_DOCENGINE	"docengine"
#define	 AOSPROCESS_DOCSTORE	"docstore"

#include <map>
using namespace std;

class AosProcessType
{

public:
	enum E
	{
		eInvalid,
		
		eCube,
		eBkpCube,
		eFrontEnd,
		eAdmin,
		eTorturer,
		eTask,
		eService,
		eMsg,
		eIndexEngine,
		eDocEngine,
		eDocStore,

		eMax,
	};
	
	static bool isValid(const E type)
	{
		return type>eInvalid && type<eMax; 
	}
	
	static E 		 toEnum(const OmnString &tp_str);
	static OmnString toStr(const E code);
	static bool 	 addName(const OmnString &name, const E code);

	static map<OmnString, u32>& getProcTypeMap();

private:
	static void 	init();

};

#endif
