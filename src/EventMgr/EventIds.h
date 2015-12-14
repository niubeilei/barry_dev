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
// 2011/01/20	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_EventMgr_EventIds_h
#define Omn_EventMgr_EventIds_h

#include "Util/String.h"

struct AosEventId
{
	enum E
	{
		eInvalid, 

		eAddMember,
		eRemoveMember,
		eMemberModified,

		eReadDoc,
		eModifyDoc,
		eDeleteDoc,
		eCreateDoc,

		eReadAttr,
		eModifyAttr,
		eDeleteAttr,
		eAddAttr,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &id);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};
#endif
#endif

