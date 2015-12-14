////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: NetworkInit.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_NetworkInit_h
#define Omn_Porting_NetworkInit_h

#include "Debug/Debug.h"


#ifdef OMN_PLATFORM_MICROSOFT

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <io.h>



extern CRITICAL_SECTION sgCsOutput;

inline bool OmnNetworkInit(OmnString &errMsg)
{ 
	InitializeCriticalSection(&sgCsOutput);	
	WORD wVersionRequested;
	WSADATA wsaData;
 
	wVersionRequested = MAKEWORD( 1, 1 );
 
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		//
		// Failed to find a usable WinSock
		//
		errMsg = "<Porting/WSA.h:>Failed to locate a usable WinSock.dll!";
		WSACleanup();
	    return false;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		//
		// Failed to find a usable WinSock
		//
		errMsg = "<Porting/WSA.h:> Failed to locate a usable WinSock.dll!";
		WSACleanup();
		return false; 
	}

	return true;
}


inline bool OmnClearNetwork()
{
	if(WSACleanup() != 0)
	{
		cout << "<Porting:WSA.h> Failed to clean up: "
			<< WSAGetLastError() 
			<< endl;
		return false;
	}
	return true;

}
#elif OMN_PLATFORM_UNIX
inline bool OmnNetworkInit(OmnString &errMsg)
{
	return true;
}

inline bool OmnClearNetwork()
{
	return true;
}

#endif

#endif

