////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// Modification History:
// 07/26/2010	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/Testers/LargeFileTester.h"

#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/IdleBlockMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"

extern AosXmlTagPtr gAosAppConfig;

static int sgTotalDocsCreated = 0;
const static  int sgMaxDocs = 1000000;
const static int eMaxPatternLen = 6;
static int eMinPatternLen = 20;
static int mNumDocs = 0;
static int eMaxRepeat = 1000;
static int eMinRepeat = 10;
static int eMaxReads = 100;
static AosIdleBlockMgr sgIdleBlockMgr;
static u64 sgOffset[sgMaxDocs];
static u32 sgSeqno[sgMaxDocs];
static int sgRepeat[sgMaxDocs];
static int sgPatternLen[sgMaxDocs];
static char* sgPattern[sgMaxDocs];
static int sgDocIdx[sgMaxDocs];
static int cn = 1;
static int dn = 0;
static int mn = 0;


AosLargeFileTester::AosLargeFileTester()
{
	mName = "Tester";
}


bool 
AosLargeFileTester::start()
{
	cout << "    Start Tester ...";
	prepareEnv();
	normalTest();

	return true;
}


bool 
AosLargeFileTester::prepareEnv()
{
	const int maxfilesize = 2000;		 
	const OmnString dirname = "/usr/TestData";
	const OmnString sizeIdxFileName = "l_idx_size";
	const OmnString seqnoIdxFileName = "l_idx_seqno";
	const OmnString docFilename = "l_doc";

	sgIdleBlockMgr.init(maxfilesize, dirname, sizeIdxFileName, seqnoIdxFileName, docFilename);
    return true;
}


bool 
AosLargeFileTester::normalTest()
{
	bool rslt = createDoc();
	aos_assert_r(rslt, false);

	for (int i=0;; i++)
	{
	//	if(i%2==0)
	//	{
//			    printf("%8d%5d cn=%d mn=%d dn=%d\n",i,mNumDocs,cn,mn,dn);
	//	}
		
		int id = rand() % 100;
		if (id < eCreateWeight) 
		{
			rslt = createDoc();
			cn++;
		}
		else if (id < eDeleteWeight) 
		{
			rslt = deleteDoc();
			dn++;
		}
		else 
		{
			rslt = modifyDoc();
			mn++;
		}

		aos_assert_r(rslt, false);
		readDoc();
printf("%8d%5d cn=%d mn=%d dn=%d\n",i,mNumDocs,cn,mn,dn);
	}
	return true;
}


bool 
AosLargeFileTester::setContents(
		const int docidx,
		char *data, 
		const int repeat, 
		const int pattern_len,
		char *pattern)
{
	for (int i=0; i<pattern_len; i++)
	{
		pattern[i] = 'a' + rand()%26;
	}
	sprintf(pattern, "%d", docidx);
	pattern[strlen(pattern)] = 'a';

	for (int i=0; i<repeat; i++)
	{
		memcpy(&data[i*pattern_len], pattern, pattern_len);
	}

	return true;
}


bool 
AosLargeFileTester::createDoc()
{
	if (mNumDocs >= sgMaxDocs) return true;

	// 1. Determine pattern length and repeat
	int pattern_len = rand() % eMaxPatternLen + eMinPatternLen;
	int repeat = rand() % eMaxRepeat + eMinRepeat;

	// 2. Set the contents
	char *data = new char[pattern_len * repeat];
	char *pattern = new char[pattern_len];
	setContents(sgTotalDocsCreated, data, repeat, pattern_len, pattern);

	// 3. Create the doc
	//u32 seqno; 
	u32 seqno = 0; 
	u64 offset = 0;
if (sgTotalDocsCreated == 53)
	OmnMark;
	bool rslt = sgIdleBlockMgr.saveDoc(seqno, offset, pattern_len*repeat, data);
	if(!rslt)
	{
		delete [] data;
		delete [] pattern;
		aos_assert_r(rslt, false);
	}
	sgOffset[mNumDocs] = offset;
	sgSeqno[mNumDocs] = seqno;
	sgRepeat[mNumDocs] = repeat;
	sgPatternLen[mNumDocs] = pattern_len;
	sgPattern[mNumDocs] = pattern;
	sgDocIdx[mNumDocs] = sgTotalDocsCreated++;
	mNumDocs++;

	delete [] data;
	return true;
}


bool 
AosLargeFileTester::modifyDoc()
{
	// 1. If there are no docs, return
	// 2. Randomly pick a doc to modify
	// 3. Create a new content
	// 4. Modify the doc
	// 5. Modify the internal data
	if (mNumDocs == 0) return true;
	int idx = rand() % mNumDocs;
	int pattern_len = rand() % eMaxPatternLen + eMinPatternLen;
	int repeat = rand() % eMaxRepeat + eMinRepeat;

	char *data = new char[pattern_len * repeat];
	char *pattern = new char[pattern_len];
	setContents(sgDocIdx[idx], data, repeat, pattern_len, pattern);

	u32 seqno = sgSeqno[idx]; 
	u64 offset = sgOffset[idx];
	//int plen = sgPatternLen[idx];
	//int repr = sgRepeat[idx];
	bool rslt = sgIdleBlockMgr.saveDoc(seqno, offset, pattern_len*repeat, data);
	if(!rslt)
	{
		OmnAlarm << "Failed to modify!" << enderr;	
		delete [] data;
		delete [] pattern;
		return false;
	}

	aos_assert_r(offset != 0, false);
	sgOffset[idx] = offset;
	sgSeqno[idx] = seqno;
	sgRepeat[idx] = repeat;
	sgPatternLen[idx] = pattern_len;
	
	
	delete [] sgPattern[idx];
	sgPattern[idx] = pattern;
	
	delete [] data;
	return true;
}


bool 
AosLargeFileTester::deleteDoc()
{
	// 1. If there are no docs, return
	// 2. Randomly pick a doc to delete
	// 3. Delete the doc
	// 4. Remove the doc from the internal list
	if (mNumDocs == 0) return true;

	int idx = rand() % mNumDocs;
	u32 seqno = sgSeqno[idx];
	u64 offset = sgOffset[idx];
	//int repeat = sgRepeat[idx];
	//int pattern_len = sgPatternLen[idx];

	bool rslt = sgIdleBlockMgr.deleteDoc(seqno, offset);
	aos_assert_r(rslt, false);
	
	delete [] sgPattern[idx];
	if(idx == mNumDocs-1)
	{
		mNumDocs--;
		return true;
	}
	sgPattern[idx] =  sgPattern[mNumDocs-1];
	sgOffset[idx] = sgOffset[mNumDocs-1];
	sgSeqno[idx] = sgSeqno[mNumDocs-1];
	sgRepeat[idx] = sgRepeat[mNumDocs-1];
	sgPatternLen[idx] = sgPatternLen[mNumDocs-1];
	mNumDocs--;
	return true;
}


bool 
AosLargeFileTester::readDoc()
{
	if (mNumDocs == 0) return true;

	int num_reads = rand() % eMaxReads;
	for (int i=0; i<num_reads; i++)
	{
		int idx = rand() % mNumDocs;

		u32 seqno = sgSeqno[idx];
		u64 offset = sgOffset[idx];
		int repeat = sgRepeat[idx];
		int pattern_len = sgPatternLen[idx];

		char *data = new char[pattern_len * repeat];
		bool rslt = sgIdleBlockMgr.readDoc(seqno, offset, pattern_len * repeat, data);
		
		if(!rslt)
		{
			delete [] data;
			aos_assert_r(rslt, false);
		}
		for (int k=0; k<repeat; k++)
		{
			//if(memcmp(&data[k*pattern_len],sgPattern[idx], pattern_len) != 0)
			if (memcmp(&data[k*pattern_len], sgPattern[idx], pattern_len))
			{
				OmnAlarm << "Failed checking: " << seqno << ":" << offset
					<< ":" << repeat << ":" << pattern_len << ":" << k<< enderr;
				return false;
			}
		}
		delete [] data;
	}
	return true;
}

