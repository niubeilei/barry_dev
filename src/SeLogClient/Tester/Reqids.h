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
//
// Modification History:
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SeLogClient_Tester_LogTestReqids_h
#define Aos_SeLogClient_Tester_LogTestReqids_h

#include "Util/String.h"



class AosLogReqid
{
public:
	enum E 
	{
		eInvalid,
		
		eLogin,
		
		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};
#endif

