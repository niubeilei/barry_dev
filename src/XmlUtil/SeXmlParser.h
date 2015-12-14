////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: XmlParser.h
//
// Modification History:
// 09/23/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_SeXmlParser_h
#define Aos_XmlUtil_SeXmlParser_h

#include "aosUtil/Types.h"
#include "Debug/Rslt.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/StrSplit.h"
#include "Util/MemoryChecker.h"
#include "UtilComm/Ptrs.h"
#include "XmlParser/Ptrs.h"
#include "XmlUtil/XmlTag.h"

#include <list>

class AosXmlParser : public OmnRCObject, public AosMemoryCheckerObj
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eLookForOpenTag,
		eProcTagAttrs,
		eProcTagBody
	};

	enum
	{
		eMaxTagNameLen = 512,
		eMaxMetaTagLevels = 5,
		eMaxTagLevels = 1000,
		eMaxTagsInPath = 5 
	};

private:
	OmnConnBuffPtr	mBuff;
	AosXmlTagPtr 	mRoot;
	AosXmlTagPtr 	mCrtTag;
	bool			mExpectValue;
	bool			mIsAttr;
	OmnString		mAttrname;
	AosStrSplit		mPath;
	int				mCrtPathIdx;
	int				mPathLen;

	// Chen Ding, 2011/04/07
	bool			mPartialParsing;

public:
	AosXmlParser();
	~AosXmlParser();

	AosXmlTagPtr	
			parse(const OmnConnBuffPtr &buff,
				const OmnString &expected AosMemoryCheckDecl); 

	AosXmlTagPtr	
			parse(const OmnString &data,
				const OmnString &expected AosMemoryCheckDecl); 

	AosXmlTagPtr	
			parse(const char *data,
				const int len,
				const OmnString &expected AosMemoryCheckDecl); 

	// Chen Ding, 05/14/2011, moved to be private
	static AosXmlTagPtr parse(const OmnString &str AosMemoryCheckDecl);
	static AosXmlTagPtr parse(const char *str, const int len AosMemoryCheckDecl);
	static AosXmlTagPtr parseStatic(const OmnConnBuffPtr &buff AosMemoryCheckDecl);

	// Chen Ding, 2011/04/07
	AosXmlTagPtr parse(const OmnString &src, const int start AosMemoryCheckDecl)
	{
		// This function parses an XML starting from 'start', 
		// and ends by its opening tag. 
		const char *data = src.data();
		aos_assert_r(start < src.length(), NULL);
		mPartialParsing = true;
		aos_assert_r(data[start] == '<', NULL);
		return parse(&data[start], src.length() - start, "" AosMemoryCheckerFileLine);
	}

	bool 	procOpenTag(
        		int &idx,
        		Status &status);
	bool 	procClosingTag(
        		int &idx, 
				const bool flag); 
	bool 	procTagBody(
        		int &idx, 
				Status &state);
	bool 	procTagText(
        		int &idx);
	bool 	procCdata(
        		int &idx);
	bool	procBdata(
				int &idx); 
	bool 	procTagAttrs(
        		int &idx,
        		Status &status);
	bool	addTag(
				const int tagStartIdx,
				const int namestartIdx, 
				const int nameendIdx);
	bool 	prepareData();
	void	init();
	bool 	popTag(
				const int tagEndIdx,
				const int tagnamestart, 
				const int tagnameend,
				const bool flag);
	bool	procExpected(const OmnString &expected);
	AosXmlTagPtr	
			getTagObj(const AosXmlTagPtr &parent,
				const int tagStartIdx,
				const int nameStart,
				const int nameEnd);
	u8 * 	checkAttrValue(
				u8 *buffData, 
				const int avstart, 
				int &avlen);
	bool 	getClosingTag(
				const char *data, 
				const int idx, 
				const int len,
				const OmnString &tagname);
	bool 	getOpeningTag(
				const char *data, 
				const int idx, 
				OmnString &tagname);

private:
	AosXmlTagPtr parsePrivate(const OmnString &expected AosMemoryCheckDecl); 
};
#endif
