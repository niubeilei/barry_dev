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
// 10/27/2010: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEServerCGI/FcgiConn.h"

AosFcgiConn::AosFcgiConn(
		const OmnString &name,
		const OmnString &uPath,
		const AosTcpLengthType lt):
OmnTcpClient(name, uPath, lt)
{
	
}


AosFcgiConn::~AosFcgiConn()
{
}

