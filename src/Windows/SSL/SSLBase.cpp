////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLBase.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\sslbase.h"

SSLBase::SSLBase(void)
: m_socket(NULL)
, m_dwSSLProtocol(0)
{
	m_hSecurity = NULL;
	m_pSSPI = NULL;
	m_hMyCertStore = NULL;
	m_pRemoteCertContext = NULL;
}

SSLBase::~SSLBase(void)
{
}

BOOL SSLBase::LoadSecurityLibrary(void)
{
	INIT_SECURITY_INTERFACE         pInitSecurityInterface;
    OSVERSIONINFO VerInfo;
    CHAR lpszDLL[MAX_PATH];

	if (m_hSecurity && m_pSSPI) return TRUE;

    //
    //  Find out which security DLL to use, depending on
    //  whether we are on Win2K, NT or Win9x
    //

    VerInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (!GetVersionEx (&VerInfo))   
    {
        return FALSE;
    }

    if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT 
        && VerInfo.dwMajorVersion == 4)
    {
        strcpy (lpszDLL, NT4_DLL_NAME );
    }
    else if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
          VerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        strcpy (lpszDLL, DLL_NAME );
    }
    else
    {
        return FALSE;
    }

    //
    //  Load Security DLL
    //

    m_hSecurity = LoadLibrary(lpszDLL);
    if(m_hSecurity == NULL)
    {
        printf("Error 0x%x loading %s.\n", GetLastError(), lpszDLL);
        return FALSE;
    }

    pInitSecurityInterface = (INIT_SECURITY_INTERFACE)GetProcAddress(
                                    m_hSecurity,
                                    "InitSecurityInterfaceA");
    
    if(pInitSecurityInterface == NULL)
    {
        printf("Error 0x%x reading InitSecurityInterface entry point.\n", 
               GetLastError());
        return FALSE;
    }

    m_pSSPI = pInitSecurityInterface();

    if(m_pSSPI == NULL)
    {
        printf("Error 0x%x reading security interface.\n",
               GetLastError());
        return FALSE;
    }

    return TRUE;
}

void SSLBase::UnloadSecurityLibrary(void)
{
	FreeLibrary(m_hSecurity);
    m_hSecurity = NULL;
	m_pSSPI = NULL;
}