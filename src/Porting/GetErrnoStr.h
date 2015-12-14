////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GetErrnoStr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_GetErrnoStr_h
#define Omn_Porting_GetErrnoStr_h

#include "Porting/GetErrno.h"
#include "Util/String.h"


#ifdef OMN_PLATFORM_UNIX
#define OMN_E_RESOURCE_N_A EWOULDBLOCK
inline OmnString OmnGetStrError(const OmnErrType::E errtype)
{
	return strerror(errno);
}


#elif OMN_PLATFORM_MICROSOFT
#define OMN_E_RESOURCE_N_A EDEADLOCK
#include <winsock2.h>
#include <windows.h>


OmnString OmnGetSelectErrString(const int eno);
OmnString OmnGetSocketErrString(const int eno);
OmnString OmnGetSetSocketOptErrString(const int eno);
OmnString OmnGetBindErrString(const int eno);
OmnString OmnGetSendtoErrString(const int eno);
OmnString OmnGetRecvfromErrString(const int eno);
OmnString OmnGetConnectErrString(const int eno);
OmnString OmnCreateDirectoryErrString(const int eno);
OmnString OmnGetAcceptErrString(const int eno);
OmnString OmnGetReadErrString(const int eno);

inline OmnString OmnGetStrError(const OmnErrType::E errtype)
{
	int eno = WSAGetLastError();
	switch (errtype)
	{
	case OmnErrType::eUnknown:
		 return "Unknown error type";

	case OmnErrType::eSelect:
		 return OmnGetSelectErrString(eno);

	case OmnErrType::eSocket:
		 return OmnGetSocketErrString(eno);

	case OmnErrType::eSetSocketOpt:
		 return OmnGetSetSocketOptErrString(eno);

	case OmnErrType::eBind:
		 return OmnGetBindErrString(eno);

	case OmnErrType::eSendto:
		 return OmnGetSendtoErrString(eno);

	case OmnErrType::eRecvfrom:
		 return OmnGetRecvfromErrString(eno);

	case OmnErrType::eRead:
		 return OmnGetReadErrString(eno);

	case OmnErrType::eConnect:
		 return OmnGetConnectErrString(eno);

	case OmnErrType::eCreateDirectory:
		 return OmnCreateDirectoryErrString(eno);

	case OmnErrType::eAccept:
		 return OmnGetAcceptErrString(eno);

	case OmnErrType::eSystem:
		 // 
		 // The error message code is retrieved by the system call. 
		 // 
		{
			DWORD dw = GetLastError();
			LPVOID lpMsgBuf;
			FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
			return (char*)lpMsgBuf;
		}


	default:
		 return OmnString("Unrecognized error type: ") << errtype;
	}

	return "Should never come to this point";
}



#endif
#endif

