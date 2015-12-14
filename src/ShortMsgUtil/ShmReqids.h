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
#ifndef Omn_ShortMsgUtil_ShmReqids_h
#define Omn_ShortMsgUtil_ShmReqids_h

#include "Util/String.h"

#define AOSSHORTMSGREQ_SENDMSG			"sendshortmsg"
#define AOSSHORTMSGREQ_SHMCONFIRM		"shortmsgconfirm"
#define AOSSHORTMSGREQ_E2SHM		    "email2shm"

class AosShmReqid
{
public:
	enum E 
	{
		eInvalid,

		eSendShortmsg,
		eShmConfirm,
		eEmail2Shm,
		
		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e);
};
#endif

