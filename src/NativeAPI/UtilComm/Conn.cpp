////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Conn.cpp
// Description:
//	A connection can be a TCP or UDP connection. It is used by OmnComm
//  to actually send and receive messages. 
//  This class defines the interface.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/Conn.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/Socket.h"
#include "Porting/Select.h"

bool
OmnConn::isConnGood(const int sock)
{
    //
    // This function determines whether the file descriptor is
    // good or not. The way to tell is to use "select(...)".
    // For more information on select(...), please refer to 
    // its mannual page.
    //

	if (sock <= 0)
	{
		return false;
	}

    fd_set fds;
    FD_ZERO(&fds);
	FD_SET(sock, &fds);

	//OmnTrace << "Socket to check: " << sock << endl;
    //
    // Set the time limit to 10 microsecond.
    //
    timeval theTime;
    theTime.tv_sec = 0;
    theTime.tv_usec = 1;
    
    //int retCode = OmnSocketSelect(sock+1, &fds, &fds, &fds, &theTime);    
    int retCode = OmnSocketSelect(sock+1, &fds, 0, 0, &theTime);    
	if (retCode >= 0)
	{
		//
		// Connection is good
		//
		return true;
	}

	OmnWarn << "Connection broken" << enderr;
	return false;
}
