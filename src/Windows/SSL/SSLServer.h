////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLServer.h
// Description:
//		SSL server class   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#pragma once

#include "sslbase.h"

class CSSLServer : public SSLBase
{
public:
	CSSLServer(void);
	~CSSLServer(void);
	SECURITY_STATUS CreateCredentials(LPSTR pszUserName);
private:
	BOOL m_bMachineStore;
};
