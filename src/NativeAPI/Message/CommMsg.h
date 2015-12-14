////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommMsg.h
// Description:
//	Almost everything is a message, all derived from OmnMsg. An OmnCommMsg
//  is a message that is used to communicate information. This is 
//  different from an SO, which is itself a data, not necessarily a 
//  message. The reason why SO is also called message is that they
//  share many things.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_CommMsg_h
#define Omn_Message_CommMsg_h



class OmnCommMsg : public OmnMsg
{
private:
	OmnCallID		mCallId;

public:
	OmnCommMsg();
	OmnCommMsg(const OmnCommMsg &rhs);
	OmnCommMsg & operator = (const OmnCommMsg &rhs);
	~OmnCommMsg();

	virtual bool		isRequest() const = 0;
};
#endif
