////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 09/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_SeXmlTypes_h
#define Aos_XmlUtil_SeXmlTypes_h

// Chen Ding, 03/19/2012, Moved from XmlTag.h
#define AOSXMLMARK_NOINDEX			"noindex"
#define AOSXMLMARK_ATTRWORD			"attrwd"
#define AOSXMLMARK_GBLOBAL_INDEX	"globalidx"
#define AOSXMLMARK_CONTAINER_INDEX	"ctnridx"
#define AOSXMLMARK_BOTH_INDEX		"bothidx"
#define AOSXMLMARK_CTNR_ONLY		"ctnronly"

class AosEntryMark
{
public:
	enum E
	{
 		eNoIndex = 0, 			// 0b0000
 		eAttrWordIndex = 1, 	// 0b0001
 		eGlobalIndex = 2, 		// 0b0010
 		eContainerIndex = 4, 	// 0b0100
 		eBothIndex = 6, 		// 0b0110
 		eCtnrOnly = 8, 			// 0b1000
	};

	static E toEnum(const OmnString &name, const E dft)
	{
		if (name == AOSXMLMARK_NOINDEX) 		return eNoIndex;
		if (name == AOSXMLMARK_ATTRWORD)		return eAttrWordIndex;
		if (name == AOSXMLMARK_GBLOBAL_INDEX)	return eGlobalIndex;
		if (name == AOSXMLMARK_CONTAINER_INDEX)	return eContainerIndex;
		if (name == AOSXMLMARK_BOTH_INDEX)		return eBothIndex;
		if (name == AOSXMLMARK_CTNR_ONLY)		return eCtnrOnly;
		return dft;
	}
};

class AosDocSource
{
public:
	enum E
	{
		eInvalid,

		eUnknown,
		eLocalCache,
		eFromServer,
		eFromClient,

		eMax
	};

	static bool isValid(const E code)
	{
		return (code > eInvalid && code < eMax);
	}
};
#endif

