////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockSSL.cpp
// Description:
//	SSL sock is a kernel sock. Whenever it receives a packet, if it is
//  a signal packet, it is processed by the normal stack. If it is an 
//	application data packet, it uses the context to decrypt the payload.
//	It then passes the user data to either upper application or to
//	its sock bridge.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosSockSSL.h"


