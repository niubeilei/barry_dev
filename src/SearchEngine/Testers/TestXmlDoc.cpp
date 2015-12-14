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
#include "SearchEngine/Testers/TestXmlDoc.h"

#include "alarm_c/alarm.h"
#include "DataStore/Ptrs.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Util/StrSplit.h"

#if 0

bool sgInitFlag = false;

const int  sgWordLenWeightSize = 100;
static int  sgWordLenWeights[sgWordLenWeightSize];

const int  sgAvLenWeightSize = 100;
static int  sgAvLenWeights[sgAvLenWeightSize];

const int  sgNumSubtagWeightSize = 100;
static int  sgNumSubtagWeights[sgNumSubtagWeightSize];

// Setup for Text Words
static char sgTextWordMap[128];
const int  sgTextWordMapMin = 33;
const int  sgTextWordMapMax = 126;

// Setup for CDATAt Words
static char sgCdataWordMap[128];
const int  sgCdataWordMapMin = 33;
const int  sgCdataWordMapMax = 126;

// Set up for Attribute Names
static char sgNameMap[128];
const int  sgNameMapMin = 33;
const int  sgNameMapMax = 126;

const int  sgNumCdataExcludes = 2;
static char *sgCdataExcludes[sgNumCdataExcludes];

// Common Words
static const int sgNumCws = 50000;
static const int sgMaxCwLen = 30;
static char sgCws[sgNumCws][sgMaxCwLen];

void
AosTestXmlDoc::init()
{
	// Set the text word map
	memset(sgTextWordMap, 1, 128);
	memset(&sgTextWordMap[0], 0, 32);
	sgTextWordMap[(int)'<'] = 0;
	sgTextWordMap[(int)'>'] = 0;
	sgTextWordMap[(int)'"'] = 0;
	sgTextWordMap[(int)'\''] = 0;
	sgTextWordMap[(int)'\\'] = 0;

	// Set the cdata word map
	sgCdataExcludes[0] = OmnNew char[10];
	sgCdataExcludes[1] = OmnNew char[10];
	strcpy(sgCdataExcludes[0], "<![CDATA[");
	strcpy(sgCdataExcludes[1], "]]>");
	memset(sgCdataWordMap, 1, 128);
	memset(&sgCdataWordMap[0], 0, 32);

	// Set the name map
	memset(sgNameMap, 1, 128);
	memset(&sgNameMap[0], 0, 32);
	sgNameMap[(int)'!'] = 0;
	sgNameMap[(int)'"'] = 0;
	sgNameMap[(int)'%'] = 0;
	sgNameMap[(int)'&'] = 0;
	sgNameMap[(int)'('] = 0;
	sgNameMap[(int)')'] = 0;
	sgNameMap[(int)'*'] = 0;
	sgNameMap[(int)'+'] = 0;
	sgNameMap[(int)','] = 0;
	sgNameMap[(int)'/'] = 0;
	sgNameMap[(int)'<'] = 0;
	sgNameMap[(int)'>'] = 0;
	sgNameMap[(int)'='] = 0;
	sgNameMap[(int)';'] = 0;
	sgNameMap[96] = 0;		// `
	sgNameMap[(int)'\''] = 0;
	sgNameMap[(int)'\\'] = 0;
	
	// Set the text word length weight
	//	Length	Weight
	//	5		75	
	//	10		15
	//	20		4
	//	100		3
	//	500		2
	//	2000	1
	const int numSegs = 6;
	const int wLens[numSegs] = 	{5,  10, 15, 20, 25, 28};
	const int wRanges[numSegs] = 	{75, 15, 4,  3,  2,  1};
	int start = 0;
	for (int k=0; k<numSegs; k++)
	{
		for (int i=start; i<start+wRanges[k]; i++) 
			sgWordLenWeights[i] = wLens[k];
		start += wRanges[k];
	}

	// Set the attribute value word length weight
	//	Length	Weight
	//	5		75	
	//	10		15
	//	15		4
	//	20		3
	//	25		2
	//	30		1
	const int vLens[numSegs] = 	{5,  10, 15, 20, 25, 30};
	const int vRanges[numSegs] = 	{75, 15, 4,  3,  2,  1};
	start = 0;
	for (int k=0; k<numSegs; k++)
	{
		for (int i=start; i<start+vRanges[k]; i++) 
			sgAvLenWeights[i] = vLens[k];
		start += vRanges[k];
	}

	// Set the number of subtag weights
	//	Length	Weight
	//	2		75	
	//	5		15
	//	8		4
	//	10		3
	//	15		2
	//	20		1
	const int sLens[numSegs] = 		{2,  5,  8,  10, 15, 20};
	const int sRanges[numSegs] = 	{75, 15, 4,  3,  2,  1};
	start = 0;
	for (int k=0; k<numSegs; k++)
	{
		for (int i=start; i<start+sRanges[k]; i++) 
			sgNumSubtagWeights[i] = sLens[k];
		start += sRanges[k];
	}

	// Create the common words
	OmnString str;
	for (int i=0; i<sgNumCws; i++)
	{
		str = AosNextWord(sgNameMap, 
				sgNameMapMin, sgNameMapMax, 
				0, 0, 
				sgWordLenWeights, 
				sgWordLenWeightSize);
		aos_assert(str.length() < sgMaxCwLen);
		strcpy(sgCws[i], str.data());
	}
}

static int sgNumXmldocCreated = 0;

AosTestXmlDoc::AosTestXmlDoc(const int depth)
:
mNumSubtags(0),
mNumAttrs(0)
{
	// mSubtags.setNullValue(0);
	sgNumXmldocCreated++;
	if (!sgInitFlag) init();
	sgInitFlag = true;

	// It creates a node with numChildren subtags and maximum depth.
	// Generate the tag name.
	mTagname = sgCws[rand()%sgNumCws];

	generateAttrs();

	int vv = rand() % 2;
	if (vv == 0)
	{
		generateTexts();
		generateSubtags(depth);
	}
	else
	{
		generateSubtags(depth);
		generateTexts();
	}
}


AosTestXmlDoc::~AosTestXmlDoc()
{
	sgNumXmldocCreated--;
	for (int i=0; i<100; i++)
	{
		mSubtags[i] = 0;
	}
}


void
AosTestXmlDoc::generateAttrs()
{
	// It generates a randum number of attributes. Both the attribute
	// names and values are randomly determined.
	// It will collect attribute value words into mAttrWords.
	//
	// Determine whethere there shall be attributes
	mAttrWords.clear();
	int vv = rand() % 100;
	if (vv < 10) 
	{
		// No attributes
		return;
	}

	// Determine how many attributes to generate
	int size;
	if (vv < 13) size = 100;
	else if (vv < 18) size = 50;
	else if (vv < 30) size = 15;
	else size = 10;

	mNumAttrs = rand() % size;
	if (mNumAttrs > eMaxAttrs) mNumAttrs = eMaxAttrs;

	// Generate attributes
	OmnString word;
	for (int i=0; i<mNumAttrs; i++)
	{
		Attr attr;
		int vv = rand();
		int vv1 = vv%100;
		if (vv1 < 90)
		{
			attr.name = sgCws[vv%sgNumCws];
		}
		else
		{
			// Generate attribute name. Note that we do not 
			// ensure attribute name uniqueness. 
			attr.name = AosNextWord(sgNameMap, 
				sgNameMapMin, sgNameMapMax, 
				0, 0, 
				sgWordLenWeights, 
				sgWordLenWeightSize);
		}

		// Generate the value
		int idx = rand() % sgAvLenWeightSize;
		int range = sgAvLenWeights[idx];
		int numWords = rand() % range;
		attr.value = "";
		for (int k=0; k<numWords; k++)
		{
			if (vv1 < 70)
			{
				word = sgCws[vv%sgNumCws];
			}
			else
			{
				word = AosNextWord(
					sgTextWordMap, sgTextWordMapMin, sgTextWordMapMax, 
					0, 0, 
					sgWordLenWeights, sgWordLenWeightSize);
			}

			if (k != 0) attr.value << " ";
			attr.value << word;
			mAttrWords.append(word);
		}
		mAttrs[i] = attr;
	}
}


void
AosTestXmlDoc::generateTexts()
{
	int numWords = rand() % eTextMaxWords;
	OmnString word;
	int vv = rand();
	if ((vv % 2) == 0) 
	{
		// Generate cdata
		mTexts = "<![CDATA[";
		for (int i=0; i<numWords; i++)
		{
			if ((vv % 100) < 60)
			{
				word = sgCws[sgNumCws];
			}
			else
			{
				word = AosNextWord(sgTextWordMap, 
							sgTextWordMapMin, sgTextWordMapMax, 
							(const char **)sgCdataExcludes, 
							sgNumCdataExcludes,
							sgWordLenWeights, 
							sgWordLenWeightSize);
			}
			mTextWords.append(word);
			if (i != 0) mTexts << " ";
			mTextWords.append(word);
			mTexts << word;
		}
		mTexts << "]]>";
	}
	else
	{
		// Generate texts
		mTexts = "";
		for (int i=0; i<numWords; i++)
		{
			if ((vv % 100) < 60)
			{
				word = sgCws[sgNumCws];
			}
			else
			{
				word = AosNextWord(sgTextWordMap, 
						sgTextWordMapMin, sgTextWordMapMax, 0, 0, 
						sgWordLenWeights, 
						sgWordLenWeightSize);
			}
			if (i != 0) mTexts << " ";
			mTexts << word;
			mTextWords.append(word);
		}
	}
}


void
AosTestXmlDoc::generateSubtags(const int depth)
{
	// It generates a number of subtags with the depth 'depth'. 
	// If 'depth' == 0, it will not generate subtags.
	if (depth == 0) 
	{
		mNumSubtags = 0;
		return;
	}
	int idx = rand() % sgNumSubtagWeightSize;
	int range = sgNumSubtagWeights[idx];
	mNumSubtags = (rand() % range) + 1;
	if (mNumSubtags > eMaxSubtags) mNumSubtags = eMaxSubtags;
	for (int i=0; i<mNumSubtags; i++)
	{
		mSubtags[i] = OmnNew AosTestXmlDoc(depth-1);	
	}
}


void
AosTestXmlDoc::composeData(OmnString &data)
{
	data << "<" << mTagname;
	if (mNumAttrs > 0)
	{
		// There are attributes
		data << " ";
		for (int i=0; i<mNumAttrs; i++)
		{
			data << " " << mAttrs[i].name << "=\"" << mAttrs[i].value << "\"";
		}
	}

	// Check whether there are subtags. 
	if (mNumSubtags == 0)
	{
		// Check whether there are texts
		if (mTexts.length() == 0)
		{
			// This is a tag with the form:
			// <tagname/> or <tagname></tagname>
			if (rand() % 2)
			{
				// Create the form "<tagname/>"
				data << "/>";
				return;
			}
			// Create the form: "<tagname></tagname>"
			data << "></" << mTagname << ">";
			return;
		}

		// It is in the form:
		//	<tagname>xxx</tagname>
		data << ">" << mTexts << "</" << mTagname << ">";
		return;
	}

	// It is in the form:
	//	<tagname> xxx <subtag ...>...</tagname> or
	//	<tagname><subtag ...>...</tagname>
	if (rand() % 2)
	{
		// Make it the form:
		//	<tagname> xxx <subtag ...>...</tagname> or
		data << ">" << mTexts;
		for (int i=0; i<mNumSubtags; i++)
			mSubtags[i]->composeData(data);
		data << "</" << mTagname << ">";
	}	
	else
	{
		// Make it the form:
		//	<tagname><subtag ...>...</tagname>
		data << ">";
		for (int i=0; i<mNumSubtags; i++)
			mSubtags[i]->composeData(data);
		data << mTexts << "</" << mTagname << ">";
	}
}


int 
AosTestXmlDoc::getAllWords(
		OmnDynArray<OmnString, eAW1InitSize, eAW1IncSize, eAW1MaxSize> &words)
{
	// All words include, recursively:
	// 1. Its tag name
	// 2. All its attribute names
	// 3. All attribute words
	// 4. All text words
	words.append(mTagname);
	for (int i=0; i<mNumAttrs; i++)
		words.append(mAttrs[i].name);

	for (int i=0; i<mAttrWords.entries(); i++)
	{
		words.append(mAttrWords[i]);
	}

	for (int i=0; i<mTextWords.entries(); i++)
	{
		words.append(mTextWords[i]);
	}

	for (int i=0; i<mNumSubtags; i++)
	{
		mSubtags[i]->getAllWords(words);
	}
	return words.entries();
}


bool
AosTestXmlDoc::saveAttrs(const u64 &docid)
{
	OmnString stmt;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnString words[eMaxAttrWords];
	int numWords;
	for (int i=0; i<mNumAttrs; i++)
	{
		stmt = "insert into torturer_attrs (name, value, docid) values ('";
		stmt << mAttrs[i].name << "', '"
			<< mAttrs[i].value << "', "
			<< docid;
		aos_assert_r(store->runSQL(stmt), false);

		aos_assert_r(AosStrSplit::getWords(mAttrs[i].value, words, numWords, 
				eMaxAttrWords, " "), false);
		for (int k=0; k<numWords; k++)
		{
			stmt = "insert into torturer_attrwords (name, word, docid) values ('";
			stmt << mAttrs[i].name << "', "
				<< words[k] << "', " << docid;
			aos_assert_r(store->runSQL(stmt), false);
		}
	}

	for (int i=0; i<mNumSubtags; i++)
	{
		aos_assert_r(saveAttrs(docid), false);
	}
	return true;
}

#endif
