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
// 01/03/2010: Copy by Brian 
////////////////////////////////////////////////////////////////////////////// 
#ifndef Aos_ShortMsgUtil_GsmListener_h 
#define Aos_ShortMsgUtil_GsmListener_h


#include "ShortMsgUtil/ShortMsg.h" 
#include "Util/String.h" 

class AosGsmListener : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool    shortMsgReceived(const AosShortMsg &message, const AosRundataPtr &rdata) = 0;

};
#endif
