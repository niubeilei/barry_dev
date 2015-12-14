////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLServer.cpp
// Description:
//		SSL server class    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include ".\sslserver.h"

CSSLServer::CSSLServer(void)
: m_bMachineStore(FALSE)
{
}

CSSLServer::~CSSLServer(void)
{
}

SECURITY_STATUS CSSLServer::CreateCredentials(LPSTR pszUserName)
{
	TimeStamp       tsExpiry;
	PCCERT_CONTEXT  pCertContext = NULL;
	SECURITY_STATUS sStatus;

	if(pszUserName == NULL || strlen(pszUserName) == 0)
	{
		printf("**** No user name specified!\n");
		return SEC_E_NO_CREDENTIALS;
	}

	// Open the "MY" certificate store.
	if( m_hMyCertStore == NULL )
	{
		if( m_bMachineStore )
		{
			m_hMyCertStore = CertOpenStore(	
								CERT_STORE_PROV_SYSTEM,
								X509_ASN_ENCODING,
								0,
								CERT_SYSTEM_STORE_LOCAL_MACHINE,
								L"MY");
		}
		else
		{
			m_hMyCertStore = CertOpenSystemStore( 0, "MY" );
		}

		if( !m_hMyCertStore )
		{
			printf("**** Error 0x%x returned by CertOpenSystemStore\n", 
				GetLastError());
			return SEC_E_NO_CREDENTIALS;
		}
	}

	// Find certificate. Note that this sample just searchs for a 
	// certificate that contains the user name somewhere in the subject name.
	// A real application should be a bit less casual.
	pCertContext = CertFindCertificateInStore(
					m_hMyCertStore, 
					X509_ASN_ENCODING, 
					0,
					CERT_FIND_SUBJECT_STR_A,
					pszUserName,
					NULL);
	if( pCertContext == NULL )
	{
		printf("**** Error 0x%x returned by CertFindCertificateInStore\n",
			GetLastError());
		sStatus = SEC_E_NO_CREDENTIALS;
		goto CLEANUP;
	}


	//
	// Build Schannel credential structure. Currently, this sample only
	// specifies the protocol to be used (and optionally the certificate, 
	// of course). Real applications may wish to specify other parameters 
	// as well.
	//

	ZeroMemory( &m_schannelCred, sizeof(m_schannelCred) );

	m_schannelCred.dwVersion = SCHANNEL_CRED_VERSION;

	m_schannelCred.cCreds = 1;
	m_schannelCred.paCred = &pCertContext;

	m_schannelCred.grbitEnabledProtocols = m_dwSSLProtocol;

	//
	// Create an SSPI credential.
	//

	sStatus = m_pSSPI->AcquireCredentialsHandle(
		NULL,                   // Name of principal
		UNISP_NAME_A,           // Name of package
		SECPKG_CRED_INBOUND,    // Flags indicating use
		NULL,                   // Pointer to logon ID
		&m_schannelCred,        // Package specific data
		NULL,                   // Pointer to GetKey() func
		NULL,                   // Value to pass to GetKey()
		&m_hCreds,              // (out) Cred Handle
		&tsExpiry);             // (out) Lifetime (optional)
	if( sStatus != SEC_E_OK )
	{
		printf("**** Error 0x%x returned by AcquireCredentialsHandle\n", sStatus);
		goto CLEANUP;
	}


	//
	// Free the certificate context. Schannel has already made its own copy.
	//

	if(pCertContext)
	{
		CertFreeCertificateContext(pCertContext);
	}

	return SEC_E_OK;

CLEANUP:
	if ( m_hMyCertStore )
	{
		CertCloseStore( m_hMyCertStore, 0 );	
		m_hMyCertStore = NULL;
	}
	return sStatus;
}
