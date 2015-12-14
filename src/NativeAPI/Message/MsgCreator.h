////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgCreator.h
// Description:
//	It defines an interface that is used to create a message based on 
//  a OmnConnBuff.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgCreator_h
#define Omn_Message_MsgCreator_h

#include "Util/RCObject.h"


class OmnMsgCreator : virtual public OmnRCObject
{
public:
	virtual OmnMsgPtr	clone() const = 0;
};
#endif
