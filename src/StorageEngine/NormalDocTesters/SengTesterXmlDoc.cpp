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
// 	There is a table that stores all the attributes:
// 		<docid> <attrname> <attrvalue>
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/NormalDocTesters/SengTesterXmlDoc.h"

#include "SEUtil/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/SeXmlParser.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/UtUtil.h"
#include "Util/StrSplit.h"
#include "Util/StrParser.h"



OmnString	AosTestXmlDoc::mAttrTname1;
OmnString	AosTestXmlDoc::mAttrwdTname1;
OmnString	AosTestXmlDoc::mDocwdTname1;
OmnString	AosTestXmlDoc::mTagTname1;
OmnString	AosTestXmlDoc::mDocTname1;

static OmnMutex sgLock;
static int sgNumAttrs = 0;
static u32 sgObjid = 100;

bool sgInitFlag = false;

const int  sgWordLenWeightSize = 60;
//const int  sgWordLenWeightSize = 100;
static int  sgWordLenWeights[sgWordLenWeightSize];

const int  sgAvLenWeightSize = 100;
//const int  sgAvLenWeightSize = 60;
static int  sgAvLenWeights[sgAvLenWeightSize];

//const int  sgNumSubtagWeightSize = 60;
const int  sgNumSubtagWeightSize = 100;
static int  sgNumSubtagWeights[sgNumSubtagWeightSize];

// Setup for Text Words
static char sgTextWordMap[128];
const int  sgTextWordMapMin = 33;
//const int  sgTextWordMapMax = 126;
const int  sgTextWordMapMax = 75;

// Set up for Attribute Names
static char sgNameMap[128];
const int  sgNameMapMin = 33;
//const int  sgNameMapMax = 126;
const int sgNameMapMax = 75;

// Common Words
static const u32 sgNumCws = 50000;
static const int sgMaxCwLen = 30;
static char sgCws[sgNumCws][sgMaxCwLen];

const int MAX_STDANAMES = 100;
static OmnString sgSdtAnames[MAX_STDANAMES];
static int sgNumStdAnames = 0;

// Common Tag Names
const int MAX_COMMON_TAGNAMES = 100;
static OmnString sgCommTagnames[MAX_COMMON_TAGNAMES];

const int MAX_SITEID = 3;
const int SITEID_START = 100;

void
AosTestXmlDoc::initPriv()
{
	// Set the text word map
	memset(sgTextWordMap, 1, 128);
	memset(&sgTextWordMap[0], 0, 32);
	sgTextWordMap[(int)'<'] = 0;
	sgTextWordMap[(int)'>'] = 0;
	sgTextWordMap[(int)'"'] = 0;
	
	//Linda,2011/02/11
	sgTextWordMap[(int)'['] = 0;
	sgTextWordMap[(int)']'] = 0;
	sgTextWordMap[(int)'&'] = 0;

	sgTextWordMap[(int)'\''] = 0;
	sgTextWordMap[(int)'\\'] = 0;

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
	sgNameMap[(int)'.'] = 0;
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
		{
			sgAvLenWeights[i] = vLens[k];
		}
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
	for (u32 i=0; i<sgNumCws; i++)
	{
		str = AosNextWord(sgNameMap, 
				sgNameMapMin, sgNameMapMax, 
				0, 0, 
				sgWordLenWeights, 
				sgWordLenWeightSize);
		aos_assert(str.length() < sgMaxCwLen);
		strcpy(sgCws[i], str.data());
	}

	int idx = 0;
	sgSdtAnames[idx++] = AOSTAG_APPNAME;
	sgSdtAnames[idx++] = AOSTAG_USERNAME;
	sgSdtAnames[idx++] = AOSTAG_OBJID;
	sgSdtAnames[idx++] = AOSTAG_OBJNAME;
	sgSdtAnames[idx++] = AOSTAG_OTYPE;
	sgSdtAnames[idx++] = AOSTAG_SITEID;
	sgSdtAnames[idx++] = AOSTAG_SUBTYPE;
	sgSdtAnames[idx++] = AOSTAG_TNAIL;
	sgSdtAnames[idx++] = AOSTAG_CMTTHREAD;
	sgSdtAnames[idx++] = AOSTAG_COMMENTED;
	sgSdtAnames[idx++] = AOSTAG_ICOMMENTED;
	sgSdtAnames[idx++] = AOSTAG_VVPD;
	sgSdtAnames[idx++] = AOSTAG_EVPD;
	sgSdtAnames[idx++] = AOSTAG_TAG;
	//sgSdtAnames[idx++] = AOSTAG_COUNERTRD;
	sgSdtAnames[idx++] = AOSTAG_COUNERTWR;
	sgSdtAnames[idx++] = AOSTAG_COUNERTUP;
	//sgSdtAnames[idx++] = AOSTAG_COUNERTDW;
	//sgSdtAnames[idx++] = AOSTAG_COUNERTCM;
	//sgSdtAnames[idx++] = AOSTAG_COUNERTRC;
	//sgSdtAnames[idx++] = AOSTAG_COUNERTLK;
	sgNumStdAnames = idx;

	for (int i=0; i<MAX_COMMON_TAGNAMES; i++)
	{
		sgCommTagnames[i] = "tag";
		sgCommTagnames[i] << i;
	}
}

static int sgNumXmldocCreated = 0;

AosTestXmlDoc::AosTestXmlDoc(
		const OmnString &siteid,
		const OmnString &docid,
		const OmnString &objid,
		const int depth)
:
mIsCdata(false),
mNumSubtags(0),
mMaxQueryFnames(10),
mNumAttrs(0)
{
	mSiteid = siteid;
	mDocid = docid;
	mObjid = objid;
	init(depth);

	addMetadata(depth);
}

AosTestXmlDoc::AosTestXmlDoc(
		const OmnString &path,
		const int depth)
:
mIsCdata(false),
mPath(path),
mNumSubtags(0),
mMaxQueryFnames(10),
mNumAttrs(0)
{
	init(depth);

	if (path == "") addMetadata(depth);
}


bool
AosTestXmlDoc::init(
		const OmnString &attrtname, 
		const OmnString &awdtname, 
		const OmnString &docwdtname, 
		const OmnString &tagtname, 
		const OmnString &doctname)
{
	mAttrTname1 = attrtname;
	mAttrwdTname1 = awdtname;
	mDocwdTname1 = docwdtname;
	mTagTname1 = tagtname;
	mDocTname1 = doctname;

	if (!sgInitFlag) initPriv();
	return true;
}


bool
AosTestXmlDoc::init(const int depth)
{
	// mSubtags.setNullValue(0);
	sgNumXmldocCreated++;
	if (!sgInitFlag) initPriv();
	sgInitFlag = true;

	// It creates a node with numChildren subtags and maximum depth.
	// Generate the tag name.
	if (depth == -3)
	{
		// It will create a constant doc:
		// 	<mydoc/>
		mTagname = "mydoc";
		return true;
	}

	if (depth == -4)
	{
		// It creates:
		// <mydoc1 zky_tag="red, blue" firstname="chen" lastname="ding"/>
		mTagname = "mydoc1";
		mAttrs[0].name = "zky_tag";
		mAttrs[0].value = "red, blue";
		mAttrs[1].name = "firstname";
		mAttrs[1].value = "chen";
		mAttrs[2].name = "lastname";
		mAttrs[2].value = "ding";
		mNumAttrs = 3;
		return true;
	}

	int vv = rand();
	if (vv % 100 < 95)
	{
		mTagname = sgCommTagnames[vv % MAX_COMMON_TAGNAMES];
	}
	else
	{
		mTagname = sgCws[rand()%sgNumCws];
	}

	if (depth == -2) return true;
	generateAttrs();
	if (depth == -1) return true;

	aos_assert_r(depth >= 0, false);
	vv = rand() % 2;
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
	return true;
}


AosTestXmlDoc::~AosTestXmlDoc()
{
	sgNumXmldocCreated--;
	for (int i=0; i<100; i++)
	{
		mSubtags[i] = 0;
	}
}


bool
AosTestXmlDoc::generateAttrs()
{
	// It generates a randum number of attributes. Both the attribute
	// names and values are randomly determined. Attributes are stored
	// into the db into the table: 
	// 		[docid, attrname, attrvalue]
	//
	// Determine whethere there shall be attributes
	int vv = rand() % 100;
	if (vv < 10) 
	{
		// No attributes
		return true;
	}

	// Determine how many attributes to generate
	int size;
	if (vv < 13) size = 100;
	else if (vv < 18) size = 50;
	else if (vv < 30) size = 15;
	else size = 10;

	mNumAttrs = rand() % size;
	if (mNumAttrs > eMaxAttrs) mNumAttrs = eMaxAttrs;

	OmnString stmt;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();

	// Generate attributes
	OmnString word;
	OmnString name, value;
	int numAttrs = mNumAttrs;
	mNumAttrs = 0;
	for (int i=0; i<numAttrs; i++)
	{
		int vv = rand();
		int vv1 = vv%100;
		if (vv1 < 70)
		{
			name = sgSdtAnames[vv%sgNumStdAnames];
		}
		else if (vv1 < 90)
		{
			name = sgCws[vv%sgNumCws];
		}
		else
		{
			// Generate attribute name. Note that we do not 
			// ensure attribute name uniqueness. 
			name = AosNextWord(sgNameMap, 
				sgNameMapMin, sgNameMapMax, 
				0, 0, 
				sgWordLenWeights, 
				sgWordLenWeightSize);
		}

		aos_assert_r(name != "", false);

		// Generate the value
		int idx = rand() % sgAvLenWeightSize;
		int range = sgAvLenWeights[idx];
		int numWords = (rand() % range) + 1;
		value = "";
		for (int k=0; k<numWords; k++)
		{
			if (vv1 < 70)
			{
				word = sgCws[rand()%sgNumCws];
			}
			else
			{
				word = AosNextWord(
					sgTextWordMap, sgTextWordMapMin, sgTextWordMapMax, 
					0, 0, 
					sgWordLenWeights, sgWordLenWeightSize);
			}
			if (k != 0) value << " ";
			value << word;
		}
		aos_assert_r(name != "", false);
		aos_assert_r(value != "", false);

		// XML does not allow attributes with empty values and 
		// duplicate attributes.
		bool found = false;
		for (int k=0; k<mNumAttrs; k++)
		{
			if (mAttrs[k].name == name)
			{
				found = true;
				break;
			}
		}

		if (name == AOSTAG_VERSION) name << "1";
		if (!found)
		{
			mAttrs[mNumAttrs].name = name;
			mAttrs[mNumAttrs].value = value;
			//Linda,
			if (mNumAttrs+20 <eMaxAttrs-1)  mNumAttrs++;
			//mNumAttrs++;
		}
	}
	return true;
}


bool
AosTestXmlDoc::generateTexts()
{
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	int numWords = rand() % eTextMaxWords;
	OmnString word;
	int vv = rand();
	mTexts = "";
	if ((vv % 2) == 0) 
	{
		// Generate cdata
		mIsCdata = true;
		for (int i=0; i<numWords; i++)
		{
			if ((vv % 100) < 60)
			{
				word = sgCws[rand() % sgNumCws];
			}
			else
			{
				word = AosNextWord(sgTextWordMap, 
							sgTextWordMapMin, sgTextWordMapMax, 
							0, 0,
							sgWordLenWeights, 
							sgWordLenWeightSize);
			}
			if (i != 0) mTexts << " ";
			mTexts << word;
		}
	}
	else
	{
		// Generate texts
		mIsCdata = false;
		for (int i=0; i<numWords; i++)
		{
			if ((vv % 100) < 60)
			{
				word = sgCws[rand() % sgNumCws];
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
		}
	}

	return true;
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

	OmnString path = mPath;
	if (path != "") path << "/";
	path << mTagname;
	for (int i=0; i<mNumSubtags; i++)
	{
		mSubtags[i] = OmnNew AosTestXmlDoc(path, depth-1);	
	}
}


void
AosTestXmlDoc::composeData(const u64 &docid, OmnString &data)
{
	data << "<" << mTagname;
	if (docid) data << " " << AOSTAG_DOCID << "=\"" << docid << "\" ";
	if (mNumAttrs > 0)
	{
		// There are attributes
		for (int i=0; i<mNumAttrs; i++)
		{
			aos_assert(mAttrs[i].name != "");
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
		mIsCdata = true;//Linda,2010/12/23 mIsCdata
		if (mIsCdata) data << "><![CDATA[" << mTexts << "]]></" << mTagname << ">";
		else data << ">" << mTexts << "</" << mTagname << ">";
		return;
	}

	// It is in the form:
	//	<tagname> xxx <subtag ...>...</tagname> or
	//	<tagname><subtag ...>...</tagname>
	/*
	//Linda 11/22/2010 setText()
	if (rand() % 2)
	{
		// Make it the form:
		//	<tagname> xxx <subtag ...>...</tagname> or
		if (mIsCdata) data << "><![CDATA[" << mTexts << "]]>";
		else data << ">" << mTexts;
		for (int i=0; i<mNumSubtags; i++)
			mSubtags[i]->composeData(data);
		data << "</" << mTagname << ">";
	}	
	else
	{*/
		// Make it the form:
		//	<tagname><subtag ...>...</tagname>
		data << ">";
		for (int i=0; i<mNumSubtags; i++)
			mSubtags[i]->composeData(0, data);
		data << "</" << mTagname << ">";
		//if (mIsCdata) data << "<![CDATA[" << mTexts << "]]></" << mTagname << ">";
		//else data << mTexts << "</" << mTagname << ">";
	//}
	
	/*
	if (rand() % 2)
	{
		// Make it the form:
		//	<tagname> xxx <subtag ...>...</tagname> or
		if (mIsCdata) data << "><![CDATA[" << mTexts << "]]>";
		else data << ">" << mTexts;
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
		if (mIsCdata) data << "<![CDATA[" << mTexts << "]]></" << mTagname << ">";
		else data << mTexts << "</" << mTagname << ">";
	}
	*/
}


bool
AosTestXmlDoc::saveWords()
{
	// Attributes are stored in the database table named by
	// mAttrTname in the form:
	// 		[docid, aname, avalue]
	
	OmnString stmt;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	for (int i=0; i<mNumAttrs; i++)
	{
		OmnString avalue = mAttrs[i].value;
		stmt = "insert into ";
		stmt << mAttrTname1 << " (siteid, docid, aname, avalue) values ('";
		stmt << mSiteid << "', '" << mDocid << "', '";
		if (mPath != "") stmt << mPath << "/";
		stmt << mAttrs[i].name << "', '" << avalue << "')";
		aos_assert_r(store->runSQL(stmt), false);

		OmnStrParser1 parser(avalue, ", ", false, false);
		OmnString word;
		while ((word = parser.nextWord("")) != "")
		{
			stmt = "insert into ";
			stmt << mAttrwdTname1 << " (siteid, docid, aname, aword) values ('";
			stmt << mSiteid << "', '" << mDocid << "', '";
			if (mPath != "") stmt << mPath << "/";
			stmt << mAttrs[i].name << "', '" << word << "')";
			aos_assert_r(store->runSQL(stmt), false);
		}
	}

	OmnStrParser1 parser(mTexts, ", \n\t", false, false);
	OmnString word;
	while ((word = parser.nextWord("")) != "")
	{
		OmnString stmt = "insert into ";
		stmt << mDocwdTname1 << " (siteid, docid, word) values ('"
			<< mSiteid << "', '" << mDocid << "', '" << word << "')";
		if (!store->runSQL(stmt))
		{
			OmnAlarm << "Failed the database operation: " << stmt << enderr;
			return false;
		}
	}

	updateCounters();

	for (int i=0; i<mNumSubtags; i++) 
	{
		mSubtags[i]->mSiteid = mSiteid;
		mSubtags[i]->mDocid = mDocid;

		OmnString path = mPath;
		if (path != "") path << "/";
		path << mSubtags[i]->getTagname1();
		mSubtags[i]->setPath(path);
		mSubtags[i]->saveWords();
	}
	sgLock.lock();
	sgNumAttrs += mNumAttrs; 
	sgLock.unlock();
	return true;
}


bool
AosTestXmlDoc::getFnames(const OmnString &siteid, OmnString &fnames)
{
	// It creates a series of field names, separated by commands
	// and/or spaces. The torturer keeps a list of commonly 
	// used field names. It will use the commonly used
	// names most of the time, but will also use some uncommon
	// names. 
	int numFields = 1 + (rand() % mMaxQueryFnames);
	fnames = "";
	for (int i=0; i<numFields; i++)
	{
		if (fnames != "") fnames << ", ";
		fnames << getAttrname(siteid);
	}

	return true;
}


OmnString
AosTestXmlDoc::getAttrname(const OmnString &siteid)
{
	OmnString aname;

	u32 vv = (u32)rand();
	u32 vv1 = vv%100;
	// if (vv1 < 80)
	if (vv1 < 120)
	{
		// Select one from the database
		if (sgNumAttrs == 0) updateCounters();
		int start = (sgNumAttrs == 0)?1:rand() % sgNumAttrs;
		OmnString stmt = "select aname from ";
		stmt << mAttrTname1 << " where siteid='" << siteid << "' limit " << start << ", 1";

		OmnDbTablePtr table;
		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		aos_assert_r(store->query(stmt, table), "");
		if (table->entries() > 0)
		{
			table->reset();
			OmnDbRecordPtr record = table->next();
			aos_assert_r(record, "");
			OmnRslt rslt;
			aname = record->getStr(0, "", rslt);
			if (aname != "") goto finish;
		}
	}

	if (vv1 < 90)
	{
		u32 tt = vv%sgNumCws;
		aname = sgCws[tt];
		goto finish;
	}
	
	// Generate attribute name. Note that we do not 
	// ensure attribute name uniqueness. 
	aname = AosNextWord(sgNameMap, 
		sgNameMapMin, sgNameMapMax, 
		0, 0, 
		sgWordLenWeights, 
		sgWordLenWeightSize);
	goto finish;

finish:
	if (aname == "" || aname == AOSTAG_SITEID || aname == AOSTAG_OBJID)
	{
		aname << rand();
	}
	return aname;
}


bool
AosTestXmlDoc::saveDocWord(const OmnString &word)
{
	OmnNotImplementedYet;
	return false;
}


OmnString 
AosTestXmlDoc::getAttrvalue(
		const OmnString &aname, 
		const OmnString &siteid,
		const OmnString &docid)
{
	// It randomly determine whether to select a valid 
	// attribute value or not. Note that all attributes
	// are stored in the table named by mAttrTname:
	// 	[docid, name, value]
	
	OmnString value;
	int vv = rand() % 100;
	if (vv < 85)
	{
		// Select a valid attribute value
		OmnString stmt;
		OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
		stmt = "select avalue from ";
		stmt << mAttrTname1;
		int startidx = 0;
		int size = 200;

		bool found = false;
		if (siteid != "")
		{
			stmt << " where siteid='" << siteid << "'";
			found = true;
		}

		if (docid != "")
		{
			if (!found) stmt << " where ";
			else stmt << " and ";
			stmt << "docid='" << docid << "'";
			found = true;
		}

		if (aname != "")
		{
			if (!found) stmt << " where ";
			else stmt << " and ";
			stmt << "aname='" << aname << "'";
		}

		stmt << " limit " << startidx << ", " << size;

		OmnDbTablePtr table;
		aos_assert_r(store->query(stmt, table), "");
		if (table->entries() > 0)
		{
			int idx = rand() % table->entries();
			OmnDbRecordPtr record = table->getRecord(idx);
			aos_assert_r(record, "");
			OmnRslt rslt;
			value = record->getStr(0, "", rslt);
			goto finish;
		}
	}

	vv = rand() % 15;
	for (int i=0; i<vv; i++)
	{
		if (value != "") value << " ";
		value << getDocWord();
	}

	goto finish;

finish:
	if (value == "") value << rand();
	return value;
}


OmnString 
AosTestXmlDoc::getDocWord()
{
	int vv = rand();
	int vv1 = vv%100;
	if (vv1 < 90)
	{
		return sgCws[vv%sgNumCws];
	}
	
	// Generate attribute name. Note that we do not 
	// ensure attribute name uniqueness. 
	return AosNextWord(sgTextWordMap, 
		sgTextWordMapMin, sgTextWordMapMax, 
		0, 0, 
		sgWordLenWeights, 
		sgWordLenWeightSize);
}


OmnString 
AosTestXmlDoc::getTexts()
{
	int len = rand()%20;
	OmnString txt;
	for (int i=0; i<len; i++)
	{
		if (txt != "") txt << " ";
		txt << getDocWord();
	}
	return txt;
}


bool
AosTestXmlDoc::addAttr(const OmnString &name, const OmnString &value)
{
	for (int i=0; i<mNumAttrs; i++)
	{
		if (mAttrs[i].name == name)
		{
			mAttrs[i].value = value;
			return true;
		}
	}

	aos_assert_r(mNumAttrs < eMaxAttrs, false);
	mAttrs[mNumAttrs].name = name;
	mAttrs[mNumAttrs].value= value;
//Linda,
	if (mNumAttrs+20 <eMaxAttrs-1) mNumAttrs++;
//	mNumAttrs++;
	return true;
}


OmnString
AosTestXmlDoc::getRandomSiteid()
{
	OmnString ss;
	ss << (rand() % MAX_SITEID) + SITEID_START;
	return ss;
}


bool
AosTestXmlDoc::addMetadata(const int depth)
{
	aos_assert_r(mNumAttrs + 20 < eMaxAttrs, false);

	OmnString value = "docname";
	int vv = (depth == -3)?1:rand() % eMaxMetadata;
	value << vv;
	addAttr(AOSTAG_OBJNAME, value);

		
	//Linda 09/18/2010
	//value = mSiteid;
	value = "100";

	if (value == "") 
	{
		value << (rand() % MAX_SITEID) + SITEID_START;
		mSiteid = value;
	}
	addAttr(AOSTAG_SITEID, value);

	if (mDocid != "") addAttr(AOSTAG_DOCID, mDocid);

	value = mObjid;
	if (value == "")
	{
		value = "objid_";
		value << sgObjid++;
		mObjid = value;
	}
	addAttr(AOSTAG_OBJID, value);

	value = "tnail";
	vv = (depth == -3)?1:rand() % eMaxMetadata;
	value << vv;
	addAttr(AOSTAG_TNAIL, value);

	value = "doctype";
	vv = (depth == -3)?1:rand() % eMaxMetadata;
	value << vv;
	addAttr(AOSTAG_OTYPE, value);

	value = "subtype";
	vv = (depth == -3)?1:rand() % eMaxMetadata;
	value << vv;
	addAttr(AOSTAG_SUBTYPE, value);

	value = "appname";
	vv = (depth == -3)?1:rand() % eMaxMetadata;
	value << vv;
	addAttr(AOSTAG_APPNAME, value);
	return true;
}


bool
AosTestXmlDoc::updateCounters()
{
	OmnString stmt = "select count(*) from ";
	stmt << mAttrTname1;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();

	OmnDbRecordPtr record;
	OmnRslt rslt;
	aos_assert_r(store->query(stmt, record), false);
	aos_assert_r(record, false);
	sgNumAttrs = record->getInt(0, -1, rslt);
	aos_assert_r(sgNumAttrs >= 0, false);
	return true;
}


OmnString
AosTestXmlDoc::getRandomTagname()
{
	int vv = rand();
	if ((vv % 100) < 50)
	//if ((vv % 100) < 95)
	{
		 return sgCommTagnames[vv % MAX_COMMON_TAGNAMES];
	}
	
	return sgCws[vv%sgNumCws];
}


OmnString 
AosTestXmlDoc::getCommonWords()
{
	return sgCws[rand()%sgNumCws];
}


void	
AosTestXmlDoc::setDocid(const OmnString &docid)
{
	mDocid = docid;
	addAttr(AOSTAG_DOCID, docid);
}


void	
AosTestXmlDoc::setSiteid(const OmnString &s)
{
	mSiteid = s;
	addAttr(AOSTAG_SITEID, s);
}



OmnString
AosTestXmlDoc::createNewDoc(const u64 &docid)
{
	OmnString data;
	composeData(docid, data);
	aos_assert_r(data!="", 0);
	return data;
}

