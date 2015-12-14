////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommProt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilComm_CommProt_h
#define Omn_UtilComm_CommProt_h

#include "Util/String.h"


class OmnCommProt
{
public:
	enum E
	{
		eInvalid,
		eUnknown,
		eUDP,
		eTcpClient,
		eTcpServer
	};

	static E strToEnum(const OmnString &str);
};

#endif
