////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgFctryIntf.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Message_MsgFctryIntf_h
#define Omn_Message_MsgFctryIntf_h

#include "Message/MsgId.h"
#include "Message/Ptrs.h"
#include "Util/RCObject.h"


class OmnMsgFctryIntf : virtual public OmnRCObject
{
public:
	virtual bool	registerMsgCreator(const OmnMsgId::E msgId, 
									   const OmnMsgCreatorPtr &creator) = 0;
};
#endif
