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
// 3/30/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MsgUtil_AmRespCode_h
#define Aos_MsgUtil_AmRespCode_h

class AosAmRespCode
{

public:
	enum E
	{
		eAllowed = 0,
		eDenied = 1,
		eUnAuthed,
		eUnknown,
		eInternalError
	};
};
#endif

