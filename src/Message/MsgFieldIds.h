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
// 2013/03/02 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Message_MsgFieldIds_h
#define Omn_Message_MsgFieldIds_h

#include "Util/String.h"

class AosMsgFieldId
{
public:
	enum E
	{
		eInvalidFieldId,

		eMsgId,
		eSendCubeId,
		eRecvCubeId,
		eSendPhysicalId,
		eRecvPhysicalId,
		eMsgVersion,
		eVersion,
		eSiteId,
		eAppname,
		eCloudId,
		eUserId,
		eSSID,

		eLastFieldId
	};

	static bool	isValid(const E code) 
	{
		return code > eInvalidFieldId && code < eLastFieldId;
	}
	
	static int getNumFields() {return eLastFieldId;}
	static E getUDF() {return (E)(eLastFieldId + 10);}
};
#endif
