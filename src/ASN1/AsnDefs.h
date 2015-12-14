////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AsnDefs.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_ASN1_AsnDefs_h
#define Omn_ASN1_AsnDefs_h

enum
{
	eAsnMaxObjComps = 20,
	eMaxObjectIdLength = 100, 
	eAsnTypeClass_Universal = 0x00,
	eAsnTypeClass_App		= 0x40,
	eAsnTypeClass_Context	= 0x80,
	eAsnTypeClass_Private	= 0xC0,

	eAosAsn_TagId_Reserved = 		0x00,
	eAosAsn_TagId_Boolean =			0x01,
	eAosAsn_TagId_Integer =			0x02,
	eAosAsn_TagId_BitString =		0x03, 
	eAosAsn_TagId_OctetString =		0x04, 
	eAosAsn_TagId_Null =			0x05, 
	eAosAsn_TagId_ObjectId =		0x06, 
	eAosAsn_TagId_Object_Descriptor,
	eAosAsn_TagId_External,
	eAosAsn_TagId_Real,
	eAosAsn_TagId_Enumerated,		//10
	eAosAsn_TagId_Embedded_Pdv,
	eAosAsn_TagId_String_Utf8,
	eAosAsn_TagId_13,
	eAosAsn_TagId_14,
	eAosAsn_TagId_15,			//15
	eAosAsn_TagId_Sequence =		0x10, 
	eAosAsn_TagId_Set =				0x11, 
	eAosAsn_TagId_String_Numeric =	0x12,
	eAosAsn_TagId_PrintableString =	0x13, 
	eAosAsn_TagId_T61String =		0x14, 
	eAosAsn_TagId_String_Videotex = 0x15,
	eAosAsn_TagId_IA5String =		0x16, 
	eAosAsn_TagId_UTCTime =			0x17, 
	eAosAsn_TagId_Time_Generalized =0x18,
	eAosAsn_TagId_String_Graphic =  0x19,		//25
	eAosAsn_TagId_String_Iso646 =	0x1a,
	eAosAsn_TagId_String_General = 	0x1b,
	eAosAsn_TagId_String_Universal =0x1c,
	eAosAsn_TagId_29 =				0x1d,
	eAosAsn_TagId_String_Bmp = 		0x1e,		//30
	eAosAsn_TagId_Last =			0x1f,		//31


};


#endif

