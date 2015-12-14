////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CertificateTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Asn1Tester_CertificateTester_h
#define Omn_Asn1Tester_CertificateTester_h

#include "Tester/TestPkg.h"


class OmnTestMgr;

class OmnCertificateTester : public OmnTestPkg
{
public:
	OmnCertificateTester();
	~OmnCertificateTester();

	virtual bool		start();

private:
	bool	decodeTester();
	bool	temp();
};
#endif

