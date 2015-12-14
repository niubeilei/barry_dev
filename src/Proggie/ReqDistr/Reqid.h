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
// 2010/12/28	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Proggie_ReqDistr_Reqid_h
#define AOS_Proggie_ReqDistr_Reqid_h

#include "Util/String.h"


class AosReqid
{
public:
	enum E
	{
		eInvalid,
		eNotifyUser,
		eUserVerification,
		eMax
	};

	static E toEnum(const OmnString &name);
};
#endif

