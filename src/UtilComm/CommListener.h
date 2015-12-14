////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommListener.h
// Description:
// Description:
//	It defines an interface. OmnComm will use this interface to call
//  back when it reads a message.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UtilComm_CommListener_h
#define Omn_UtilComm_CommListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"



class OmnCommListener : public virtual OmnRCObject
{
public:
	virtual bool		msgRead(const OmnConnBuffPtr &buff) = 0;
	virtual OmnString	getCommListenerName() const = 0;
	virtual void 		readingFailed()  = 0;
};
#endif
