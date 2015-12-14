////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransBasic_MsgType_h
#define AOS_TransBasic_MsgType_h

#include "SEInterfaces/TransType.h"

typedef AosTransType MsgType; 

/*
class MsgType
{
public:
	enum E
	{
		eInvalid,
				
		eConnAck,
		eGetWaitSeq,
		eWaitSeq,

		eTrans,
		eResp,
		eTransAck,
		eFinishTid,
		eCleanTid,
		eRmCltTransLog,

		eSvrIsUp,
		eMarkSvrDeath,
		eBkpSvrIsUp,
		
		eMax,
	};
	
	static bool isValid(const E type)
	{
		return type>eInvalid && type<eMax; 
	}
};
*/

#endif
