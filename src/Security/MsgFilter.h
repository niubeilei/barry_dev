////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgFilter.h
// Description:
//	As one of the security measurements, this class defines an interface
//  to filter messages. In many important points during a message 
//  transportation, SPNR uses message filters to ensure network security.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_MsgFilter_h
#define Omn_Security_MsgFilter_h

#include "Message/Ptrs.h"
#include "Util/RCObject.h"


class OmnMsgFilter : public OmnRCObject
{
public:
	virtual bool		filter(const OmnMsgPtr &msg) = 0;
};
#endif
