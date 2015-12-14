////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OcspCrlStatus.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_UnitTest_OcspCrlTester_OcspCrlStatus_h
#define Omn_UnitTest_OcspCrlTester_OcspCrlStatus_h
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosOcspCrlStatus:public OmnRCObject 
{
	OmnDefineRCObject;
public:    
	bool    statusOcsp; //ocsp sw. on/off
    bool    statusCrl; //crl sw. on/off
    bool    statusClientAut; //clientauth sw on/off
    bool    statusCert; // cert invalid/valid
    bool    statusConnOcspSer; // conn to ocsp on/offline
    bool    responseOcspSer; // whether to response
//    bool    statusConnCrl; // conn to crl on/offline
//    bool    responseCrlSer; // whether to response
public:
	AosOcspCrlStatus();
	virtual ~AosOcspCrlStatus();
};

#endif
