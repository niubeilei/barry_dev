////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertificateTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Asn1Tester/CertificateTester.h"

#include "Debug/Debug.h"
#include "PKCS/Certificate.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "ASN1/AsnCodec.h"
#include "Util/OmnNew.h"

#include "Tester/FuncTester.h"
#include "Tester/EnumInt.h"


OmnCertificateTester::OmnCertificateTester()
{
	mName = "OmnCertificateTester";
}


OmnCertificateTester::~OmnCertificateTester()
{
}
 
 
bool 
OmnCertificateTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Certificate Tester";

	temp();
	decodeTester();
	return true;
}

   
bool
OmnCertificateTester::decodeTester()
{
	//
	// This is an example from SSL book [Eric Rescorla]
	// 30 82 01 e9 (01 e9 = 489): SEQUENCE {...}
	//    30 82 01 52 (338): tbsCertificate encoding
	//       02 01 04: SerialNumber: 4
	//       30 0d: AlgorithmId (signature)
	//			06 09: OID
	//			   2a 86 48 86 f7 0d 01 01 04 (1 2 1608 423693 1 1 4) 
	//             05 00 (NULL)
	//       30 5b (SEQUENCE {...} for Issuer
	//		    31 0b (11) SET OF {...}
	//             30 09 SEQUENCE {...}
	//                06 03 OID
	//                   55 04 06 (2 5 4 6)
	//                13 02 PRINTABLE STRING
	//                   41 55 "AU"
	//          31 13 (19) SET OF 
	//             30 11 (17) SEQUENCE
	//                06 03 55 04 08 (2 5 4 8)
	//                13 0a (PRINTABLE STRING)
	//                   51 75 65 65 6e 73 6c 61 6e 64 ("Queensland")
	//          31 1a (SET OF)
	//             30 18
	//                06 03 55 04 0a (2 5 4 10)
	//                13 11 (PRINTABLE STRING)
	//                   43 72 79 70 74 53 65 66 74 20 50 74 79 20 40 74 64
	//	                 ("CryptSoft Pty Ltd")
	//          31 1b (SET OF)
	//             30 19 SEQUENCE 
	//                06 03 55 04 03 (2 5 4 3)
	//                13 12 (PRINTABLE STRING)
	//                   54 65 73 74 20 43 41 20 28 31 30 32 34 20 62 69 74 29
	//					 "Test CA (1024 bit)"
	//       30 1e (30) SEQUENCE for Vadility
	//          17 0d UTCTime
	//             39 38 30 36 32 39 32 33 35 32 34 30 5a
	//          17 0d UTCTime
	//             30 30 30 36 32 38 32 33 35 32 34 30 5a
	//       30 63 SEQUENCE for Subject
	//          31 0b 30 09 06 03 55 04 06 13 02 41 55 31 13
	//          30 11 06 03 55 04 08 13 0a 51 75 65
	//          65 6e 73 6c 6q 6e 64 31 1a 30 18 06
	//          03 55 04 0a 13 11 43 72 79 70 74 53
	//          6f 66 74 20 50 74 79 20 4c 74 64 31
	//          23 30 21 06 03 55 04 03 13 1a 53 65
	//          72 76 65 72 20 74 65 73 74 20 63 65
	//          72 74 20 28 35 31 32 20 62 69 74 29
	//       30 5c SEQUENCE for public key
	//                30 0d 06 09 2a 86 48 86 f7 0d
	//          01 01 01 05 00 03 4b 00 30 48 02 41
	//          00 9f b3 c3 84 27 95 ff 12 31 52 0f
	//          15 ef 46 11 c4 ad 80 e6 36 5b 0f dd
	//          80 d7 61 8d e0 fc 72 45 09 34 fe 55
	//          66 45 43 4c 68 97 6a fe a8 a0 a5 df
	//          5f 78 ff ee d7 64 b8 3f 04 cb 6f ff
	//          2a fe fe b9 ed 02 03 01 00 01
	//       30 0d SEQUENCE for Algorithm ID
	//          06 09 2a 86 48 86 f7 0d 01 01 04 05 00
	//       03 81 81 (129) BIT STRING for Signature
	//            
	struct aosCertificate *cert = 0;
	char buffer[500] = {(char)0x30, (char)0x82, (char)0x01, (char)0xe9, (char)0x30, 
						(char)0x82, (char)0x01, (char)0x52,
 						(char)0x02, (char)0x01, (char)0x04,   // Serial Number
						(char)0x30, (char)0x0d,			// Algorithm Id
						(char)0x06, (char)0x09, (char)0x2a, (char)0x86, (char)0x48, 
						(char)0x86, (char)0xf7, (char)0x0d, (char)0x01, (char)0x01, 
						(char)0x04,	(char)0x05, (char)0x00, (char)0x30, (char)0x5b, 
						(char)0x31, (char)0x0b, (char)0x30, (char)0x09, (char)0x06, 
						(char)0x03, (char)0x55, (char)0x04, (char)0x06, (char)0x13, 
						(char)0x02, (char)0x41, (char)0x55, (char)0x31, (char)0x13, 
						(char)0x30, (char)0x11, (char)0x06, (char)0x03, (char)0x55, 
						(char)0x04, (char)0x08, (char)0x13, (char)0x0a, (char)0x51, 
						(char)0x75, (char)0x65, (char)0x65, (char)0x6e, (char)0x73, 
						(char)0x6c, (char)0x61, (char)0x6e, (char)0x64, (char)0x31, 
						(char)0x1a, (char)0x30, (char)0x18, (char)0x06, (char)0x03, 
						(char)0x55, (char)0x04, (char)0x0a, (char)0x13, (char)0x11,
						(char)0x43, (char)0x72, (char)0x79, (char)0x70, (char)0x74, 
						(char)0x53, (char)0x6F, (char)0x66, (char)0x74, (char)0x20, 
						(char)0x50, (char)0x74, (char)0x79, (char)0x20, (char)0x4c, 
						(char)0x74, (char)0x64, (char)0x31, (char)0x1b, (char)0x30, 
						(char)0x19, (char)0x06, (char)0x03, (char)0x55, (char)0x04, 
						(char)0x03, (char)0x13, (char)0x12,	(char)0x54, (char)0x65, 
						(char)0x73, (char)0x74, (char)0x20, (char)0x43, (char)0x41, 
						(char)0x20, (char)0x28, (char)0x31, (char)0x30, (char)0x32, 
						(char)0x34, (char)0x20, (char)0x62, (char)0x69, (char)0x74,  //21 
						(char)0x29, 
						(char)0x30, (char)0x1e, // Vadility
						(char)0x17, (char)0x0d, // Not before
						(char)0x39, (char)0x38, (char)0x30, (char)0x36, (char)0x32, 
						(char)0x39, (char)0x32, (char)0x33, (char)0x35, (char)0x32, 
						(char)0x34, (char)0x30, (char)0x5a,
						(char)0x17, (char)0x0d, // Not after
						(char)0x30, (char)0x30, (char)0x30, (char)0x36, (char)0x32, 
						(char)0x38, (char)0x32, (char)0x33, (char)0x35, (char)0x32, 
						(char)0x34, (char)0x30, (char)0x5a,
						(char)0x30, (char)0x63,			// SEQUENCE for Subject
						(char)0x31, (char)0x0b, (char)0x30, (char)0x09, (char)0x06, 
						(char)0x03, (char)0x55, (char)0x04, (char)0x06, (char)0x13, 
						(char)0x02, (char)0x41,	(char)0x55, (char)0x31, (char)0x13, 
						(char)0x30, (char)0x11, (char)0x06, (char)0x03, (char)0x55, 
						(char)0x04, (char)0x08, (char)0x13, (char)0x0a,	(char)0x51, 
						(char)0x75, (char)0x65, (char)0x65, (char)0x6e, (char)0x73, 
						(char)0x6c, (char)0x61, (char)0x6e, (char)0x64, (char)0x31, 
						(char)0x1a, (char)0x30, (char)0x18, (char)0x06, (char)0x03, 
						(char)0x55, (char)0x04, (char)0x0a, (char)0x13, (char)0x11, 
						(char)0x43, (char)0x72, (char)0x79, (char)0x70, (char)0x74, 
						(char)0x53, (char)0x6f, (char)0x66, (char)0x74, (char)0x20, 
						(char)0x50, (char)0x74, (char)0x79, (char)0x20, (char)0x4c, 
						(char)0x74, (char)0x64, (char)0x31, (char)0x23, (char)0x30, 
						(char)0x21, (char)0x06, (char)0x03, (char)0x55, (char)0x04, 
						(char)0x03, (char)0x13, (char)0x1a, (char)0x53, (char)0x65, 
						(char)0x72, (char)0x76, (char)0x65, (char)0x72, (char)0x20, 
						(char)0x74, (char)0x65, (char)0x73, (char)0x74, (char)0x20, 
						(char)0x63, (char)0x65, (char)0x72, (char)0x74, (char)0x20, 
						(char)0x28, (char)0x35, (char)0x31, (char)0x32, (char)0x20,  //19 
						(char)0x62, (char)0x69, (char)0x74, (char)0x29,
						(char)0x30, (char)0x5c,		// SEQUENCE for Public Key
						(char)0x30, (char)0x0d, (char)0x06, (char)0x09, (char)0x2a, 
						(char)0x86, (char)0x48, (char)0x86, (char)0xf7, (char)0x0d, 
						(char)0x01, (char)0x01, (char)0x01, (char)0x05, (char)0x00,
						(char)0x03, (char)0x4b, (char)0x00, (char)0x30, (char)0x48, 
						(char)0x02, (char)0x41, (char)0x00, (char)0x9f, (char)0xb3, 
						(char)0xc3, (char)0x84, (char)0x27, (char)0x95, (char)0xff, 
						(char)0x12, (char)0x31, (char)0x52, (char)0x0f, (char)0x15, 
						(char)0xef, (char)0x46, (char)0x11, (char)0xc4, (char)0xad, 
						(char)0x80, (char)0xe6, (char)0x36, (char)0x5b, (char)0x0f, 
						(char)0xdd, (char)0x80, (char)0xd7, (char)0x61, (char)0x8d, 
						(char)0xe0, (char)0xfc, (char)0x72, (char)0x45, (char)0x09, 
						(char)0x34, (char)0xfe, (char)0x55, (char)0x66, (char)0x45, 
						(char)0x43, (char)0x4c, (char)0x68, (char)0x97, (char)0x6a, 
						(char)0xfe, (char)0xa8, (char)0xa0, (char)0xa5, (char)0xdf, 
						(char)0x5f, (char)0x78, (char)0xff, (char)0xee, (char)0xd7, 
						(char)0x64, (char)0xb8, (char)0x3f, (char)0x04, (char)0xcb, 
						(char)0x6f, (char)0xff, (char)0x2a, (char)0xfe, (char)0xfe, 
						(char)0xb9, (char)0xed, (char)0x02, (char)0x03, (char)0x01, // 18
						(char)0x00, (char)0x01,     
						(char)0x30, (char)0x0d,		// For Algorithm ID
						(char)0x06, (char)0x09, (char)0x2a, (char)0x86, (char)0x48,
						(char)0x86, (char)0xf7, (char)0x0d, (char)0x01, (char)0x01, 
						(char)0x04, (char)0x05, (char)0x00,
						(char)0x03, (char)0x81, (char)0x81, // (129) BIT STRING for Signature
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,

						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,

						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04, (char)0x05,
						(char)0x01, (char)0x02, (char)0x03, (char)0x04,
	};  
	unsigned int buflen = 500;
       
/*	int ret = aosCertificate_decode(buffer, buflen, &cert);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(cert != 0)) << endtc;
	if (cert == 0)
	{
		return false;
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(cert->mVersion)) << endtc;
	OmnTC(OmnExpected<int>(4), OmnActual<int>(cert->mSerialNum)) << endtc;
*/
	return true;
}

int f(int v1, int v2) {cout << "Value: " << v1 << ":" << v2 << " called" << endl; return 0;}

bool 
OmnCertificateTester::temp()
{
	OmnFuncTester<int, int, int, 41> tt(this, __FILE__, __LINE__);
  
	int values[5] = {1, 2, 3, 4, 5}; 
	int values2[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
 
	OmnEnumInt v1(values, 5, 0);
	OmnEnumInt v2(values2, 10, 0);

	tt.setValue(&v1, &v2);
	tt.setFunc(f);
	tt.setReturnValue(0);

	tt.start();

	tt.printInput();

	tt.restart();

	tt.printInput();

	return true;
}