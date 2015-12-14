////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RetransRequester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Retrans_RetransRequester_h
#define Omn_Retrans_RetransRequester_h

#include "aosUtil/Types.h"
#include "Message/Ptrs.h"
#include "Porting/LongTypes.h"
#include "Util/RCObject.h"


class OmnRetransRequester : public virtual OmnRCObject
{
public:
	// 
	// If the retransmission failed, this function will be called.
	// 'numTries' is the number of sending tries.
	//
	virtual void		retransFailed(const int64_t &transId, 
									  const int numTries,
									  const OmnMsgPtr &msg, 
									  void *userData) = 0;

	// 
	// If the retransmission is needed, this function is called. 
	// The requester will implement how to resend the message. 
	// The message itself is included. 'numTries' is the number
	// of sending tries.
	//
	virtual void		resend(int64_t &transId, 
							   const int numTries, 
							   const OmnMsgPtr &msg,
							   void *userData) = 0;

	// 
	// Returns the name of the requester.
	//
	virtual OmnString	getRetransRequesterName() const = 0;
};

#endif

