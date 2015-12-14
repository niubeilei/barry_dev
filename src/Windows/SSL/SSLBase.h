////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLBase.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once

#include <schannel.h>
#define SECURITY_WIN32
#include <security.h>

#define DLL_NAME TEXT("Secur32.dll")
#define NT4_DLL_NAME TEXT("Security.dll")
#define IO_BUFFER_SIZE  0x10000

class AFX_CLASS_EXPORT SSLBase
{
public:
	SSLBase(void);
	~SSLBase(void);
	
	HMODULE m_hSecurity;
	PSecurityFunctionTable m_pSSPI;
	HCERTSTORE m_hMyCertStore;
	SCHANNEL_CRED m_schannelCred;

	BOOL LoadSecurityLibrary(void);
	void UnloadSecurityLibrary(void);
	SOCKET m_socket;
	CredHandle m_hCreds;
	CtxtHandle m_hCtxt;
	PCCERT_CONTEXT m_pRemoteCertContext;
protected:
	DWORD m_dwSSLProtocol;
};
