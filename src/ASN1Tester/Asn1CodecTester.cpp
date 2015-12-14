////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Asn1CodecTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Asn1Tester/Asn1CodecTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "ASN1/AsnCodec.h"
#include "ASN1/ObjId.h"


OmnAsn1CodecTester::OmnAsn1CodecTester()
{
	mName = "OmnAsn1CodecTester";
}


OmnAsn1CodecTester::~OmnAsn1CodecTester()
{
}
 
 
bool 
OmnAsn1CodecTester::start()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Asn1 Tester";

	objIdCodecTester();
	printableStrCodecTester();
	integerCodecTester();
	bitstringCodecTester();
	return true;
}

  
bool
OmnAsn1CodecTester::objIdCodecTester()
{
	// {1 2 840 113549} ==> 06 06 2a 86 48 86 f7 0d

	unsigned int id1[100] = {1, 2, 840, 113549};
	char expected1[100] = {	(char)0x06, (char)0x06, 
							(char)0x2a, (char)0x86, 
							(char)0x48, (char)0x86, 
							(char)0xf7, (char)0x0d};
//	bool rslt;

//	rslt = checkOneObjId(6, eAsnTypeClass_Universal, 1, id1, 4, expected1, 8);
//	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	unsigned int id2[100] = {2, 5, 4, 6};
	char expected2[100] = { (char)0x06, (char)0x03, 
							(char)0x55, (char)0x04, 
							(char)0x06};
//	rslt = checkOneObjId(6, eAsnTypeClass_Universal, 1, id2, 4, expected2, 5);
//	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	return true;
}


bool
OmnAsn1CodecTester::checkOneObjId(unsigned int id, 
								  unsigned char theclass, 
								  char isPrimitive,
								  struct aosAsnObjId *objid, 
								  char *expected, 
								  int expLen)
{
	unsigned int cursor = 0;
	char buffer[1000];
	int ret;
  
	ret = aosAsn1Codec_objId_encode(id, theclass, isPrimitive, 
		objid, buffer, 1000, &cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(expLen), OmnActual<int>(cursor)) << endtc;
	ret = strncmp(buffer, expected, cursor-1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;

}
 

bool
OmnAsn1CodecTester::printableStrCodecTester()
{
	char str1[100];
	strcpy(str1, "Test User 1");
	char expected1[100] = {	(char)0x13, (char)0x0b, 
							(char)0x54, (char)0x65, 
							(char)0x73, (char)0x74, 
							(char)0x20, (char)0x55, 
							(char)0x73, (char)0x65,
							(char)0x72, (char)0x20, 
							(char)0x31};

	bool rslt = checkOnePrintableStr(19, eAsnTypeClass_Universal, 1, 
		str1, expected1, 13);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) 
		<< "Original string: Test User 1" << endtc;

	return true;
}


bool
OmnAsn1CodecTester::checkOnePrintableStr(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  char *str, 
						  char *expected, 
						  int expLen)
{
	unsigned int cursor = 0;
	char buffer[1000];
	int ret;
  
	ret = aosAsn1Codec_string_encode(id, theclass, isPrimitive, 
		str, strlen(str), buffer, 1000, &cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(expLen), OmnActual<int>(cursor)) << endtc;
	ret = strncmp(buffer, expected, cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}


bool
OmnAsn1CodecTester::integerCodecTester()
{
	bool rslt;
	char expected1[100] = {(char)0x02, (char)0x01, (char)0x00};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 0, expected1, 3);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	char expected2[100] = {(char)0x02, (char)0x01, (char)0x7f};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 127, expected2, 3);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	char expected3[100] = {(char)0x02, (char)0x02, (char)0x00, (char)0x80};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 128, expected3, 4);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;
 
	char expected4[100] = {(char)0x02, (char)0x02, (char)0x01, (char)0x00};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 256, expected4, 4);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	char expected41[100] = {(char)0x02, (char)0x02, (char)0x7f, (char)0xff};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 32767, expected41, 4);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	char expected42[100] = {(char)0x02, (char)0x03, (char)0x00, (char)0x80, (char)0x00};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, 32768, expected42, 5);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;
 
	char expected5[100] = {(char)0x02, (char)0x01, (char)0x80};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, -128, expected5, 3);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

 	char expected6[100] = {(char)0x02, (char)0x02, (char)0xff, (char)0x7f};
	rslt = checkOneInteger(2, eAsnTypeClass_Universal, 1, -129, expected6, 4);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << endtc;

	return true;
}


bool
OmnAsn1CodecTester::checkOneInteger(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  int value, 
						  char *expected, 
						  int expLen)
{
	unsigned int cursor = 0;
	char buffer[1000];
	int ret;
       
  	ret = aosAsn1Codec_integer_encode(id, theclass, isPrimitive, 
		value, buffer, 1000, &cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(expLen), OmnActual<int>(cursor)) << endtc;
	ret = strncmp(buffer, expected, cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}
	

bool
OmnAsn1CodecTester::bitstringCodecTester()
{
	// "01101110 0101 110111"
	char bitstr1[20] = {(char)0x6e, (char)0x5d, (char)0xc0};
	char expected1[20] = {(char)0x03, (char)0x04, (char)0x06, 
						  (char)0x6e, (char)0x5d, (char)0xc0};
	bool rslt;

	rslt = checkBitstring(3, eAsnTypeClass_Universal, 1, bitstr1, 18, expected1, 6);
	return true;
}


bool
OmnAsn1CodecTester::checkBitstring(unsigned int id, 
						  unsigned char theclass, 
						  char isPrimitive,
						  char *bitstr, 
						  unsigned int numbits,
						  char *expected, 
						  int expLen)
{
	unsigned int cursor = 0;
	char buffer[1000];
	int ret;
        
   	ret = aosAsn1Codec_bitstring_encode(id, theclass, isPrimitive, 
		bitstr, numbits, buffer, 1000, &cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	OmnTC(OmnExpected<int>(expLen), OmnActual<int>(cursor)) << endtc;
	ret = strncmp(buffer, expected, cursor);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}
	


