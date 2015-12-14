////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_ReturnCode_h
#define Aos_ssl_ReturnCode_h

#include "aos/aosReturnCode.h"


enum
{
	eAosRc_NoMsg = eAosRc_SSLStart+1,
	eAosRc_CertificateMismatch,
	eAosRc_FinishedIncorrect,
	eAosRc_ForwardToSender,
	eAosRc_Pending,
	eAosRc_SSLAbort,
	eAosRc_SslCliErr,
	eAosRc_SslTestErr,
	eAosRc_CipherNotFound,
	eAosAlarm_CipherSuiteNotSupported,

	eAosAlarm_SSLFlowErr = eAosRc_SSLStart+1,
	eAosAlarm_SSLSynErr,
	eAosAlarm_SSLProcErr,
	eAosAlarm_VerifyCertFailed,
};

enum AosSSLErrCodes
{
	eAosSSLErr_NoCertificate = 0x0229, 	// No Certificate or Private Key
	eAosSSLErr_InternalErr = 0x0250, 

	eAosSSLErr_LevelWarning = 0x0100,
	eAosSSLErr_LevelFatal = 0x0200,

	eAosSSLErr_CloseNotify = 0,
	
//	eAosSSLErr_NoCertificate = 41,
	eAosSSLErr_BadCertificate = 42,
	eAosSSLErr_UnsupportedCertificate = 43,
	eAosSSLErr_CertificateRevoked = 44,
	eAosSSLErr_CertificateExpired = 45,
	eAosSSLErr_CertificateUnknown = 46,

	eAosSSLErr_UsbkeyError = 81,
	
};


#endif

