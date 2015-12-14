////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 02/29/2012 Created by Chen Ding
// 2015/05/28 Modified by Chen Ding, added a few more doc types
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_DocTypes_h
#define Aos_XmlUtil_DocTypes_h

#define AOSDOCTYPE_XML                  "xml"

class AosDocType
{
public:
	enum
	{
		eDocTypeBitsToShift = 56		// The highest byte 
	};

	enum E
	{
       	eNormalDoc = 0,

    	eAdminDoc,
    	eAccessDoc,
    	eLogDoc,
    	eCounterDoc,
		eTempDoc,
		eGroupedDoc,
		eBinaryDoc,
		eIIL,
		eXmlDoc,				// Chen Ding, 2015/05/28, #3-1-94
		eCSVGroupDoc,			// Chen Ding, 2015/05/28, #3-1-94
		eFixLenGroupDoc,		// Chen Ding, 2015/05/28, #3-1-94
		eCSVDoc,				// Chen Ding, 2015/05/28, #3-1-94

		eMax
	};

	static E getDocType(const u64 &docid)
	{
		u8 d = (docid >> eDocTypeBitsToShift);
		if (d < eMax) return (E)d;
		return eMax;
	}

	static bool isValid(const E code)
	{
		return code < eMax;
	}

	static inline void setDocidType(AosDocType::E type, u64 &docid)
	{
		if (AosDocType::isValid(type))
		{
			u8 * typemask = (u8*)&docid;
			typemask[7] = (u8)type;
		}
		else
			docid = 0;
	}

	static inline bool isTempDoc(const u64 &docid)
	{
		return ((docid >> eDocTypeBitsToShift) == eTempDoc);
	}
};

#endif
