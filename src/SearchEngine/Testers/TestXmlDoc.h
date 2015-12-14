////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2009/10/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SearchEngine_Testers_TestXmlDoc_h
#define Aos_SearchEngine_Testers_TestXmlDoc_h

#include "SearchEngine/Testers/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "Util/String.h"


class AosTestXmlDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eTextMaxWords = 20,
		eArrayInitSize = 10,
		eArrayIncSize = 10,
		eArrayMaxSize = 1000,
		eAWInitSize = 20,
		eAWIncSize = 10,
		eAWMaxSize = 10000,
		eAW1InitSize = 5000,
		eAW1IncSize = 1000,
		eAW1MaxSize = 20000,
		eMaxSubtags = 100,
		eMaxAttrs = 300,
		eMaxAttrWords = 100
	};

	struct Attr
	{
		OmnString name;
		OmnString value;
	};

	OmnString			mTagname;
	OmnString			mTexts;
	AosTestXmlDocPtr	mSubtags[eMaxSubtags];
	int					mNumSubtags;
	Attr				mAttrs[eMaxAttrs];
	int					mNumAttrs;

	OmnDynArray<OmnString, eArrayInitSize, eArrayIncSize, eArrayMaxSize> mTagnames;
	OmnDynArray<OmnString, eAWInitSize, eAWIncSize, eAWMaxSize>			mAttrWords;
	OmnDynArray<OmnString, eAWInitSize, eAWIncSize, eAWMaxSize>			mTextWords;
	OmnDynArray<OmnString, eAWInitSize, eAWIncSize, eAWMaxSize>			mAttrnameWords;
	OmnDynArray<OmnString, eAWInitSize, eAWIncSize, eAWMaxSize>			mAttrvalueWords;

public:
	AosTestXmlDoc(const int depth);
	~AosTestXmlDoc();

	void 	init();
	void 	generateTagName();
	void 	generateAttrs();
	void 	generateTexts();
	void 	generateSubtags(const int depth);
	void	composeData(OmnString &data);
	int 	getAllWords(OmnDynArray<OmnString, eAW1InitSize, eAW1IncSize, eAW1MaxSize> &words);
	bool	saveAttrs(const u64 &docid);
};
#endif
