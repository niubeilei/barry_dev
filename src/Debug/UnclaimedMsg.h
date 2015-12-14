////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UnclaimedMsg.h
// Description:
//    This is a SingletonClass.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Debug_UnclaimedMsg_h
#define Omn_Debug_UnclaimedMsg_h


class OmnUnclaimedMsg  OmnDeriveFromObj
{
public:
	OmnUnclaimedMsg();
	~OmnUnclaimedMsg();

	static int	log(const OmnString &fileName, 
			        const int lineNo,
			        const OmnMsg &msg,
			        const OmnString &errMsg);
};
#endif
