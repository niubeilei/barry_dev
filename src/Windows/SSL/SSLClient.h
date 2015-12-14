////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLClient.h
// Description:
//		SSL client class    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sslbase.h"

#define MAX_SSL_RECEIVE_BUFFER_SIZE 16500

class AFX_CLASS_EXPORT CSSLClient : public SSLBase
{
public:
	CSSLClient(void);
	~CSSLClient(void);

	//DWORD m_dwSSLProtocol;
	ALG_ID m_aiKeyExch;
	BOOL m_fUseProxy;
	USHORT m_sProxyPort;
	LPSTR m_pszProxyServer;
	CHAR m_sServerName[MAX_PATH];
	SecPkgContext_StreamSizes m_Sizes;
	PCHAR m_pRcvBuf,m_pRcvdEnd, m_pRead;

	SECURITY_STATUS CreateCredentials(LPSTR pszUserName);
	int ConnectToServer(LPSTR pszServerName, USHORT sPortNumber);
	SECURITY_STATUS PerformHandshake(
		SOCKET          Socket,         // in
		PCredHandle     phCreds,        // in
		LPSTR           pszServerName,  // in
		CtxtHandle *    phContext,      // out
		SecBuffer *     pExtraData);    // out
	SECURITY_STATUS HandshakeLoop(
		SOCKET          Socket,         // in
		PCredHandle     phCreds,        // in
		CtxtHandle *    phContext,      // in, out
		BOOL            fDoInitialRead, // in
		SecBuffer *     pExtraData);     // out
	void GetNewClientCredentials(CredHandle* phCreds, CtxtHandle* phContext);
	BOOL AuthServerCred(CtxtHandle hContext, PSTR pszServerName);
	void DisplayCertChain(PCCERT_CONTEXT pServerCert, bool fLocal);
	DWORD VerifyServerCertificate(PCCERT_CONTEXT pServerCert, PSTR pszServerName, DWORD dwCertFlags);
	void DisplayWinVerifyTrustError(DWORD Status);
	int EncryptSend(CHAR* cpBuffer, int iBufferLen);
	int RcvDecrypt1(CHAR* cpBuffer, int iBufferLen);
	int RcvDecrypt(CHAR* cpBuffer, int iBufferLen, int& iRemainPlainLen);
	BOOL Init(PSTR pszUserName);
	BOOL Handshake(void);
	VOID Release(void);
};
