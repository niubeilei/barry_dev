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
// 06/15/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ShortMsgUtil_ShmHandlerId_h
#define Omn_ShortMsgUtil_ShmHandlerId_h

#include "Util/String.h"


#define   AOSHANDLER_CONFIRM 				"1"
#define   AOSHANDLER_EMAIL2SHM 				"2"
#define   AOSHANDLER_MICROBLOG 				"3"
#define   AOSHANDLER_SHM2EMAIL 				"4"
#define   AOSHANDLER_SHM2SHM 				"5"
#define   AOSHANDLER_BROADCAST 				"6"


class AosShmHandlerId
{
public:
	enum E 
	{
		eInvalid,
		
		eHandlerConfirm,
		eEmail2Shm,
		eMicroblog,
		eShm2Email,
		eShm2Shm,
		eBroadcast,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E eid);
};
#endif

