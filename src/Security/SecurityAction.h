////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecurityAction.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Security_SecurityAction_h
#define Omn_Security_SecurityAction_h


class OmnSecurityAction
{
public:
	enum E
	{
		eBlock,				// It is considered a potential attack. Message
							// should be filtered out. No processing.
		eNormal,			// This is a normal message. Should process it.
		eRetransmitted		// This is a retransmitted message
	};

};
#endif
