////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Asn1CodecTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Asn1Tester_Asn1Tester_h
#define Omn_Asn1Tester_Asn1Tester_h

#include "Tester/TestPkg.h"

struct aosAsnObjId;

class OmnTestMgr;

class OmnAsn1CodecTester : public OmnTestPkg
{
public:
	OmnAsn1CodecTester();
	~OmnAsn1CodecTester();

	virtual bool		start();

private:
	bool	checkOneObjId(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  struct aosAsnObjId *objid, 
						  char *expected, 
						  int expLen);
	bool	checkOnePrintableStr(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  char *str, 
						  char *expected, 
						  int expLen);
	bool	checkOneInteger(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  int value, 
						  char *expected, 
						  int expLen);
	bool	checkBitstring(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  char *bitstr, 
						  unsigned int numbits,
						  char *expected, 
						  int expLen);

	bool	objIdCodecTester();
	bool	printableStrCodecTester();
	bool	integerCodecTester();
	bool	bitstringCodecTester();
};
#endif

