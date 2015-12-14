////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLClient.cpp
// Description:
//		SSL client class    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include ".\SSLClient.h"
#include "..\Common\common.h"

CSSLClient::CSSLClient(void)
: m_fUseProxy(FALSE)
{
	m_aiKeyExch = 0;
	m_sProxyPort = 0;
	m_pszProxyServer = NULL;
	memset(m_sServerName, 0, sizeof(m_sServerName));
	memset(&m_Sizes, 0, sizeof(m_Sizes));
	m_pRcvBuf = NULL;
	m_pRcvdEnd = NULL;
	m_pRead = NULL;
}

CSSLClient::~CSSLClient(void)
{
}

SECURITY_STATUS CSSLClient::CreateCredentials(LPSTR pszUserName)
{
	TimeStamp       tsExpiry;
	SECURITY_STATUS Status;

    DWORD           cSupportedAlgs = 0;
    ALG_ID          rgbSupportedAlgs[16];

    PCCERT_CONTEXT  pCertContext = NULL;

    // Open the "MY" certificate store, which is where Internet Explorer
    // stores its client certificates.
    if(m_hMyCertStore == NULL)
    {
        m_hMyCertStore = CertOpenSystemStore(0, "MY");

        if(!m_hMyCertStore)
        {
            printf("**** Error 0x%x returned by CertOpenSystemStore\n", GetLastError());
            return SEC_E_NO_CREDENTIALS;
        }
    }

    //
    // If a user name is specified, then attempt to find a client
    // certificate. Otherwise, just create a NULL credential.
    //

    if(pszUserName)
    {
        // Find client certificate. Note that this sample just searchs for a 
        // certificate that contains the user name somewhere in the subject name.
        // A real application should be a bit less casual.
        pCertContext = CertFindCertificateInStore(m_hMyCertStore, 
                                                  X509_ASN_ENCODING, 
                                                  0,
                                                  CERT_FIND_SUBJECT_STR_A,
                                                  pszUserName,
                                                  NULL);
        if(pCertContext == NULL)
        {
            printf("**** Error 0x%x returned by CertFindCertificateInStore\n",
                GetLastError());
			CertCloseStore(m_hMyCertStore, 0);
			m_hMyCertStore = NULL;
            return SEC_E_NO_CREDENTIALS;
        }
    }


    //
    // Build Schannel credential structure. Currently, this sample only
    // specifies the protocol to be used (and optionally the certificate, 
    // of course). Real applications may wish to specify other parameters 
    // as well.
    //

    ZeroMemory(&m_schannelCred, sizeof(m_schannelCred));

    m_schannelCred.dwVersion  = SCHANNEL_CRED_VERSION;
    if(pCertContext)
    {
        m_schannelCred.cCreds     = 1;
        m_schannelCred.paCred     = &pCertContext;
    }

    m_schannelCred.grbitEnabledProtocols = m_dwSSLProtocol;

    if(m_aiKeyExch)
    {
        rgbSupportedAlgs[cSupportedAlgs++] = m_aiKeyExch;
    }

    if(cSupportedAlgs)
    {
        m_schannelCred.cSupportedAlgs    = cSupportedAlgs;
        m_schannelCred.palgSupportedAlgs = rgbSupportedAlgs;
    }

    m_schannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;

    // The SCH_CRED_MANUAL_CRED_VALIDATION flag is specified because
    // this sample verifies the server certificate manually. 
    // Applications that expect to run on WinNT, Win9x, or WinME 
    // should specify this flag and also manually verify the server
    // certificate. Applications running on newer versions of Windows can
    // leave off this flag, in which case the InitializeSecurityContext
    // function will validate the server certificate automatically.
    m_schannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;

    //
    // Create an SSPI credential.
    //

    Status = m_pSSPI->AcquireCredentialsHandleA(
                        NULL,                   // Name of principal    
                        UNISP_NAME_A,           // Name of package
                        SECPKG_CRED_OUTBOUND,   // Flags indicating use
                        NULL,                   // Pointer to logon ID
                        &m_schannelCred,        // Package specific data
                        NULL,                   // Pointer to GetKey() func
                        NULL,                   // Value to pass to GetKey()
                        &m_hCreds,              // (out) Cred Handle
                        &tsExpiry);             // (out) Lifetime (optional)
    if(Status != SEC_E_OK)
    {
        printf("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
        goto cleanup;
    }

cleanup:

    //
    // Free the certificate context. Schannel has already made its own copy.
    //

    if(pCertContext)
    {
        CertFreeCertificateContext(pCertContext);
    }


    return Status;
}

int CSSLClient::ConnectToServer(LPSTR pszServerName, USHORT sPortNumber)
{
    struct sockaddr_in sin;
    struct hostent *hp;

	if(pszServerName)
	{
		strncpy(m_sServerName, pszServerName, sizeof(m_sServerName));
	}

    m_socket = socket(PF_INET, SOCK_STREAM, 0);
    if(m_socket == INVALID_SOCKET)
    {
        printf("**** Error %d creating socket\n", WSAGetLastError());
        return WSAGetLastError();
    }

    if(m_fUseProxy)
    {
        sin.sin_family = AF_INET;
        sin.sin_port = ntohs(m_sProxyPort);

        if((hp = gethostbyname(m_pszProxyServer)) == NULL)
        {
            printf("**** Error %d returned by gethostbyname\n", WSAGetLastError());
            return WSAGetLastError();
        }
        else
        {
            memcpy(&sin.sin_addr, hp->h_addr, 4);
        }
    }
    else
    {
        sin.sin_family = AF_INET;
        sin.sin_port = htons(sPortNumber);

        if((hp = gethostbyname(pszServerName)) == NULL)
        {
            printf("**** Error %d returned by gethostbyname\n", WSAGetLastError());
            return WSAGetLastError();
        }
        else
        {
            memcpy(&sin.sin_addr, hp->h_addr, 4);
        }
    }

    if(connect(m_socket, (struct sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("**** Error %d connecting to \"%s\" (%s)\n", 
            WSAGetLastError(),
            pszServerName, 
            inet_ntoa(sin.sin_addr));
        closesocket(m_socket);
        return WSAGetLastError();
    }

    if(m_fUseProxy)
    {
        CHAR  pbMessage[200]; 
        DWORD cbMessage;

        // Build message for proxy server
        strcpy(pbMessage, "CONNECT ");
        strcat(pbMessage, pszServerName);
        strcat(pbMessage, ":");
        _itoa(sPortNumber, pbMessage + strlen(pbMessage), 10);
        strcat(pbMessage, " HTTP/1.0\r\nUser-Agent: webclient\r\n\r\n");
        cbMessage = (DWORD)strlen(pbMessage);

        // Send message to proxy server
        if(send(m_socket, pbMessage, cbMessage, 0) == SOCKET_ERROR)
        {
            printf("**** Error %d sending message to proxy!\n", WSAGetLastError());
            return WSAGetLastError();
        }

        // Receive message from proxy server
        cbMessage = recv(m_socket, pbMessage, 200, 0);
        if(cbMessage == SOCKET_ERROR)
        {
            printf("**** Error %d receiving message from proxy\n", WSAGetLastError());
            return WSAGetLastError();
        }

        // this sample is limited but in normal use it 
        // should continue to receive until CR LF CR LF is received
    }

    return SEC_E_OK;
}

SECURITY_STATUS CSSLClient::PerformHandshake(
	SOCKET          Socket,         // in
    PCredHandle     phCreds,        // in
    LPSTR           pszServerName,  // in
    CtxtHandle *    phContext,      // out
    SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    //
    //  Initiate a ClientHello message and generate a token.
    //

    OutBuffers[0].pvBuffer   = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer   = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    scRet = m_pSSPI->InitializeSecurityContextA(
                    phCreds,
                    NULL,
                    pszServerName,
                    dwSSPIFlags,
                    0,
                    SECURITY_NATIVE_DREP,
                    NULL,
                    0,
                    phContext,
                    &OutBuffer,
                    &dwSSPIOutFlags,
                    &tsExpiry);

    if(scRet != SEC_I_CONTINUE_NEEDED)
    {
        printf("**** Error %d returned by InitializeSecurityContext (1)\n", scRet);
        return scRet;
    }

    // Send response to server if there is one.
    if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
    {
        cbData = send(Socket,
                      (const char *)OutBuffers[0].pvBuffer,
                      OutBuffers[0].cbBuffer,
                      0);
        if(cbData == SOCKET_ERROR || cbData == 0)
        {
            printf("**** Error %d sending data to server (1)\n", WSAGetLastError());
            m_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
            m_pSSPI->DeleteSecurityContext(phContext);
            return SEC_E_INTERNAL_ERROR;
        }

        printf("%d bytes of handshake data sent\n", cbData);

        // Free output buffer.
        m_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
        OutBuffers[0].pvBuffer = NULL;
    }


    return HandshakeLoop(Socket, phCreds, phContext, TRUE, pExtraData);
}

SECURITY_STATUS CSSLClient::HandshakeLoop( 
	SOCKET          Socket,         // in
    PCredHandle     phCreds,        // in
    CtxtHandle *    phContext,      // in, out
    BOOL            fDoInitialRead, // in
    SecBuffer *     pExtraData)     // out
{
    SecBufferDesc   InBuffer;
    SecBuffer       InBuffers[2];
    SecBufferDesc   OutBuffer;
    SecBuffer       OutBuffers[1];
    DWORD           dwSSPIFlags;
    DWORD           dwSSPIOutFlags;
    TimeStamp       tsExpiry;
    SECURITY_STATUS scRet;
    DWORD           cbData;

    PUCHAR          IoBuffer;
    DWORD           cbIoBuffer;
    BOOL            fDoRead;


    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT   |
                  ISC_REQ_REPLAY_DETECT     |
                  ISC_REQ_CONFIDENTIALITY   |
                  ISC_RET_EXTENDED_ERROR    |
                  ISC_REQ_ALLOCATE_MEMORY   |
                  ISC_REQ_STREAM;

    //
    // Allocate data buffer.
    //

    IoBuffer = (PUCHAR)LocalAlloc(LMEM_FIXED, IO_BUFFER_SIZE);
    if(IoBuffer == NULL)
    {
        printf("**** Out of memory (1)\n");
        return SEC_E_INTERNAL_ERROR;
    }
    cbIoBuffer = 0;

    fDoRead = fDoInitialRead;


    // 
    // Loop until the handshake is finished or an error occurs.
    //

    scRet = SEC_I_CONTINUE_NEEDED;

    while(scRet == SEC_I_CONTINUE_NEEDED        ||
          scRet == SEC_E_INCOMPLETE_MESSAGE     ||
          scRet == SEC_I_INCOMPLETE_CREDENTIALS) 
   {

        //
        // Read data from server.
        //

        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            if(fDoRead)
            {
                cbData = recv(Socket, 
                              (CHAR*)(IoBuffer + cbIoBuffer),
                              IO_BUFFER_SIZE - cbIoBuffer, 
                              0);
                if(cbData == SOCKET_ERROR)
                {
                    printf("**** Error %d reading data from server\n", WSAGetLastError());
                    scRet = SEC_E_INTERNAL_ERROR;
                    break;
                }
                else if(cbData == 0)
                {
                    printf("**** Server unexpectedly disconnected\n");
                    scRet = SEC_E_INTERNAL_ERROR;
                    break;
                }

                printf("%d bytes of handshake data received\n", cbData);

                cbIoBuffer += cbData;
            }
            else
            {
                fDoRead = TRUE;
            }
        }


        //
        // Set up the input buffers. Buffer 0 is used to pass in data
        // received from the server. Schannel will consume some or all
        // of this. Leftover data (if any) will be placed in buffer 1 and
        // given a buffer type of SECBUFFER_EXTRA.
        //

        InBuffers[0].pvBuffer   = IoBuffer;
        InBuffers[0].cbBuffer   = cbIoBuffer;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;

        InBuffers[1].pvBuffer   = NULL;
        InBuffers[1].cbBuffer   = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        InBuffer.cBuffers       = 2;
        InBuffer.pBuffers       = InBuffers;
        InBuffer.ulVersion      = SECBUFFER_VERSION;

        //
        // Set up the output buffers. These are initialized to NULL
        // so as to make it less likely we'll attempt to free random
        // garbage later.
        //

        OutBuffers[0].pvBuffer  = NULL;
        OutBuffers[0].BufferType= SECBUFFER_TOKEN;
        OutBuffers[0].cbBuffer  = 0;

        OutBuffer.cBuffers      = 1;
        OutBuffer.pBuffers      = OutBuffers;
        OutBuffer.ulVersion     = SECBUFFER_VERSION;

        //
        // Call InitializeSecurityContext.
        //

        scRet = m_pSSPI->InitializeSecurityContextA(
										  phCreds,
                                          phContext,
                                          NULL,
                                          dwSSPIFlags,
                                          0,
                                          SECURITY_NATIVE_DREP,
                                          &InBuffer,
                                          0,
                                          NULL,
                                          &OutBuffer,
                                          &dwSSPIOutFlags,
                                          &tsExpiry);

        //
        // If InitializeSecurityContext was successful (or if the error was 
        // one of the special extended ones), send the contends of the output
        // buffer to the server.
        //

        if(scRet == SEC_E_OK                ||
           scRet == SEC_I_CONTINUE_NEEDED   ||
           FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
        {
            if(OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
            {
                cbData = send(Socket,
                              (const CHAR*)OutBuffers[0].pvBuffer,
                              OutBuffers[0].cbBuffer,
                              0);
                if(cbData == SOCKET_ERROR || cbData == 0)
                {
                    printf("**** Error %d sending data to server (2)\n", 
                        WSAGetLastError());
                    m_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
                    m_pSSPI->DeleteSecurityContext(phContext);
                    return SEC_E_INTERNAL_ERROR;
                }

                printf("%d bytes of handshake data sent\n", cbData);

                // Free output buffer.
                m_pSSPI->FreeContextBuffer(OutBuffers[0].pvBuffer);
                OutBuffers[0].pvBuffer = NULL;
            }
        }


        //
        // If InitializeSecurityContext returned SEC_E_INCOMPLETE_MESSAGE,
        // then we need to read more data from the server and try again.
        //

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            continue;
        }


        //
        // If InitializeSecurityContext returned SEC_E_OK, then the 
        // handshake completed successfully.
        //

        if(scRet == SEC_E_OK)
        {
            //
            // If the "extra" buffer contains data, this is encrypted application
            // protocol layer stuff. It needs to be saved. The application layer
            // will later decrypt it with DecryptMessage.
            //

            printf("Handshake was successful\n");

            if(InBuffers[1].BufferType == SECBUFFER_EXTRA)
            {
                pExtraData->pvBuffer = LocalAlloc(LMEM_FIXED, 
                                                  InBuffers[1].cbBuffer);
                if(pExtraData->pvBuffer == NULL)
                {
                    printf("**** Out of memory (2)\n");
                    return SEC_E_INTERNAL_ERROR;
                }

                MoveMemory(pExtraData->pvBuffer,
                           IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                           InBuffers[1].cbBuffer);

                pExtraData->cbBuffer   = InBuffers[1].cbBuffer;
                pExtraData->BufferType = SECBUFFER_TOKEN;

                printf("%d bytes of app data was bundled with handshake data\n",
                    pExtraData->cbBuffer);
            }
            else
            {
                pExtraData->pvBuffer   = NULL;
                pExtraData->cbBuffer   = 0;
                pExtraData->BufferType = SECBUFFER_EMPTY;
            }

            //
            // Bail out to quit
            //

            break;
        }


        //
        // Check for fatal error.
        //

        if(FAILED(scRet))
        {
            printf("**** Error 0x%x returned by InitializeSecurityContext (2)\n", scRet);
            break;
        }


        //
        // If InitializeSecurityContext returned SEC_I_INCOMPLETE_CREDENTIALS,
        // then the server just requested client authentication. 
        //

        if(scRet == SEC_I_INCOMPLETE_CREDENTIALS)
        {
            //
            // Busted. The server has requested client authentication and
            // the credential we supplied didn't contain a client certificate.
            //

            // 
            // This function will read the list of trusted certificate
            // authorities ("issuers") that was received from the server
            // and attempt to find a suitable client certificate that
            // was issued by one of these. If this function is successful, 
            // then we will connect using the new certificate. Otherwise,
            // we will attempt to connect anonymously (using our current
            // credentials).
            //
            
            GetNewClientCredentials(phCreds, phContext);

            // Go around again.
            fDoRead = FALSE;
            scRet = SEC_I_CONTINUE_NEEDED;
            continue;
        }


        //
        // Copy any leftover data from the "extra" buffer, and go around
        // again.
        //

        if ( InBuffers[1].BufferType == SECBUFFER_EXTRA )
        {
            MoveMemory(IoBuffer,
                       IoBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
                       InBuffers[1].cbBuffer);

            cbIoBuffer = InBuffers[1].cbBuffer;
        }
        else
        {
            cbIoBuffer = 0;
        }
    }

    // Delete the security context in the case of a fatal error.
    if(FAILED(scRet))
    {
        m_pSSPI->DeleteSecurityContext(phContext);
    }

    LocalFree(IoBuffer);

    return scRet;
}

void CSSLClient::GetNewClientCredentials(CredHandle* phCreds, CtxtHandle* phContext)
{
	CredHandle hCreds;
    SecPkgContext_IssuerListInfoEx IssuerListInfo;
    PCCERT_CHAIN_CONTEXT pChainContext;
    CERT_CHAIN_FIND_BY_ISSUER_PARA FindByIssuerPara;
    PCCERT_CONTEXT  pCertContext;
    TimeStamp       tsExpiry;
    SECURITY_STATUS Status;

    //
    // Read list of trusted issuers from schannel.
    //

    Status = m_pSSPI->QueryContextAttributes(phContext,
                                    SECPKG_ATTR_ISSUER_LIST_EX,
                                    (PVOID)&IssuerListInfo);
    if(Status != SEC_E_OK)
    {
        printf("Error 0x%x querying issuer list info\n", Status);
        return;
    }

    //
    // Enumerate the client certificates.
    //

    ZeroMemory(&FindByIssuerPara, sizeof(FindByIssuerPara));

    FindByIssuerPara.cbSize = sizeof(FindByIssuerPara);
    FindByIssuerPara.pszUsageIdentifier = szOID_PKIX_KP_CLIENT_AUTH;
    FindByIssuerPara.dwKeySpec = 0;
    FindByIssuerPara.cIssuer   = IssuerListInfo.cIssuers;
    FindByIssuerPara.rgIssuer  = IssuerListInfo.aIssuers;

    pChainContext = NULL;

    while(TRUE)
    {
        // Find a certificate chain.
        pChainContext = CertFindChainInStore(m_hMyCertStore,
                                             X509_ASN_ENCODING,
                                             0,
                                             CERT_CHAIN_FIND_BY_ISSUER,
                                             &FindByIssuerPara,
                                             pChainContext);
        if(pChainContext == NULL)
        {
            printf("Error 0x%x finding cert chain\n", GetLastError());
            break;
        }
        printf("\ncertificate chain found\n");

        // Get pointer to leaf certificate context.
        pCertContext = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;

        // Create schannel credential.
        m_schannelCred.dwVersion = SCHANNEL_CRED_VERSION;
        m_schannelCred.cCreds = 1;
        m_schannelCred.paCred = &pCertContext;

        Status = m_pSSPI->AcquireCredentialsHandleA(
                            NULL,                   // Name of principal
                            UNISP_NAME_A,           // Name of package
                            SECPKG_CRED_OUTBOUND,   // Flags indicating use
                            NULL,                   // Pointer to logon ID
                            &m_schannelCred,          // Package specific data
                            NULL,                   // Pointer to GetKey() func
                            NULL,                   // Value to pass to GetKey()
                            &hCreds,                // (out) Cred Handle
                            &tsExpiry);             // (out) Lifetime (optional)
        if(Status != SEC_E_OK)
        {
            printf("**** Error 0x%x returned by AcquireCredentialsHandle\n", Status);
            continue;
        }
        printf("\nnew schannel credential created\n");

        // Destroy the old credentials.
        m_pSSPI->FreeCredentialsHandle(phCreds);

        *phCreds = hCreds;

        //
        // As you can see, this sample code maintains a single credential
        // handle, replacing it as necessary. This is a little unusual.
        //
        // Many applications maintain a global credential handle that's
        // anonymous (that is, it doesn't contain a client certificate),
        // which is used to connect to all servers. If a particular server
        // should require client authentication, then a new credential 
        // is created for use when connecting to that server. The global
        // anonymous credential is retained for future connections to
        // other servers.
        //
        // Maintaining a single anonymous credential that's used whenever
        // possible is most efficient, since creating new credentials all
        // the time is rather expensive.
        //

        break;
    }
}

BOOL CSSLClient::AuthServerCred(CtxtHandle hContext,PSTR pszServerName)
{
	//
    // Authenticate server's credentials.
    //
	SECURITY_STATUS Status;
    // Get server's certificate.
    Status = m_pSSPI->QueryContextAttributes(&hContext,
                                             SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                             (PVOID)&m_pRemoteCertContext);
    if(Status != SEC_E_OK)
    {
        printf("Error 0x%x querying remote certificate\n", Status);
        return FALSE;
    }

    // Display server certificate chain.
    DisplayCertChain(m_pRemoteCertContext, FALSE);

    // Attempt to validate server certificate.
    Status = VerifyServerCertificate(m_pRemoteCertContext,
                                     pszServerName,
                                     0);
    if(Status)
    {
        // The server certificate did not validate correctly. At this
        // point, we cannot tell if we are connecting to the correct 
        // server, or if we are connecting to a "man in the middle" 
        // attack server.

        // It is therefore best if we abort the connection.

        printf("**** Error 0x%x authenticating server credentials!\n", Status);
		// goto cleanup;
    }

    // Free the server certificate context.
    CertFreeCertificateContext(m_pRemoteCertContext);
    m_pRemoteCertContext = NULL;

 	return TRUE;
}

void CSSLClient::DisplayCertChain(PCCERT_CONTEXT pServerCert, bool fLocal)
{
	CHAR szName[1000];
    PCCERT_CONTEXT pCurrentCert;
    PCCERT_CONTEXT pIssuerCert;
    DWORD dwVerificationFlags;

    printf("\n");

    // display leaf name
    if(!CertNameToStr(pServerCert->dwCertEncodingType,
                      &pServerCert->pCertInfo->Subject,
                      CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                      szName, sizeof(szName)))
    {
        printf("**** Error 0x%x building subject name\n", GetLastError());
    }
    if(fLocal)
    {
        printf("Client subject: %s\n", szName);
    }
    else
    {
        printf("Server subject: %s\n", szName);
    }
    if(!CertNameToStr(pServerCert->dwCertEncodingType,
                      &pServerCert->pCertInfo->Issuer,
                      CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                      szName, sizeof(szName)))
    {
        printf("**** Error 0x%x building issuer name\n", GetLastError());
    }
    if(fLocal)
    {
        printf("Client issuer: %s\n", szName);
    }
    else
    {
        printf("Server issuer: %s\n\n", szName);
    }


    // display certificate chain
    pCurrentCert = pServerCert;
    while(pCurrentCert != NULL)
    {
        dwVerificationFlags = 0;
        pIssuerCert = CertGetIssuerCertificateFromStore(pServerCert->hCertStore,
                                                        pCurrentCert,
                                                        NULL,
                                                        &dwVerificationFlags);
        if(pIssuerCert == NULL)
        {
            if(pCurrentCert != pServerCert)
            {
                CertFreeCertificateContext(pCurrentCert);
            }
            break;
        }

        if(!CertNameToStr(pIssuerCert->dwCertEncodingType,
                          &pIssuerCert->pCertInfo->Subject,
                          CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                          szName, sizeof(szName)))
        {
            printf("**** Error 0x%x building subject name\n", GetLastError());
        }
        printf("CA subject: %s\n", szName);
        if(!CertNameToStr(pIssuerCert->dwCertEncodingType,
                          &pIssuerCert->pCertInfo->Issuer,
                          CERT_X500_NAME_STR | CERT_NAME_STR_NO_PLUS_FLAG,
                          szName, sizeof(szName)))
        {
            printf("**** Error 0x%x building issuer name\n", GetLastError());
        }
        printf("CA issuer: %s\n\n", szName);

        if(pCurrentCert != pServerCert)
        {
            CertFreeCertificateContext(pCurrentCert);
        }
        pCurrentCert = pIssuerCert;
        pIssuerCert = NULL;
    }
}

DWORD CSSLClient::VerifyServerCertificate(PCCERT_CONTEXT pServerCert, PSTR pszServerName, DWORD dwCertFlags)
{
    HTTPSPolicyCallbackData  polHttps;
    CERT_CHAIN_POLICY_PARA   PolicyPara;
    CERT_CHAIN_POLICY_STATUS PolicyStatus;
    CERT_CHAIN_PARA          ChainPara;
    PCCERT_CHAIN_CONTEXT     pChainContext = NULL;

    LPSTR rgszUsages[] = {  szOID_PKIX_KP_SERVER_AUTH,
                            szOID_SERVER_GATED_CRYPTO,
                            szOID_SGC_NETSCAPE };
    DWORD cUsages = sizeof(rgszUsages) / sizeof(LPSTR);

    PWSTR   pwszServerName = NULL;
    DWORD   cchServerName;
    DWORD   Status;

    if(pServerCert == NULL)
    {
        Status = SEC_E_WRONG_PRINCIPAL;
        goto cleanup;
    }


    //
    // Convert server name to unicode.
    //

    if(pszServerName == NULL || strlen(pszServerName) == 0)
    {
        Status = SEC_E_WRONG_PRINCIPAL;
        goto cleanup;
    }

    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, NULL, 0);
    pwszServerName = (PWSTR)LocalAlloc(LMEM_FIXED, cchServerName * sizeof(WCHAR));
    if(pwszServerName == NULL)
    {
        Status = SEC_E_INSUFFICIENT_MEMORY;
        goto cleanup;
    }
    cchServerName = MultiByteToWideChar(CP_ACP, 0, pszServerName, -1, pwszServerName, cchServerName);
    if(cchServerName == 0)
    {
        Status = SEC_E_WRONG_PRINCIPAL;
        goto cleanup;
    }


    //
    // Build certificate chain.
    //

    ZeroMemory(&ChainPara, sizeof(ChainPara));
    ChainPara.cbSize = sizeof(ChainPara);
    ChainPara.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
    ChainPara.RequestedUsage.Usage.cUsageIdentifier     = cUsages;
    ChainPara.RequestedUsage.Usage.rgpszUsageIdentifier = rgszUsages;

    if(!CertGetCertificateChain(
                            NULL,
                            pServerCert,
                            NULL,
                            pServerCert->hCertStore,
                            &ChainPara,
                            0,
                            NULL,
                            &pChainContext))
    {
        Status = GetLastError();
        printf("Error 0x%x returned by CertGetCertificateChain!\n", Status);
        goto cleanup;
    }


    //
    // Validate certificate chain.
    // 

    ZeroMemory(&polHttps, sizeof(HTTPSPolicyCallbackData));
    polHttps.cbStruct           = sizeof(HTTPSPolicyCallbackData);
    polHttps.dwAuthType         = AUTHTYPE_SERVER;
    polHttps.fdwChecks          = dwCertFlags;
    polHttps.pwszServerName     = pwszServerName;

    memset(&PolicyPara, 0, sizeof(PolicyPara));
    PolicyPara.cbSize            = sizeof(PolicyPara);
    PolicyPara.pvExtraPolicyPara = &polHttps;

    memset(&PolicyStatus, 0, sizeof(PolicyStatus));
    PolicyStatus.cbSize = sizeof(PolicyStatus);

    if(!CertVerifyCertificateChainPolicy(
                            CERT_CHAIN_POLICY_SSL,
                            pChainContext,
                            &PolicyPara,
                            &PolicyStatus))
    {
        Status = GetLastError();
        printf("Error 0x%x returned by CertVerifyCertificateChainPolicy!\n", Status);
        goto cleanup;
    }

    if(PolicyStatus.dwError)
    {
        Status = PolicyStatus.dwError;
        DisplayWinVerifyTrustError(Status); 
        goto cleanup;
    }


    Status = SEC_E_OK;

cleanup:

    if(pChainContext)
    {
        CertFreeCertificateChain(pChainContext);
    }

    if(pwszServerName)
    {
        LocalFree(pwszServerName);
    }

    return Status;
}

void CSSLClient::DisplayWinVerifyTrustError(DWORD Status)
{
    LPSTR pszName = NULL;

    switch(Status)
    {
    case CERT_E_EXPIRED:                pszName = "CERT_E_EXPIRED";                 break;
    case CERT_E_VALIDITYPERIODNESTING:  pszName = "CERT_E_VALIDITYPERIODNESTING";   break;
    case CERT_E_ROLE:                   pszName = "CERT_E_ROLE";                    break;
    case CERT_E_PATHLENCONST:           pszName = "CERT_E_PATHLENCONST";            break;
    case CERT_E_CRITICAL:               pszName = "CERT_E_CRITICAL";                break;
    case CERT_E_PURPOSE:                pszName = "CERT_E_PURPOSE";                 break;
    case CERT_E_ISSUERCHAINING:         pszName = "CERT_E_ISSUERCHAINING";          break;
    case CERT_E_MALFORMED:              pszName = "CERT_E_MALFORMED";               break;
    case CERT_E_UNTRUSTEDROOT:          pszName = "CERT_E_UNTRUSTEDROOT";           break;
    case CERT_E_CHAINING:               pszName = "CERT_E_CHAINING";                break;
    case TRUST_E_FAIL:                  pszName = "TRUST_E_FAIL";                   break;
    case CERT_E_REVOKED:                pszName = "CERT_E_REVOKED";                 break;
    case CERT_E_UNTRUSTEDTESTROOT:      pszName = "CERT_E_UNTRUSTEDTESTROOT";       break;
    case CERT_E_REVOCATION_FAILURE:     pszName = "CERT_E_REVOCATION_FAILURE";      break;
    case CERT_E_CN_NO_MATCH:            pszName = "CERT_E_CN_NO_MATCH";             break;
    case CERT_E_WRONG_USAGE:            pszName = "CERT_E_WRONG_USAGE";             break;
    default:                            pszName = "(unknown)";                      break;
    }

    printf("Error 0x%x (%s) returned by CertVerifyCertificateChainPolicy!\n", 
        Status, pszName);
}

int CSSLClient::EncryptSend(CHAR* cpBuffer, int iBufferLen)
{
	SECURITY_STATUS scRet;
	SecBufferDesc Message;
	SecBuffer Buffers[4];
	DWORD cbIoBufferLength;
	PBYTE pbIoBuffer;
	PBYTE pbMessage;
	DWORD cbMessage;
	DWORD cbData;

	//
    // Allocate a working buffer. The plaintext sent to EncryptMessage
    // should never be more than 'Sizes.cbMaximumMessage', so a buffer 
    // size of this plus the header and trailer sizes should be safe enough.
    // 

    cbIoBufferLength = m_Sizes.cbHeader + 
                       m_Sizes.cbMaximumMessage +
                       m_Sizes.cbTrailer;

    pbIoBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    if(pbIoBuffer == NULL)
    {
        printf("**** Out of memory (2)\n");
        return SEC_E_INTERNAL_ERROR;
    }
	
	// Build the input data offset into the data buffer by "header size"
    // bytes. This enables Schannel to perform the encryption in place,
    // which is a significant performance win.
    pbMessage = pbIoBuffer + m_Sizes.cbHeader;
	memcpy(pbMessage, (const VOID*)cpBuffer, iBufferLen);
	cbMessage = iBufferLen;

	//
    // Encrypt the input data.
    //

    Buffers[0].pvBuffer     = pbIoBuffer;
    Buffers[0].cbBuffer     = m_Sizes.cbHeader;
    Buffers[0].BufferType   = SECBUFFER_STREAM_HEADER;

    Buffers[1].pvBuffer     = pbMessage;
    Buffers[1].cbBuffer     = cbMessage;
    Buffers[1].BufferType   = SECBUFFER_DATA;

    Buffers[2].pvBuffer     = pbMessage + cbMessage;
    Buffers[2].cbBuffer     = m_Sizes.cbTrailer;
    Buffers[2].BufferType   = SECBUFFER_STREAM_TRAILER;

    Buffers[3].BufferType   = SECBUFFER_EMPTY;

    Message.ulVersion       = SECBUFFER_VERSION;
    Message.cBuffers        = 4;
    Message.pBuffers        = Buffers;

    scRet = m_pSSPI->EncryptMessage(&m_hCtxt, 0, &Message, 0);

	if(FAILED(scRet))
    {
        printf("**** Error 0x%x returned by EncryptMessage\n", scRet);
        return scRet;
    }


    // 
    // Send the encrypted data to the server.
    //

    cbData = send(m_socket,
                  (const CHAR*)pbIoBuffer,
                  Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer,
                  0);
    if(cbData == SOCKET_ERROR || cbData == 0)
    {
        printf("**** Error %d sending data to server (3)\n", 
            WSAGetLastError());
        m_pSSPI->DeleteSecurityContext(&m_hCtxt);
        return SEC_E_INTERNAL_ERROR;
    }

	return cbData;
}

int CSSLClient::RcvDecrypt1(CHAR* cpBuffer, int iBufferLen)
{
    SECURITY_STATUS scRet;
	SecBufferDesc   Message;
    SecBuffer       Buffers[4];
    SecBuffer *     pDataBuffer;
    SecBuffer *     pExtraBuffer;
    SecBuffer       ExtraBuffer;

    PBYTE pbIoBuffer;
    DWORD cbIoBuffer;
    DWORD cbIoBufferLength;

    DWORD cbData;
    INT   i;
	int iPlainRcved = 0;

	//
    // Allocate a working buffer. The plaintext sent to EncryptMessage
    // should never be more than 'Sizes.cbMaximumMessage', so a buffer 
    // size of this plus the header and trailer sizes should be safe enough.
    // 

    cbIoBufferLength = m_Sizes.cbHeader + 
                       m_Sizes.cbMaximumMessage +
                       m_Sizes.cbTrailer;

    pbIoBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbIoBufferLength);
    if(pbIoBuffer == NULL)
    {
        printf("**** Out of memory (2)\n");
        return -2;
    }
	//
    // Read data from server until done.
    //

    cbIoBuffer = 0;

    while(iPlainRcved < iBufferLen)
    {
        //
        // Read some data.
        //

        if(0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            cbData = recv(m_socket, 
                          (CHAR*)pbIoBuffer + cbIoBuffer, 
                          cbIoBufferLength - cbIoBuffer, 
                          0);
            if(cbData == SOCKET_ERROR)
            {
                printf("**** Error %d reading data from server\n", WSAGetLastError());
                scRet = SEC_E_INTERNAL_ERROR;
                break;
            }
            else if(cbData == 0)
            {
                // Server disconnected.
                if(cbIoBuffer)
                {
                    printf("**** Server unexpectedly disconnected\n");
                    scRet = SEC_E_INTERNAL_ERROR;
                    return 0;
                }
                else
                {
                    break;
                }
            }
            else
            {
                printf("%d bytes of (encrypted) application data received\n", cbData);

                cbIoBuffer += cbData;
            }
        }

        // 
        // Attempt to decrypt the received data.
        //

        Buffers[0].pvBuffer     = pbIoBuffer;
        Buffers[0].cbBuffer     = cbIoBuffer;
        Buffers[0].BufferType   = SECBUFFER_DATA;

        Buffers[1].BufferType   = SECBUFFER_EMPTY;
        Buffers[2].BufferType   = SECBUFFER_EMPTY;
        Buffers[3].BufferType   = SECBUFFER_EMPTY;

        Message.ulVersion       = SECBUFFER_VERSION;
        Message.cBuffers        = 4;
        Message.pBuffers        = Buffers;

        scRet = m_pSSPI->DecryptMessage(&m_hCtxt, &Message, 0, NULL);

        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            // The input buffer contains only a fragment of an
            // encrypted record. Loop around and read some more
            // data.
            continue;
        }

        // Server signalled end of session
        if(scRet == SEC_I_CONTEXT_EXPIRED)
            break;

        if( scRet != SEC_E_OK && 
            scRet != SEC_I_RENEGOTIATE && 
            scRet != SEC_I_CONTEXT_EXPIRED)
        {
            printf("**** Error 0x%x returned by DecryptMessage\n", scRet);
            return -3;
        }

        // Locate data and (optional) extra buffers.
        pDataBuffer  = NULL;
        pExtraBuffer = NULL;
        for(i = 1; i < 4; i++)
        {

            if(pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
            {
                pDataBuffer = &Buffers[i];
                printf("Buffers[%d].BufferType = SECBUFFER_DATA\n",i);
            }
            if(pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
            {
                pExtraBuffer = &Buffers[i];
            }
        }

        // Display or otherwise process the decrypted data.
        if(pDataBuffer)
        {
			if(int(pDataBuffer->cbBuffer+iPlainRcved) < iBufferLen)
			{
				memcpy(cpBuffer+iPlainRcved, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
			}
			iPlainRcved += pDataBuffer->cbBuffer;
            printf("Decrypted data: %d bytes\n", pDataBuffer->cbBuffer);
			
			char tmp[256];
			sprintf(tmp, "Decrypted data: %d bytes\n", iPlainRcved);
			AfxMessageBox(tmp);

        }

        // Move any "extra" data to the input buffer.
        if(pExtraBuffer)
        {
            MoveMemory(pbIoBuffer, pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
            cbIoBuffer = pExtraBuffer->cbBuffer;
        }
        else
        {
            cbIoBuffer = 0;
        }

        if(scRet == SEC_I_RENEGOTIATE)
        {
            // The server wants to perform another handshake
            // sequence.

            printf("Server requested renegotiate!\n");

            scRet = HandshakeLoop(m_socket, 
                                &m_hCreds, 
                                &m_hCtxt, 
                                FALSE, 
                                &ExtraBuffer);
			if(scRet != SEC_E_OK)
            {
                return -4;
            }

            // Move any "extra" data to the input buffer.
            if(ExtraBuffer.pvBuffer)
            {
                MoveMemory(pbIoBuffer, ExtraBuffer.pvBuffer, ExtraBuffer.cbBuffer);
                cbIoBuffer = ExtraBuffer.cbBuffer;
            }
        }
    }

    return iPlainRcved;
}

int CSSLClient::RcvDecrypt(CHAR* cpBuffer, int iBufferLen, int& iRemainPlainLen)
{
    SECURITY_STATUS scRet;
	SecBufferDesc   Message;
    SecBuffer       Buffers[4];
    SecBuffer *     pDataBuffer;
    SecBuffer *     pExtraBuffer;

    DWORD cbData;

	INT   i;
	INT iPlainRcvd = 0, iWritePos = 0, iDataNeeded = 0;


	if(!m_pRcvBuf)
	{
		m_pRcvBuf = (PCHAR)malloc(MAX_SSL_RECEIVE_BUFFER_SIZE);
		if(!m_pRcvBuf)
		{
			// add log here
			return eAosWinRc_Failed;
		}
		m_pRead = m_pRcvdEnd = m_pRcvBuf;
	}

	// plain data remained in m_pcRcvBuf, rcv from these remained data first
	if(m_pRead < m_pRcvdEnd)
	{
		iPlainRcvd = (int)(m_pRcvdEnd - m_pRead);
		if(iPlainRcvd > iBufferLen)
		{
			memcpy(cpBuffer, m_pRead, iBufferLen);
			m_pRead += iBufferLen;
			iRemainPlainLen = iPlainRcvd - iBufferLen;
			return iBufferLen;
		}
		else
		{
			memcpy(cpBuffer, m_pRead, iPlainRcvd);
			m_pRead += iPlainRcvd;
			iRemainPlainLen = 0;
			return iPlainRcvd;
		}
	}

    //
    // Read one ssl record once.
    //
    while(TRUE)
	{
		// read ssl record header first
		if(iWritePos < (int)m_Sizes.cbHeader)
		{
			iDataNeeded = m_Sizes.cbHeader - iWritePos;

			cbData = recv(m_socket, m_pRcvBuf + iWritePos, iDataNeeded, 0);

			if(cbData == SOCKET_ERROR)
			{
				if(WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				// add log here 
				return eAosWinRc_Failed-1;
			}
			else if(cbData == 0)
			{
				// Server disconnected.
				// add log here
				return 0;
			}
			else
			{
				iWritePos += cbData;
				if(iWritePos < (int)m_Sizes.cbHeader)	continue;

 				// parse the header
				USHORT usLen;
				memcpy(&usLen, m_pRcvBuf + iWritePos - 2, 2);
				iDataNeeded = ntohs(usLen);
			}
		}

    	// read ssl record body
     
        cbData = recv(m_socket,m_pRcvBuf + iWritePos, iDataNeeded, 0);

        if(cbData == SOCKET_ERROR)
        {
			DWORD dwError = WSAGetLastError();
			if(dwError == WSAEWOULDBLOCK)
			{
				// add log here ("**** Error %d reading data from server\n", WSAGetLastError()));
				continue;
			}
			return eAosWinRc_Failed-2;
        }
        else if(cbData == 0)
        {
            // Server disconnected.
            // add log here ("**** Server disconnected, encrypted data buffered %d\n", m_nWritePos));
            return 0;
        }

		iWritePos += cbData;
		iDataNeeded -= cbData;

		if(iDataNeeded)
		{
			// add log here"**** Need more encrypted data, read %d, waiting for %d\n", m_nWritePos, m_nDataNeeded));
			continue;
			//return eAosWinRc_Failed-3;
		}
        // 
        // Attempt to decrypt the received data.
        //

        Buffers[0].pvBuffer     = m_pRcvBuf;
        Buffers[0].cbBuffer     = iWritePos;
        Buffers[0].BufferType   = SECBUFFER_DATA;

        Buffers[1].BufferType   = SECBUFFER_EMPTY;
        Buffers[2].BufferType   = SECBUFFER_EMPTY;
        Buffers[3].BufferType   = SECBUFFER_EMPTY;

        Message.ulVersion       = SECBUFFER_VERSION;
        Message.cBuffers        = 4;
        Message.pBuffers        = Buffers;

        scRet = m_pSSPI->DecryptMessage(&m_hCtxt, &Message, 0, NULL);
		iWritePos = 0;
        if(scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            // we already have full ssl record, should not happen
			WSASetLastError(WSAENETDOWN);
            return eAosWinRc_Failed-4;
        }

        // Server signalled end of session
        if(scRet == SEC_I_CONTEXT_EXPIRED)
		{
			WSASetLastError(WSAETIMEDOUT);
            return eAosWinRc_Failed-5;
		}

        if( scRet != SEC_E_OK && 
            scRet != SEC_I_RENEGOTIATE && 
            scRet != SEC_I_CONTEXT_EXPIRED)
        {
            WSASetLastError(WSAENETDOWN);
            return eAosWinRc_Failed-6;
        }

        // Locate data and (optional) extra buffers.
        pDataBuffer  = NULL;
        pExtraBuffer = NULL;
        for(i = 1; i < 4; i++)
        {

            if(pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
            {
                pDataBuffer = &Buffers[i];
                printf("Buffers[%d].BufferType = SECBUFFER_DATA\n",i);
            }
            if(pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
            {
                pExtraBuffer = &Buffers[i];
            }
        }

		// should not happen.
		if(pExtraBuffer)
		{
			// add log here
			WSASetLastError(WSAENETDOWN);
			return eAosWinRc_Failed-7;
		}

		if(scRet == SEC_I_RENEGOTIATE)
		{
			// The server wants to perform another handshake
			// sequence.
			// add log here 
			WSASetLastError(WSAENETDOWN);
			return eAosWinRc_Failed-8;
		}
		// Display or otherwise process the decrypted data.
        if(pDataBuffer)
        {
			m_pRcvdEnd = (PCHAR)pDataBuffer->pvBuffer + pDataBuffer->cbBuffer;
			m_pRead = (PCHAR)pDataBuffer->pvBuffer;
			iPlainRcvd = pDataBuffer->cbBuffer;
			if(iPlainRcvd > iBufferLen)
			{
				memcpy(cpBuffer,  m_pRead, iBufferLen);
				m_pRead += iBufferLen;
				iRemainPlainLen = iPlainRcvd - iBufferLen;
				return iBufferLen;
			}
			else
			{
				memcpy(cpBuffer, m_pRead, iPlainRcvd);
				m_pRead += iPlainRcvd;
				iRemainPlainLen = 0;
				return iPlainRcvd;
			}
        }
		else
		{
			m_pRcvdEnd = NULL;
		}
    }

    return iPlainRcvd;
}

BOOL CSSLClient::Init(PSTR pszUserName)
{
	if(!LoadSecurityLibrary())
		return FALSE;
	if(SEC_E_OK != CreateCredentials(pszUserName))
	{
		UnloadSecurityLibrary();
		return FALSE;
	}
	return TRUE;
}

BOOL CSSLClient::Handshake(void)
{
	SecBuffer ExtraData;
	SECURITY_STATUS scRet;
	if(SEC_E_OK != PerformHandshake(m_socket, &m_hCreds, m_sServerName, &m_hCtxt,&ExtraData))
	{
		return FALSE;
	}
	
	//
    // Read stream encryption properties.
    //
	scRet = m_pSSPI->QueryContextAttributes(&m_hCtxt,SECPKG_ATTR_STREAM_SIZES,&m_Sizes);
    if(SEC_E_OK != scRet)
    {
        printf("**** Error 0x%x reading SECPKG_ATTR_STREAM_SIZES\n", scRet);
        return FALSE;
    }
	return TRUE;
}

VOID CSSLClient::Release(void)
{
	if(m_pRcvBuf) free(m_pRcvBuf);
	return;
}
