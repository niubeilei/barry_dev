////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertMgrTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "PKCS/Tester/CertMgrTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "PKCS/CertMgr.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"


bool AosCertMgrTester::start()
{
	// 
	// Test default constructor
	//
	certDecodeTest();
	return true;
}


bool AosCertMgrTester::certDecodeTest()
{
	OmnBeginTest << "Test Certificate Decoding";
	mTcNameRoot = "Test_Cert_Decode";
	OmnString filename = "../../../Data/Certificates/Certificate01.txt";
	OmnFile file(filename, OmnFile::eReadOnly);
	bool rslt;
	int ret;
	X509_CERT_INFO *certificate;

	rslt = file.isGood();
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt))
		<< "Failed to open file: " << filename << endtc;

	if (!rslt) return false;

	rslt = file.skipTo("-----BEGIN CERTIFICATE-----", false);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt))
		<< "Certificate file incorrect" << endtc;

	OmnString base64;
	rslt = file.readToString(base64);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt))
		<< "Failed to read file" << endtc;
	
	char errmsg[1000];
	ret = AosCertMgr_decodeBase64((char *)base64.data(), &certificate, errmsg);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
			
	return true;
}	

