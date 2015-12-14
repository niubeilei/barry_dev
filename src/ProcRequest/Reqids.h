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
// 2011/07/28	Created by Jackie zhao
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_ProcReqids_h
#define Omn_ReqProc_ProcReqids_h

#include "Util/String.h"



class AosProcReqid
{
public:
	enum E 
	{
		eInvalid,

		eServiceCmds,
		eRegisterService,
		
		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif

