////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslServer.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include <linux/in.h>
#include <linux/net.h>
#include <net/sock.h>
#endif

#include "KernelSimu/string.h"
#include "aos/aosReturnCode.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Random.h"

#include "ssl/SslCommon.h"
#include "ssl/SslServer.h"
#include "ssl/SslMisc.h"
#include "ssl/SslRecord.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslSessionMgr.h"
#include "ssl/rsa.h"
#include "ssl/SslCipher.h"
#include "ssl/Ssl.h"
#include "ssl/cavium.h"
#include "ssl/SslWrapper.h"

#include "PKCS/x509.h"
#include "PKCS/CertChain.h"

#include "CertVerify/aosCertVerify.h"
#include "CertVerify/aosAccessManage.h"
#include "AppProxy/usbkey.h"
#include "Ktcpvs/aosProxyConn.h"

//#include "Proxy/wrapper_ssl.h"
#include "Porting/TimeOfDay.h"
#ifdef __KERNEL__
#include "CertVerify/amm_priv.h"
extern AMM_CONFIG * amc;
#include "ssl/cavium.h"
#endif

uint8 aosCert[]={
	0x30, // the start of the first cert
	0x82, 0x02, 0x89, 0x30, 0x82, 0x01, 0xf2, 0x02, 0x01, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
	0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04, 0x05, 0x00, 0x30, 0x81, 0x88, 0x31, 0x0b, 0x30, 0x09,
	0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 0x4e, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55,
	0x04, 0x08, 0x13, 0x07, 0x62, 0x65, 0x69, 0x6a, 0x69, 0x6e, 0x67, 0x31, 0x10, 0x30, 0x0e, 0x06,
	0x03, 0x55, 0x04, 0x07, 0x13, 0x07, 0x62, 0x65, 0x69, 0x6a, 0x69, 0x6e, 0x67, 0x31, 0x0f, 0x30,
	0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x06, 0x41, 0x4f, 0x53, 0x4c, 0x54, 0x44, 0x31, 0x0c,
	0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x03, 0x41, 0x4f, 0x53, 0x31, 0x0e, 0x30, 0x0c,
	0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x05, 0x41, 0x4f, 0x53, 0x43, 0x41, 0x31, 0x26, 0x30, 0x24,
	0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x17, 0x61, 0x6f, 0x73,
	0x63, 0x61, 0x40, 0x69, 0x70, 0x61, 0x63, 0x6b, 0x65, 0x74, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65,
	0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x35, 0x30, 0x39, 0x31, 0x35, 0x30, 0x39,
	0x30, 0x31, 0x31, 0x31, 0x5a, 0x17, 0x0d, 0x30, 0x36, 0x30, 0x39, 0x31, 0x35, 0x30, 0x39, 0x30,
	0x31, 0x31, 0x31, 0x5a, 0x30, 0x81, 0x90, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
	0x13, 0x02, 0x43, 0x4e, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x07, 0x62,
	0x65, 0x69, 0x6a, 0x69, 0x6e, 0x67, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13,
	0x07, 0x62, 0x65, 0x69, 0x6a, 0x69, 0x6e, 0x67, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
	0x0a, 0x13, 0x06, 0x41, 0x4f, 0x53, 0x4c, 0x54, 0x44, 0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55,
	0x04, 0x0b, 0x13, 0x03, 0x41, 0x4f, 0x53, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x03,
	0x13, 0x09, 0x41, 0x6f, 0x73, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x31, 0x2a, 0x30, 0x28, 0x06,
	0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x1b, 0x61, 0x6f, 0x73, 0x73,
	0x65, 0x72, 0x76, 0x65, 0x72, 0x40, 0x69, 0x70, 0x61, 0x63, 0x6b, 0x65, 0x74, 0x65, 0x6e, 0x67,
	0x69, 0x6e, 0x65, 0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
	0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d, 0x00, 0x30, 0x81, 0x89,
	0x02, 0x81, 0x81, 0x00, 0xc7, 0xa7, 0x41, 0x27, 0x66, 0x58, 0x1d, 0xf1, 0xf0, 0x73, 0x80, 0xad,
	0xcd, 0x73, 0x86, 0xab, 0x2e, 0x9d, 0x21, 0x49, 0x0e, 0x12, 0x81, 0x46, 0x2d, 0x99, 0x76, 0x5d,
	0x7d, 0xdb, 0x5c, 0xc5, 0x71, 0xd6, 0x82, 0xb1, 0xac, 0x16, 0x66, 0xd2, 0x89, 0x06, 0x7a, 0x65,
	0xb2, 0x0a, 0x5f, 0x44, 0x53, 0x8c, 0x12, 0xad, 0x30, 0xa0, 0x53, 0xa8, 0x84, 0x5c, 0x68, 0xc0,
	0x7c, 0x29, 0xd1, 0x74, 0xa1, 0x42, 0x13, 0x26, 0xc0, 0xc4, 0x5d, 0xfc, 0x0e, 0x1c, 0x83, 0x08,
	0xa5, 0x8d, 0x12, 0x03, 0x00, 0x9a, 0x40, 0x6e, 0x64, 0x23, 0xe6, 0xa6, 0x0a, 0xa0, 0xa8, 0xf7,
	0xd2, 0xa9, 0xb1, 0xd6, 0x47, 0x19, 0x08, 0xa0, 0xef, 0x9b, 0x9c, 0x3a, 0xac, 0x4f, 0xfc, 0x30,
	0x51, 0x16, 0xbb, 0x25, 0x8a, 0xb7, 0x1e, 0x5b, 0x0d, 0xdf, 0x5e, 0xa2, 0xd5, 0xb1, 0x44, 0x12,
	0x06, 0x03, 0xda, 0xbd, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
	0x86, 0xf7, 0x0d, 0x01, 0x01, 0x04, 0x05, 0x00, 0x03, 0x81, 0x81, 0x00, 0x52, 0x11, 0xfe, 0x5e,
	0x0c, 0xc9, 0x81, 0x34, 0x8f, 0x36, 0x6a, 0xcf, 0x8d, 0x6b, 0xd8, 0x8a, 0x80, 0xf8, 0x75, 0x10,
	0x3d, 0x27, 0x56, 0x3e, 0x90, 0xec, 0x48, 0xda, 0x80, 0xf1, 0x2d, 0x97, 0x41, 0x6f, 0xbe, 0x33,
	0x81, 0x46, 0x4a, 0xf8, 0x1c, 0xbc, 0x58, 0x09, 0x1e, 0x95, 0x09, 0x39, 0x49, 0x4f, 0x69, 0xf7,
	0x3b, 0x1c, 0x6d, 0xd7, 0xb8, 0x49, 0x0b, 0x86, 0xc9, 0xbd, 0x1b, 0x7a, 0x4e, 0x7b, 0xd3, 0x02,
	0x2c, 0xfe, 0xf6, 0x8e, 0xf2, 0x84, 0x64, 0xf3, 0x78, 0xd6, 0xad, 0xe7, 0xdc, 0x0e, 0x91, 0x3b,
	0x57, 0x98, 0x36, 0xe0, 0x07, 0xdc, 0x4d, 0x6d, 0x3b, 0xcb, 0xde, 0x90, 0xeb, 0x79, 0xd3, 0xec,
	0xc6, 0xf6, 0x9e, 0x35, 0xb9, 0x91, 0xfe, 0x11, 0x33, 0x7e, 0xec, 0xc2, 0x4a, 0xa4, 0xbb, 0xc5,
	0x85, 0xb5, 0x5f, 0x29, 0x2a, 0xde, 0xd5, 0xf9, 0x22, 0xe2, 0x65, 0x20}; 

uint8 modulus[MAX_RSA_MODULUS_LEN]={
	0xc7, 0xa7, 0x41, 0x27, 0x66, 0x58, 0x1d, 0xf1, 0xf0, 0x73, 0x80, 0xad, 0xcd, 0x73, 0x86, 0xab, 
    0x2e, 0x9d, 0x21, 0x49, 0x0e, 0x12, 0x81, 0x46, 0x2d, 0x99, 0x76, 0x5d, 0x7d, 0xdb, 0x5c, 0xc5, 
    0x71, 0xd6, 0x82, 0xb1, 0xac, 0x16, 0x66, 0xd2, 0x89, 0x06, 0x7a, 0x65, 0xb2, 0x0a, 0x5f, 0x44, 
    0x53, 0x8c, 0x12, 0xad, 0x30, 0xa0, 0x53, 0xa8, 0x84, 0x5c, 0x68, 0xc0, 0x7c, 0x29, 0xd1, 0x74, 
    0xa1, 0x42, 0x13, 0x26, 0xc0, 0xc4, 0x5d, 0xfc, 0x0e, 0x1c, 0x83, 0x08, 0xa5, 0x8d, 0x12, 0x03, 
    0x00, 0x9a, 0x40, 0x6e, 0x64, 0x23, 0xe6, 0xa6, 0x0a, 0xa0, 0xa8, 0xf7, 0xd2, 0xa9, 0xb1, 0xd6, 
    0x47, 0x19, 0x08, 0xa0, 0xef, 0x9b, 0x9c, 0x3a, 0xac, 0x4f, 0xfc, 0x30, 0x51, 0x16, 0xbb, 0x25, 
    0x8a, 0xb7, 0x1e, 0x5b, 0x0d, 0xdf, 0x5e, 0xa2, 0xd5, 0xb1, 0x44, 0x12, 0x06, 0x03, 0xda, 0xbd
};

uint8 privateExponent[MAX_RSA_MODULUS_LEN]={
	0x6b, 0xef, 0x49, 0xd3, 0xe9, 0x0a, 0x5e, 0x35, 0xfb, 0xce, 0x15, 0x94, 0xe0, 0x6b, 0x42, 0x34, 
    0xad, 0x20, 0x99, 0x6c, 0x0b, 0x45, 0x89, 0xc9, 0xcc, 0xc1, 0x18, 0x32, 0x76, 0xe5, 0xee, 0x83,
    0x77, 0x85, 0x86, 0x66, 0x53, 0x28, 0x51, 0x32, 0x24, 0x8d, 0xf2, 0x01, 0xa4, 0x7c, 0xe2, 0xff,
    0xe7, 0x42, 0xe4, 0x9d, 0xe8, 0x99, 0xe4, 0x60, 0x48, 0x1e, 0x45, 0x2b, 0x3a, 0x74, 0xc9, 0x1c,
    0x94, 0x71, 0xcd, 0x73, 0x27, 0x87, 0xaa, 0xb8, 0x64, 0x37, 0x9f, 0x67, 0x89, 0x7f, 0x70, 0x6f,
    0xeb, 0xe3, 0x06, 0xb4, 0x2f, 0xcd, 0x77, 0xaa, 0x6b, 0x3a, 0x8d, 0x01, 0x44, 0xe8, 0x85, 0x2f,
    0x87, 0x80, 0x43, 0xb7, 0x73, 0xaf, 0xc4, 0xf1, 0xa9, 0x93, 0x65, 0x7d, 0xf8, 0x74, 0xe0, 0x48,
    0x5a, 0xba, 0xc6, 0x9f, 0xeb, 0x24, 0x45, 0x44, 0xad, 0xb5, 0x1c, 0x3a, 0xaa, 0x4b, 0x89, 0xa9
};

uint8 prime[2][MAX_RSA_PRIME_LEN]={
   {
		0xfb, 0x94, 0xd8, 0x1a, 0x74, 0xd9, 0x0b, 0x70, 0x4f, 0x05, 0x83, 0x1c, 0xa5, 0x6b, 0x09, 0x8a,
    	0x13, 0xac, 0x55, 0x21, 0x79, 0x10, 0xa3, 0x18, 0xc8, 0x05, 0xa6, 0xbe, 0xa9, 0xca, 0x26, 0xed,
    	0x05, 0x07, 0x5d, 0xea, 0x02, 0x2f, 0x7d, 0xd2, 0x9b, 0x2f, 0x7a, 0x1d, 0x3a, 0x60, 0xa8, 0x6c,
    	0x4f, 0xbd, 0x18, 0xa5, 0xdf, 0x19, 0xb4, 0x8c, 0x7e, 0x38, 0x4d, 0xd8, 0x21, 0xf8, 0x82, 0xab
   },
   {
		0xcb, 0x28, 0xee, 0xa7, 0x29, 0x42, 0x7d, 0x3f, 0x8d, 0xc8, 0x67, 0x3d, 0x05, 0x5b, 0x6e, 0xdc,
    	0xcf, 0x05, 0xf9, 0xd0, 0xa2, 0xc0, 0x16, 0x6c, 0x0b, 0x95, 0x5b, 0x4c, 0xfc, 0xb2, 0xab, 0xe7,
    	0x0a, 0xc7, 0xe6, 0x66, 0x3c, 0xb8, 0x84, 0x4e, 0x09, 0x6e, 0x6f, 0x4e, 0xa7, 0xfe, 0xea, 0x5a,
    	0x5a, 0x1f, 0xf9, 0xb1, 0xa4, 0xd5, 0xee, 0xec, 0xd4, 0xce, 0xcd, 0x28, 0x6e, 0x1f, 0x58, 0x37
   }
};

uint8 primeExponent[2][MAX_RSA_PRIME_LEN]={
   { 
		0x87, 0xaa, 0x7b, 0x62, 0xc0, 0xf2, 0x2e, 0x3d, 0xf1, 0x0f, 0x0a, 0xe6, 0x0e, 0xba, 0x0a, 0xd9,
    	0xdf, 0xc4, 0x05, 0xe0, 0x58, 0x15, 0xc5, 0xad, 0xde, 0x67, 0x55, 0xc4, 0xe9, 0x70, 0xae, 0xcf,
    	0x0f, 0xf0, 0x2a, 0x0f, 0xbf, 0xff, 0xf6, 0x3c, 0xf5, 0x1c, 0x84, 0x20, 0x70, 0xd3, 0xa0, 0xdf,
    	0x4f, 0x9c, 0xac, 0x46, 0xfe, 0x69, 0x31, 0x38, 0xb5, 0xf4, 0x0c, 0xde, 0x69, 0x6b, 0x46, 0xa3  
   },
   {
		0x96, 0x66, 0xf9, 0x37, 0x5b, 0xbe, 0x53, 0x87, 0x27, 0x6a, 0x03, 0xb4, 0x13, 0x31, 0x3b, 0x83,
    	0xd0, 0xe7, 0xf5, 0xa7, 0xd5, 0xb5, 0x2b, 0x64, 0x55, 0xb6, 0xfe, 0xde, 0xa1, 0xfd, 0x8c, 0x72,
    	0x01, 0x73, 0x38, 0x48, 0xdc, 0x0f, 0x26, 0x33, 0xb7, 0x9b, 0xc0, 0x5f, 0x94, 0xd3, 0x8d, 0x9f,
    	0xd2, 0x34, 0xf5, 0xdf, 0xe3, 0xce, 0x55, 0x91, 0x93, 0x82, 0x7e, 0x33, 0xbe, 0x43, 0x0a, 0x45
   }
};

uint8 coefficient[MAX_RSA_PRIME_LEN]={
	0x18, 0xd6, 0x24, 0xa3, 0x97, 0x8e, 0xdf, 0xff, 0x88, 0x58, 0x40, 0xf6, 0x47, 0x97, 0x4d, 0xaa,
    0xbb, 0xcd, 0xd1, 0x23, 0x03, 0xbe, 0xb2, 0x60, 0xd1, 0x3e, 0x17, 0x8d, 0xc2, 0x3e, 0x3e, 0x51,
    0x8a, 0x1d, 0xe3, 0x83, 0x7d, 0x55, 0x31, 0x9d, 0xe5, 0x34, 0xa4, 0x39, 0xf6, 0x54, 0xc1, 0xa5,
    0x40, 0xf2, 0xe8, 0x2e, 0xbd, 0xe4, 0xed, 0xda, 0xf0, 0x44, 0x8a, 0xa4, 0xb3, 0x9e, 0x4b, 0xf3
};

uint8 ca_dn[]={
	0x00, 0x8b, // ca dn length
	0x30, 0x81, 0x88, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x43, 0x4e,
	0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x07, 0x62, 0x65, 0x69, 0x6a, 0x69,
	0x6e, 0x67, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 0x07, 0x62, 0x65, 0x69,
	0x6a, 0x69, 0x6e, 0x67, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x06, 0x41,
	0x4f, 0x53, 0x4c, 0x54, 0x44, 0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x03,
	0x41, 0x4f, 0x53, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x05, 0x41, 0x4f,
	0x53, 0x43, 0x41, 0x31, 0x26, 0x30, 0x24, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
	0x09, 0x01, 0x16, 0x17, 0x61, 0x6f, 0x73, 0x63, 0x61, 0x40, 0x69, 0x70, 0x61, 0x63, 0x6b, 0x65,
	0x74, 0x65, 0x6e, 0x67, 0x69, 0x6e, 0x65, 0x2e, 0x63, 0x6f, 0x6d
};


//
// Description:
//	This function generate serverHello message.
//	1. Set server version to message.
//	2. Set server random to message.
//	3. Set session number to message.
//	4. Set selected cipher suite to message.
//	5. Set selected compression to message.(always is null)
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//    resume: 1 resume sessionid, it is stored in context, 0 do not resume
//  Output:
//	record:Pointer to the output severHello record.
// Return value:
//	eAosRc_Success: success
//    <0: error returned
//
int AosSsl_genSrvHelloRec(struct AosSslContext *context, AosSslRecord_t *record, int resume)
{
	uint8* data = (uint8*)AosSslRecord_getMsgBody(record);
	int pos = 0;
#ifdef __KERNEL__
	uint32_t	len;
	struct sockaddr_in addr;
	struct sock *csock;
#endif

	//ServerHello
	//	server_version 	2 bytes
	//	random			32 bytes
	//	sessionid			<0..32>
	//	cipher_suites		<2..2^16-1>
	//	compression		<1..2^8-1>
	//

	// create server_version
	data[pos++]=(uint8)eAosSSL_VersionMajor;
	data[pos++]=(uint8)eAosSSL_VersionMinor;

	// create server random
	//if(!resume || !aos_ssl_hardware_flag)
	if(!resume || (gAosSslAcceleratorType != eAosSslAcceleratorType_517PM) )
		AosRandom_get((char*)(context->server_random), RANDOM_LENGTH);
	memcpy(data+pos, context->server_random, RANDOM_LENGTH);
	pos += RANDOM_LENGTH;

	// create session ID and store it into context
	if (!resume) 
		AosRandom_get((char*)(context->session_id), eAosSSL_SessionIdLen);
	data[pos++] = (uint8)eAosSSL_SessionIdLen;

#ifdef __KERNEL__
	csock = (struct sock*)context->sock;

	aos_inet_getname(csock,(struct sockaddr*)&addr,&len,0);	
	len = htonl(addr.sin_addr.s_addr);
	memcpy(context->session_id, &len,sizeof(int));
	
	aos_inet_getname(csock,(struct sockaddr*)&addr,&len,1);	
	len = htonl(addr.sin_addr.s_addr);
	memcpy(context->session_id+4, &len,sizeof(int));
#endif

	memcpy(data+pos, context->session_id, eAosSSL_SessionIdLen);
	pos += eAosSSL_SessionIdLen;

	// create selected ciphersuite, only one cipher here
	data[pos++] = (context->cipher_selected&0xff00)>>8;
	data[pos++] = context->cipher_selected&0x00ff;

	// create compression method, the compression method is null
	data[pos++]=0x00;	

	// create record
	AosSslRecord_createHandshake( record, eAosSSLMsg_ServerHello, pos);
	// store ServerHello message into context
	AosSSL_addhandshake(context, record);

	return eAosRc_Success;
}


//
// Description:
//	This function generate certificate request message record.
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//  Output:
//   record: the output buffer of record
// Return value:
//	0: success
//    <0: error returned
//
int AosSsl_genCertReqRec(struct AosSslContext *context, AosSslRecord_t *record)
{
	struct _X509_CERT_INFO  *certs;
	uint8* data = (uint8*)AosSslRecord_getMsgBody(record);
	uint16 dn_len, dn_total_len = 0;
	int pos = 0;
	int i;

	//CertificateRequest
	//	certificate_types<1..2^8-1>
	//	certificate_authorities<3..2^16-1>

	// create certificate_types, now we only support rsa_sign;
	data[pos++] = 1;
	data[pos++] = rsa_sign;
	
	// skip the total dn length field, will be filled at the end of generation
	pos += 2;
	
	// create certificate_authorities
	aos_assert1(context->authorities);
	for (i = 0; i < (int)(context->authorities->num_certs); i++)
	{
		certs = context->authorities->certs[i];
		aos_assert1(certs);
		dn_len = certs->subject_der_len;
		data[pos++] = (dn_len>>8) & 0x00ff;
		data[pos++] = dn_len&0x00ff;
		memcpy(data+pos, certs->subject_der, dn_len);
		pos += dn_len;
		dn_total_len += 2+dn_len;
	}

	data[2] = (dn_total_len>>8) & 0x00ff;
	data[3] = dn_total_len&0x00ff;

	// create record
	AosSslRecord_createHandshake( record, eAosSSLMsg_CertificateRequest, pos);
	// store Server CertificateRequest message into context
	AosSSL_addhandshake(context, record);

	return eAosRc_Success;
}

//
// Description:
//	This function generate ServerHelloDone message record
//	This message is null message, only set head of message.
// Arguments:
//  Input:
//	context:Point to the struct AosSslContext.
//  Output:
//   record: the output buffer of record
// Return value:
//	>=0: Success
//    <0: error returned
//
int AosSsl_genSrvHelloDoneRec(struct AosSslContext *context, AosSslRecord_t *record)
{
	// create record
	AosSslRecord_createHandshake( record, eAosSSLMsg_ServerHelloDone, 0);
	// store ServerHelloDone message into context
	AosSSL_addhandshake(context, record);

	return eAosRc_Success;
}

int AosSslPoto_Established(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	*errcode = 0;
	//aos_trace(" AosSslPoto_Established ");
	return eAosRc_Success;
}

static int AosSslServer_ClientFinished_Csp1(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg, *msg_created;
	int msg_len;
	int finished_msg_len;
	int ret;
		
//aos_min_log( eAosMD_TcpProxy, "ClientFinished_Csp1 start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log(eAosMD_SSL, "receive and try to verify Client Finished message\n");

	aos_assert1(context);
	aos_assert1(record->record_cur_type == eAosContentType_Handshake);

	msg = AosSslRecord_getMsg(record);
	msg_len = AosSslRecord_getMsgLen( record );
	// 
	// Need to verify the client Finished message. 
	//
	if ( memcmp(context->enc_clt_fin_msg, msg, msg_len) != 0 )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Client Finished message verify failed");
		return -eAosRc_FinishedIncorrect;
	}

	context->flags &= ~eAosSSLFlag_ExpectFinished;
	record->msg_cur_type = eAosSSLMsg_Finished;
	record->msg_cur_body_len = 60;

	if (context->handshake)
	{
		aos_free(context->handshake);
		context->handshake = NULL;
		context->handshake_len = 0;
	}

	if (context->is_resumed)
	{
		context->state = eAosSSLState_Established;
		context->state_func = AosSslPoto_Established;
		AosSsl_preEstablished(context);
		return eAosRc_Success;
	}
	
	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
	// create Server Change Cipher record
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	// create Server Finished record
	msg_created = AosSslRecord_getMsg( record_out );
	AosSsl_getFinishMsgLen( context, &finished_msg_len );
	memcpy( msg_created, context->enc_svr_fin_msg, finished_msg_len );
	AosSslRecord_createFinishedRecord( record_out, finished_msg_len );

	// 
	// Send the message
	//
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
		AosSslRecord_release( record_out );

	*errcode = 0;

	if (context->enc_svr_fin_msg)
	{
		aos_free(context->enc_svr_fin_msg);
		context->enc_svr_fin_msg = NULL;
	}

	if (context->enc_clt_fin_msg)
	{
		aos_free(context->enc_clt_fin_msg);
		context->enc_clt_fin_msg = NULL;
	}
	
	// change state
	context->state = eAosSSLState_Established;
	context->state_func = AosSslPoto_Established;
	AosSsl_preEstablished(context);

//aos_min_log( eAosMD_TcpProxy, "ClientFinished_Csp1 end, wo %p, usec %u", context->wo, AosGetUsec() );
	return eAosRc_Success;
	
}
static int AosSslServer_ClientFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg, *msg_created;
	int msg_len;
	u32 msg_created_len;
	int ret;

	if ( gAosSslAcceleratorType & eAosSslAcceleratorType_CaviumPro )
	{
		return AosSslServer_ClientFinished_Csp1( context, record, callback, errcode );
	}	
		
	aos_debug_log(eAosMD_SSL, "parsing Client Finished message\n");

	aos_assert1(context);
	aos_assert1(record->record_cur_type == eAosContentType_Handshake);

	msg = AosSslRecord_getMsg(record);
	msg_len = AosSslRecord_getMsgLen( record );



	aos_trace( "cipher_selected=%d", context->cipher_selected );
#ifdef __KERNEL__
	if(!IS_USBKEY_USER(context))
		context->wo->isSecureUser=1;
	
	if(context->flags & eAosSSLFlag_ClientAuth && 
		!context->is_resumed && IS_USBKEY_USER(context))
	{
		char * user_id;
		int len;

		user_id = (char*)context->peer_cert->subject_unique_id.data;
		len = context->peer_cert->subject_unique_id.len;
		aos_trace("usbkey user id =%s, processing ...",user_id);
		/*
		if( ! is_the_same_lan((struct socket*)(context->sock)) )
		{
			aos_trace("usbkey user id =%s, not in lan, deny",user_id);
			goto error;
		}
		*/

		if(20!=len || !user_id)
		{
			aos_trace("user id len=%d, user_id addr=%xd",len,user_id);
			goto error;
		}
		user_id[USBKEYLEN]=0;
/*		if( !conn->svc->conf.pmi )
		{
			if( !in_usbkey_list(user_id) )
			{
				aos_trace("usbkey user id =%s, not in list, deny",user_id);
				goto error;
			}
		}*/
		aos_trace("usbkey user id =%s, allow",user_id);
		goto success;
error:
		context->state = eAosSSLState_Error;
		//*errcode = eAosSSLErr_LevelFatal|eAosSSLErr_UsbkeyError;
        	AosSsl_createAndSendAlertRecord(context, eAosSSLErr_LevelFatal|eAosSSLErr_UsbkeyError);
		context->flags &= ~eAosSSLFlag_ExpectFinished;
		record->msg_cur_type = eAosSSLMsg_Finished;
		record->msg_cur_body_len = 60;
		return 0;
success:
		;
	}
#endif

	// 
	// Need to verify the client Finished message. 
	//
	if ( eAosRc_Success != AosSSL_verifyFinished(context, eAosSSLClient, (uint8*)msg, msg_len) ) 
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Client Finished message verify failed");
		return -eAosRc_FinishedIncorrect;
	}

	context->flags &= ~eAosSSLFlag_ExpectFinished;
	record->msg_cur_type = eAosSSLMsg_Finished;
	record->msg_cur_body_len = 60;

	if (context->is_resumed)
	{
		context->state = eAosSSLState_Established;
		//context->state_func = AosSslPoto_Established;
		//AosSsl_preEstablished(context);
		context->state_func = AosSslStm_Established;
		return eAosRc_Success;
	}
	
	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
	aos_assert1( record_out );
	// create Server Change Cipher record
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	// create Server Finished record
	msg_created = AosSslRecord_getMsg( record_out );
	AosSSL_createFinishedMsg(
		context->hash_len, 
		context, 
		msg_created, 
		&msg_created_len, 
		eAosSSLServer);
	aos_assert1(msg_created_len > 4);
	AosSslRecord_createFinishedRecord( record_out, msg_created_len );

	// 
	// Send the message
	//
	if ((ret = AosSslRecord_send( record_out, context )) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
	AosSslRecord_release( record_out );

	*errcode = 0;
	if (context->handshake)
	{
		aos_free(context->handshake);
		context->handshake = NULL;
		context->handshake_len = 0;
	}

	// change state
	context->state = eAosSSLState_Established;
	// context->state_func = AosSslPoto_Established;
	//AosSsl_preEstablished(context);
	
	context->state_func = AosSslStm_Established;

	return eAosRc_Success;
}

static int AosSslServer_ClientChangeCipher(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	aos_debug_log(eAosMD_SSL, "parsing Client ChangeCipherSpec message\n");

	aos_assert1(context);
	aos_assert1(record->record_cur_type == eAosContentType_ChangeCipherSpec);

	context->flags |= eAosSSLFlag_ExpectFinished;
	context->state = eAosSSLState_ClientChangeCipherRcvd;
	context->state_func = AosSslServer_ClientFinished;

	return eAosRc_Success;
}

static int AosSslServer_Csp1FinishedXFinish_Finish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	aos_debug_log(eAosMD_SSL, "AosSslServer_Csp1FinishedXFinish_Finish\n");

	// if resumed, save the session
	if (AosSsl_ServerSessionReuse)
	{
		AosSsl_SessionSave(
			"test",
			context->session_id,
			sizeof(context->session_id), 
			context->master_secret, 
			sizeof(context->master_secret),
			context->client_random,
			context->server_random,
			context->peer_cert? &(context->peer_cert->serial_number):NULL,
			(uint32)86400);
	}
	context->state = eAosSSLState_ClientKeyExchangeRcvd;
	context->state_func = AosSslServer_ClientChangeCipher;
	return eAosRc_Success;

}

static int AosSslServer_Csp1ClientCertVerifyFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

	aos_trace("Csp1ClientCertVerifyFinish entry");

	aos_assert1( context->veri_rcvd_len == MD5_LEN+SHA1_LEN );
	aos_assert1( context->veri_rcvd );
	aos_assert1( context->verify_data );
	
	// compare
	if ( memcmp( context->veri_rcvd, context->verify_data, MD5_LEN+SHA1_LEN) != 0 )
	{
		aos_free( context->veri_rcvd );
		context->veri_rcvd = NULL;
		context->veri_rcvd_len = 0;
		aos_free(context->verify_data);
		context->verify_data = NULL;

		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1ClientCertVerifyFinish, client CertificateVerify invalid");
	}

	aos_free( context->veri_rcvd );
	context->veri_rcvd = NULL;
	context->veri_rcvd_len = 0;
	aos_free( context->verify_data );
	context->verify_data = NULL;

	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		if (AosSsl_mallocFinishedMessage(context))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client or Server Finished Message allocate failed");
		}
		ret = Kernel_Csp1FinishedRc4Finish (
			gAosSslCaviumRequestType,
			context->cavium_context,
			(HashType)context->hash_type,
			context->ssl_version,
			context->handshake_len,
			context->handshake,
			context->enc_clt_fin_msg,
			context->enc_svr_fin_msg, 
			&request_id,
			AosSslStm_cardCallback,
			(void*)record);
		break;
	default:
		return	aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
				"AosSslServer_ClientKeyExchange_Csp1RSAServerVerify cipher %d is not supported", context->encrypt_type);
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1Finished*Finish failed, ret is %d, cipher is %d", ret, context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslServer_Csp1FinishedXFinish_Finish;

	return eAosRc_Pending;	
	
}

static int AosSslServer_Csp1ClientCertVerify(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *msg_body;
	int msg_body_len;
	uint8 exponent[3] = {1, 0, 1};
	uint32 request_id;
	int ret;

	aos_trace("Csp1ClientCertVerify, parsing client cert verify message\n");

	aos_assert1(context);
	aos_assert1(record->msg_cur_type == eAosSSLMsg_CertificateVerify);

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	aos_assert1(msg_body_len == MAX_RSA_MODULUS_LEN+2);
	
	// store this message into context
	context->handshake_len = 0;
	AosSSL_addhandshake(context, record);
	
	// decrypt CertificateVerify use client pubkey
	context->veri_rcvd = (uint8*)aos_malloc_atomic( context->peer_cert->pubkey.modulus.len );
	aos_assert1( context->veri_rcvd );
				
	ret = Kernel_Csp1Pkcs1v15Dec(
			gAosSslCaviumRequestType,
			BT1,
			context->peer_cert->pubkey.modulus.len, 
			3,
			context->peer_cert->pubkey.modulus.data, 
			exponent, 
			(uint8*)(msg_body+2),
			&context->veri_rcvd_len,
			context->veri_rcvd,
			&request_id,
			AosSslStm_cardCallback,
			(void*) record);
	if ( ret )
	{
		aos_free( context->veri_rcvd );
		context->veri_rcvd = 0;
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Failed to decrypt clientverify: ret %d", ret);
	}
	
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslServer_Csp1ClientCertVerifyFinish;
					
	return eAosRc_Pending;	
	
}

static int AosSslServer_ClientCertVerify(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	struct R_RSA_PUBLIC_KEY pubkey;
	char *msg_body;
	uint8 veri_rcvd[MAX_RSA_MODULUS_LEN];
	uint8 veri_expected[MD5_LEN+SHA1_LEN];
	int msg_body_len, veri_rcvd_len;
	int ret;

	//aos_trace("parsing Client CertificateVerify message\n");

	aos_assert1(context);
	aos_assert1(record->msg_cur_type == eAosSSLMsg_CertificateVerify);

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	aos_assert1(msg_body_len == MAX_RSA_MODULUS_LEN+2);
	// decrypt CertificateVerify use client pubkey
	pubkey.bits = (context->peer_cert->pubkey.modulus.len << 3);
	memcpy(pubkey.modulus, context->peer_cert->pubkey.modulus.data, context->peer_cert->pubkey.modulus.len);
	memset(pubkey.exponent, 0, MAX_RSA_MODULUS_LEN);
	pubkey.exponent[MAX_RSA_MODULUS_LEN-1] = 1;
	pubkey.exponent[MAX_RSA_MODULUS_LEN-3] = 1;

	if (0 != aos_ssl_asycipher.pubdec (
					(unsigned char*)veri_rcvd, 
					(unsigned int*)&veri_rcvd_len, 
					(unsigned char*)msg_body+2, 
					msg_body_len-2, 
					&pubkey))
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "client CertificateVerify decryption failed");
	}	
	aos_assert1(veri_rcvd_len == MD5_LEN+SHA1_LEN);

	AosSsl_createCltCertVeriMsg(context, veri_expected, MD5_LEN+SHA1_LEN);

	// compare
	if ( memcmp(veri_rcvd, veri_expected, MD5_LEN+SHA1_LEN) != 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "client CertificateVerify invalid");
	}

	// store this message into context
	AosSSL_addhandshake(context, record);
	
	context->state = eAosSSLState_ClientCertVerifyRcvd;
	context->state_func = AosSslServer_ClientChangeCipher;
	aos_trace_hex("cert serial", (char*)(context->peer_cert->serial_number.data), context->peer_cert->serial_number.len);

	AosCert_hold(context->peer_cert);
	//AosAppProxy_connHold(conn);
	if (eAosRc_Success == (ret=aos_verify_cert(
			(void*)context->peer_cert, 
			context, 
			AosSslStm_ocspCallback))) 
	{
		context->state = eAosSSLState_Pending;
		return eAosRc_Pending;
	}
	AosCert_put(context->peer_cert);
	//AosAppProxy_connPut(conn);

	//aos_trace("aos_verify_cert return is %d", ret);
	
	return eAosRc_Success;
}

static int AosSslServer_ClientKeyExchange_Csp1RSAServerVerifyFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	aos_debug_log(eAosMD_SSL, "ClientKeyExchange Csp1RSAServerVerify finish\n");

	// if resumed, save the session
	if (AosSsl_ServerSessionReuse)
	{
		AosSsl_SessionSave(
			"test",
			context->session_id,
			sizeof(context->session_id), 
			context->master_secret, 
			sizeof(context->master_secret),
			context->client_random,
			context->server_random,
			context->peer_cert? &(context->peer_cert->serial_number):NULL,
			(uint32)86400);
	}
	context->state = eAosSSLState_ClientKeyExchangeRcvd;
	context->state_func = AosSslServer_Csp1ClientCertVerify;
	return eAosRc_Success;
}

static int AosSslServer_ClientKeyExchange_Csp1RSAServerVerify(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *msg_body;
	int msg_body_len;
	uint32 request_id = 0;
	int ret;

	aos_debug_log(eAosMD_SSL, "parsing ClientKeyExchange message, Csp1RSAServerVerify\n");

	aos_assert1(context);
	aos_assert1(record->msg_cur_type == eAosSSLMsg_ClientKeyExchange);

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	if (msg_body_len != ENCRYPT_PRE_MASTER_SECRET_LENGTH)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "ClientKeyExchange msg body len %d error", msg_body_len);
		return eAosAlarm_SSLSynErr;
	}

	// store message into context
	AosSSL_addhandshake(context, record);
	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		context->verify_data = (uint8*)aos_malloc_atomic(MAX_VERIFY_DATA_SIZE);
		aos_assert1(context->verify_data);
		ret = Kernel_Csp1RsaServerVerifyRc4(
				gAosSslCaviumRequestType,
				context->cavium_context,
				&context->cavium_keyhandle,
				(HashType)context->hash_type,
				context->ssl_version,
				RC4_128,
				RETURN_ENCRYPTED,
				MAX_RSA_MODULUS_LEN,
				(uint8 *)msg_body,
				context->client_random,
				context->server_random,
				context->handshake_len,
				context->handshake,
				context->verify_data,
				context->master_secret,
				&request_id,
				AosSslStm_cardCallback,
				(void*)record);
		break;
	default:
		return	aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
				"AosSslServer_ClientKeyExchange_Csp1RSAServerVerify cipher %d is not supported", context->encrypt_type);
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1RsaServerVerify* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslServer_ClientKeyExchange_Csp1RSAServerVerifyFinish;

	/*return AosSslServer_ClientKeyExchange_Csp1RSAServerVerifyFinish(
				record, 
				context,
				callback, 
				errcode);	
	*/			
	return eAosRc_Pending;
}


static int AosSslServer_ClientKeyExchange_Csp1RSAServerFull_Finish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	aos_debug_log(eAosMD_SSL, "ClientKeyExchange Csp1RSAServerFull finish\n");

//aos_min_log( eAosMD_TcpProxy, "ServerFull_Finish start, wo %p, usec %u", context->wo, AosGetUsec() );
	// if resumed, save the session
	if (AosSsl_ServerSessionReuse)
	{
		AosSsl_SessionSave(
			"test",
			context->session_id,
			sizeof(context->session_id), 
			context->master_secret, 
			sizeof(context->master_secret),
			context->client_random,
			context->server_random,
			context->peer_cert? &(context->peer_cert->serial_number):NULL,
			(uint32)86400);
	}
	context->state = eAosSSLState_ClientKeyExchangeRcvd;
	context->state_func = AosSslServer_ClientChangeCipher;
	return eAosRc_Success;
}

static int AosSslServer_ClientKeyExchange_Csp1RSAServerFull(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *msg_body;
	int msg_body_len;
	uint32 request_id = 0;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "ServerFull start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log(eAosMD_SSL, "parsing ClientKeyExchange message, Cavium RSAServerFull\n");

	aos_assert1( context );
	aos_assert1( record->msg_cur_type == eAosSSLMsg_ClientKeyExchange );

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	if (msg_body_len != ENCRYPT_PRE_MASTER_SECRET_LENGTH)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "ClientKeyExchang msg body len %d error", msg_body_len);
		return eAosAlarm_SSLSynErr;
	}

	// store message into context
	AosSSL_addhandshake(context, record);
	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client or Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}

		ret = Kernel_Csp1RsaServerFullRc4 (
			      gAosSslCaviumRequestType,
			      context->cavium_context,
			      &context->cavium_keyhandle,
			      (HashType)context->hash_type,
			      context->ssl_version, // VER3_0
			      RC4_128,
			      RETURN_ENCRYPTED,
			      MAX_RSA_MODULUS_LEN,	//default modlength is 128 Bytes
			      (uint8*)msg_body,
			      context->client_random,
			      context->server_random,
			      context->handshake_len,
			      context->handshake,
			      context->enc_clt_fin_msg,
			      context->enc_svr_fin_msg,
			      context->master_secret, 
			      &request_id,
			      AosSslStm_cardCallback,
			      (void*)record);
		break;
	default:
		return	aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
				"AosSslServer_ClientKeyExchange_Csp1RSAServerFull cipher %d is not supported", context->encrypt_type);
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1RsaServerFull* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslServer_ClientKeyExchange_Csp1RSAServerFull_Finish;

	/*return AosSslServer_ClientKeyExchange_Csp1RSAServerFull_Finish(
				record, 
				context,
				callback, 
				errcode);	
	*/			
//aos_min_log( eAosMD_TcpProxy, "ServerFull end, wo %p, usec %u", context->wo, AosGetUsec() );
	return eAosRc_Pending;
}

static int AosSslServer_ClientKeyExchange(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *msg_body;
	int msg_body_len, key_block_length;
	int pos = 0;
	// uint16 out_len;
	u32 out_len;
	int ret;


	if (gAosSslAcceleratorType & eAosSslAcceleratorType_CaviumPro)
	{
		if (!(context->flags & eAosSSLFlag_ClientAuth))
		{
			return AosSslServer_ClientKeyExchange_Csp1RSAServerFull( context, record, callback, errcode );
		}
		else
		{
			return AosSslServer_ClientKeyExchange_Csp1RSAServerVerify( context, record, callback, errcode );
		}
	}	
	aos_debug_log(eAosMD_SSL, "parsing ClientKeyExchange message\n");

	aos_assert1( context );
	aos_assert1( record->msg_cur_type == eAosSSLMsg_ClientKeyExchange );

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	if (msg_body_len != ENCRYPT_PRE_MASTER_SECRET_LENGTH)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "ClientKeyExchang msg body len %d error", msg_body_len);
		return eAosAlarm_SSLSynErr;
	}
	
	if(context->auth_type == eAosAuthType_RSA)
	{
		ret = aos_ssl_asycipher.pridec(
				context->pre_master_secret, 
				(unsigned int*)&out_len, 
				(uint8*)msg_body, 
				ENCRYPT_PRE_MASTER_SECRET_LENGTH, 
				context->private_key);
		if (ret != eAosRc_Success)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "pre_master_secret decrytion error");
			return eAosAlarm_SSLSynErr;
		}
	}
	pos += msg_body_len;

	// Create the master_secret
	AosSsl_getmaster_secret(context->pre_master_secret, PRE_MASTER_SECRET_LENGTH,
          context->master_secret, context->client_random, context->server_random);	
	
	// if resumed, save the session
	if (AosSsl_ServerSessionReuse)
	{
		AosSsl_SessionSave(
			"test",
			context->session_id,
			sizeof(context->session_id), 
			context->master_secret, 
			sizeof(context->master_secret),
			context->client_random,
			context->server_random,
			context->peer_cert? &(context->peer_cert->serial_number):NULL,
			(uint32)86400);
	}
 
	// Determine the hash length
      	key_block_length = 2*(context->hash_len+context->encrypt_key_len+context->iv_len);

	// Calculate all the keys
   	AosSsl_calculateKeyBlock(context->master_secret, context->server_random,
       context->client_random, context, context->hash_len, key_block_length);
	
	// store message into context
	AosSSL_addhandshake(context, record);

	context->state = eAosSSLState_ClientKeyExchangeRcvd;
	if (context->flags & eAosSSLFlag_ClientAuth)
		context->state_func = AosSslServer_ClientCertVerify;
	else
		context->state_func = AosSslServer_ClientChangeCipher;
	return eAosRc_Success;
}

// Decription:
//	1. Decode client cert, store cert into context
//	2. Verify client certificate signature
//	3. Validate client certificate by CRL or OCSP
//
// 
static int AosSslServer_ClientCert(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *msg_body;
	int msg_body_len;
	int ret;
		
	aos_trace("parsing ClientCertificate message\n");

	aos_assert1(context);

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	// check message type
	if ( record->msg_cur_type != eAosSSLMsg_Certificate )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "it is not the client certificate message");
		aos_trace_hex("client hello record", msg_body, msg_body_len);
		return -eAosAlarm_SSLProcErr;
	}

	// Decode the first certificate, get its public key used in ClientVerify
	ret = AosSsl_decodeCertMsg((uint8*)msg_body, msg_body_len, &context->peer_cert);
	if (ret != eAosRc_Success)
		return ret;

	aos_assert1(context->peer_cert);
	
	// store Client Certificate into context
	AosSSL_addhandshake(context, record);

	context->callback = callback;
	
	context->state = eAosSSLState_ClientCertRcvd;
	context->state_func = AosSslServer_ClientKeyExchange;

	errcode = 0;
	return eAosRc_Success;
}

int AosSslServer_Csp1ResumeFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg_created;
	int finished_msg_len;
	int ret;

	aos_trace("AosSslServer_Csp1ResumeFinish");
	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output);
	aos_assert1( record_out );
	//  Create ChangeCipherSpec
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );


	// create Server Finished record
	msg_created = AosSslRecord_getMsg( record_out );
	AosSsl_getFinishMsgLen( context, &finished_msg_len );
	memcpy( msg_created, context->enc_svr_fin_msg, finished_msg_len );
	AosSslRecord_createFinishedRecord( record_out, finished_msg_len );

	// 
	// Send the message
	//
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
	AosSslRecord_release( record_out );

	// change state
	context->state = eAosSSLState_ServerFinished;
	context->state_func = AosSslServer_ClientChangeCipher;

	errcode = 0;		
	return eAosRc_Success;
}

int AosSslServer_Csp1Resume(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;
	aos_trace("AosSslServer_Csp1Resume");
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client or Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Kernel_Csp1ResumeRc4 (
			gAosSslCaviumRequestType,
			context->cavium_context,
	       	(HashType)context->hash_type,
	       	context->ssl_version,
	       	RC4_128,
	       	INPUT_ENCRYPTED,
	       	context->client_random,
	       	context->server_random,
	       	context->master_secret,
	       	context->handshake_len,
	       	context->handshake,
	       	context->enc_clt_fin_msg,
	       	context->enc_svr_fin_msg,
	       	&request_id,
	       	AosSslStm_cardCallback,
	       	(void*)record);
		break;
	default:
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslServer_Csp1Resume cipher %d is not supported", context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}

	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1Resume* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslServer_Csp1ResumeFinish;

	return eAosRc_Pending;
	//return AosSslServer_Csp1ResumeFinish(record, context, callback, errcode)	;
}

int AosSslServer_Resume(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg_body, *msg_created;
	int msg_body_len, msg_created_len;
	int pos = 0, ret;
	
	aos_trace("AosSslServer_Resume");
	
	aos_assert1( context );
	aos_assert1( record->msg_cur_type == eAosSSLMsg_ClientHello );
	
	// Resume process part will do the following.
	// 	1. parse client hello and store it into context.
	// 	2. Create server hello and store it into context
	//  3. Create ChangeCipherSpec
	// 	4. Create server finish message.

	// Refresh the handshake in context
	context->handshake_len=0;
	
	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	// ClientVersion+client_random+Sessionid offset
	pos += 2 + RANDOM_LENGTH;
    
	// SessionidLength+Sessionid offset	
	pos += msg_body[pos] + 1;
		
	//Select cipher from clienthello and parse cipher type, store to context
	if (( ret = AosSsl_checkAndSelectCipher(context, msg_body+pos, msg_body_len - pos)) <= 0)
	{
		return -eAosAlarm_SSLSynErr;
	}
	pos += ret;

	if (pos >= msg_body_len)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
			"client hello msg body length %d too short, not include compression", msg_body_len);
		return -eAosAlarm_SSLSynErr;
	}
	//Select compression from clienthello
	if ((ret = AosSsl_selectCompression(context, msg_body+pos, msg_body_len - pos)) < 0)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"client hello msg body length %d too short, not include compression", msg_body_len);
		return -eAosAlarm_SSLSynErr;
	}
	pos += ret;

	// Store ClientHello message into context
	AosSSL_addhandshake(context, record);

	context->is_resumed = 1;

	record_out = AosSslRecord_create( NULL, 512, eAosSslRecord_Output );
	aos_assert1( record_out );
	// Create ServerHello
	if ( AosSsl_genSrvHelloRec( context, record_out, 1 ) != eAosRc_Success )
	{
		AosSslRecord_release( record_out );
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHelloDone message record generation failed");
		return -eAosAlarm_SSLProcErr;
	}
	// Send ServerHello message
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
	AosSslRecord_release( record_out );

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		return AosSslServer_Csp1Resume( context, record, callback, errcode );
	}

	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
	aos_assert1( record_out );
	//  Create ChangeCipherSpec
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	// Determine the hash length
      	context->key_block_len = 2*(context->hash_len+context->encrypt_key_len+context->iv_len);

	// Calculate all the keys
    	AosSsl_calculateKeyBlock(
		context->master_secret, 
		context->server_random,
        	context->client_random, 
		context, 
		context->hash_len, 
		context->key_block_len);

	// create Server Finished record
	msg_created = AosSslRecord_getMsg( record_out );
	AosSSL_createFinishedMsg(
		context->hash_len, 
		context, 
		msg_created, 
		(u32*)(&msg_created_len), 
		eAosSSLServer);
	aos_assert1(msg_created_len > 4);
	AosSslRecord_createFinishedRecord( record_out, msg_created_len );

	// 
	// Send the message
	//
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
	AosSslRecord_release( record_out );


	// change state
	context->state = eAosSSLState_ServerFinished;
	context->state_func = AosSslServer_ClientChangeCipher;

	errcode = 0;		
	return eAosRc_Success;
}

// 
// State: ClientHelloReceived
// Processing:
//		1.Parsing the ClientHello message
//		2.Create the ServerHello message record
//		3.Create ServerCertificate message record
//		4.Create Server CertificateRequest message record if ClientAuth enabled
//		5.Create ServerHello message record
//		
static int AosSslServer_ClientHelloV2(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg;
	int msg_len, cipher_len, sessionid_len, challenge_len;
	int ret;
	
	aos_debug_log(eAosMD_SSL, "parsing sslv2 client hello message\n");

	aos_assert1( context );

	msg = AosSslRecord_getMsg(record);
	msg_len = AosSslRecord_getMsgLen( record ); 


	// ClientHello V2 
	// 	Handshake Type 1 byte
	// 	version 2 bytes
	//  Cipher spec length 2 bytes
	//  SessionID length 2 bytes
	//  Challenge length 2 bytes
	//  Cipher spec <2..2^16>
	//  SessionID   <0..2^16>
	//  Challenge   <2..2^16>
	
	aos_assert1( msg_len > 6 );

	cipher_len = ((msg[3]<<8)&0xff00) + (msg[4]&0x00ff);
	sessionid_len = ((msg[5]<<8)&0xff00) + (msg[6]&0x00ff);
	challenge_len = ((msg[7]<<8)&0xff00) + (msg[8]&0x00ff);

	aos_assert1( 9 + cipher_len + sessionid_len + challenge_len == msg_len );
	
	//Select cipher from clienthello and parse cipher type, store to context
	if ( ( ret = AosSsl_checkAndSelectCipherV2( context, msg+9, cipher_len ) ) <= 0 ) 
	{
		return -eAosAlarm_SSLSynErr;
	}
	
	// Get Client random, and store it into context
	aos_assert1( challenge_len == 16 || challenge_len == 32);
	memcpy( context->client_random+(32-challenge_len), msg+9+cipher_len+sessionid_len, challenge_len );

	// store clienthello into context
	AosSSL_addhandshake(context, record);
	// end of the ClientHello parsing

	
	// change version to sslv3, now we only support sslv3
	context->ssl_version = VER3_0;
	
	record_out = AosSslRecord_create( NULL, eAosSSL_HandshakeMaxLen, eAosSslRecord_Output);
	aos_assert1( record_out );
	//Construct serverHello message
	if ( AosSsl_genSrvHelloRec( context, record_out, 0) != eAosRc_Success )
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHello message record generation failed" );
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	AosSslRecord_moveToNextMsg( record_out );
	
	//Construct certification message
	if ( AosSsl_createCertificateMsg( context, record_out, context->self_cert) != eAosRc_Success )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Server Certificate message record generation failed");
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	AosSslRecord_moveToNextMsg( record_out );
	
	//Construct certificate request message
	if ( context->flags & eAosSSLFlag_ClientAuth )
	{
		aos_trace("Client Auth is set");
		if ( AosSsl_genCertReqRec(context, record_out) != eAosRc_Success )
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Server CertificateRequest message record generation failed");
			AosSslRecord_release( record_out );
			return -eAosAlarm_SSLProcErr;
		}
		AosSslRecord_moveToNextMsg( record_out );
	}
	
	//Construct serverDone message
	if ( AosSsl_genSrvHelloDoneRec( context,  record_out ) != eAosRc_Success )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHelloDone message record generation failed");
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	*errcode = 0;
	// 
	// Send the message
	//
	if ( (ret = AosSslRecord_send( record_out, context )) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
	AosSslRecord_release( record_out );

	context->state = eAosSSLState_ServerHelloDone;
	if (context->flags & eAosSSLFlag_ClientAuth)
		context->state_func = AosSslServer_ClientCert;
	else
		context->state_func = AosSslServer_ClientKeyExchange;

	return eAosRc_Success;
	
}

// 
// State: ClientHelloReceived
// Processing:
//		1.Parsing the ClientHello message
//		2.Create the ServerHello message record
//		3.Create ServerCertificate message record
//		4.Create Server CertificateRequest message record if ClientAuth enabled
//		5.Create ServerHello message record
//		
int AosSslServer_ClientHello(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	struct aosSslSession *session = NULL;
	AosSslRecord_t *record_out;
	char *msg_body;
	int msg_body_len;
	int pos = 0;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "ClientHello start, wo %p, usec %u", context->wo, AosGetUsec() );
	if ( record->ssl_version == VER2_0 )
	{
		context->ssl_version = VER2_0;
		return AosSslServer_ClientHelloV2( context, record, callback, errcode );
	}
	
	aos_debug_log(eAosMD_SSL, "parsing client hello message\n");

	aos_assert1( context );
	aos_assert1( record->msg_cur_type == eAosSSLMsg_ClientHello );

	msg_body = AosSslRecord_getMsgBody(record);
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	
	
	// Check the handshake msg body length
	if (msg_body_len < eAosSSL_ClientHelloMsgBodyMinLen)
	{
		aos_alarm(eAosMD_SSL, eAosAlarmProgErr, 
			"handshake msg body  length error, length returnd %d", msg_body_len);
		return -eAosAlarm_SSLSynErr;
	}

	// ClientHello 
	//   ClientVersion 	2 bytes
	//   client_random 	32 bytes
	//   SessionID       32 bytes
	//	CipherSuite    <2..2^16-1>
	//	Compression  <1..2^8-1>
	
	//Check the version of ssl client, we expect Major 3, Minor 0
	if (msg_body[pos++] !=MAJOR_VERSION || msg_body[pos++] !=MINOR_VERSION)
	{
		aos_trace("ssl version error, major %d: minor %d", msg_body[pos-2], msg_body[pos-1]);	
		return -eAosAlarm_SSLSynErr;
	}
	
	//aos_trace("ssl v3 clienthello received\n");

	// Initialize context
	// ???

	// Get Client random, and store it into context
	memcpy(context->client_random, msg_body+pos, RANDOM_LENGTH);
	pos += RANDOM_LENGTH;

	// Get client sessionid, if not zero, then client want to resume the session
	if ((ret = AosSsl_checkSessionResume(context, msg_body+pos, msg_body_len - pos)) > 1)
	{
		//aos_trace_hex("client want to resume session:", msg_body+pos+1, ret-1);
		aos_debug_log(eAosMD_SSL, "client want to resume session");
	
		if (AosSsl_ServerSessionReuse)	
		{
    		// Find resumed session
			session =  AosSsl_SessionFind((uint8*)msg_body+pos+1, msg_body[pos]);
			if (session)
			{
				memcpy(context->master_secret, session->master_secret, sizeof(session->master_secret));
				if(gAosSslAcceleratorType == eAosSslAcceleratorType_517PM)
				{
					memcpy(context->client_random, session->client_random, RANDOM_LENGTH);
					memcpy(context->server_random, session->server_random, RANDOM_LENGTH);
					memcpy(&context->peer_cert_sn, &session->serial_number, sizeof(asn_data_t));
				}
				//aos_trace_hex("resumed MS:", (char*)context->master_secret, sizeof(context->master_secret));
				return AosSslServer_Resume( context, record, callback, errcode );
			}
			aos_debug_log(eAosMD_SSL, "did not find the resumed session");
		}
		else
		{
			aos_debug_log(eAosMD_SSL, "ssl server session resumed is off");
		}
	}
	else if (ret < 1)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
			"sessionid in client hello message error");
	}
	pos += ret;
		
	//Select cipher from clienthello and parse cipher type, store to context
	if (( ret = AosSsl_checkAndSelectCipher(context, msg_body+pos, msg_body_len - pos)) <= 0)
	{
		return -eAosAlarm_SSLSynErr;
	}
	pos += ret;

	if (pos >= msg_body_len)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, 
			"client hello msg body length %d too short, not include compression", msg_body_len);
		return -eAosAlarm_SSLSynErr;
	}
	//Select compression from clienthello
	if ((ret = AosSsl_selectCompression(context, msg_body+pos, msg_body_len - pos)) < 0)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"client hello msg body length %d too short, not include compression", msg_body_len);
		return -eAosAlarm_SSLSynErr;
	}
	pos += ret;

	// store clienthello into context
	AosSSL_addhandshake(context, record);
	// end of the ClientHello parsing

	
	record_out = AosSslRecord_create( NULL, eAosSSL_HandshakeMaxLen, eAosSslRecord_Output);
	aos_assert1( record_out );
	//Construct serverHello message
	if ( AosSsl_genSrvHelloRec( context, record_out, 0) != eAosRc_Success )
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHello message record generation failed" );
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	AosSslRecord_moveToNextMsg( record_out );
	
	//Construct certification message
	if ( AosSsl_createCertificateMsg( context, record_out, context->self_cert) != eAosRc_Success )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Server Certificate message record generation failed");
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	AosSslRecord_moveToNextMsg( record_out );
	
	//Construct certificate request message
	if ( context->flags & eAosSSLFlag_ClientAuth )
	{
		aos_trace("Client Auth is set");
		if ( AosSsl_genCertReqRec(context, record_out) != eAosRc_Success )
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Server CertificateRequest message record generation failed");
			AosSslRecord_release( record_out );
			return -eAosAlarm_SSLProcErr;
		}
		AosSslRecord_moveToNextMsg( record_out );
	}
	
	//Construct serverDone message
	if ( AosSsl_genSrvHelloDoneRec( context,  record_out ) != eAosRc_Success )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHelloDone message record generation failed");
		AosSslRecord_release( record_out );
		return -eAosAlarm_SSLProcErr;
	}
	*errcode = 0;
	// 
	// Send the message
	//
	if ( (ret = AosSslRecord_send( record_out, context )) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send handshake message, %d", ret);
	}
	if (ret == eAosRc_Success) 
		AosSslRecord_release( record_out );

	context->state = eAosSSLState_ServerHelloDone;
	if (context->flags & eAosSSLFlag_ClientAuth)
		context->state_func = AosSslServer_ClientCert;
	else
		context->state_func = AosSslServer_ClientKeyExchange;

//aos_min_log( eAosMD_TcpProxy, "ClientHello end, wo %p, usec %u", context->wo, AosGetUsec() );
	return eAosRc_Success;
	
}


