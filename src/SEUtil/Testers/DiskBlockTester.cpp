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
// 07/26/2010	Created by James Kong
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/Testers/DiskBlockTester.h"

#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "SEUtil/DocFileMgr.h"
#include "XmlUtil/SeXmlParser.h"
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

const static  int sgMaxDocs = 500000;
const int eMaxPatternLen = 300;
static int eMinPatternLen = 2;
static int mNumDocs = 0;
const int eMaxRepeat = 100000;
static int eMaxReads = 100;
static AosDocFileMgr sgDocFileMgr;
static u32 sgOffset[sgMaxDocs];
static u32 sgSeqno[sgMaxDocs];
static int sgRepeat[sgMaxDocs];
static int sgPatternLen[sgMaxDocs];
static char sgPatterns[sgMaxDocs][eMaxPatternLen];
static int cn = 0;
static int dn = 0;
static int mn = 0;
static char sgData[eMaxRepeat * eMaxPatternLen + 1];
static char sgPattern[eMaxPatternLen];

const int eRepeatLevel1 = 40;			// patternlen == 300, average 6k
const int eRepeatLevel2 = 400;			// patternlen == 300, average 60k
const int eRepeatLevel3 = 4000;			// patternlen == 300, average 600k
const int eRepeatLevel4 = 10000;		// patternlen == 300, average 1.5M
const int eRepeatLevel5 = 40000;		// patternlen == 300, average 6M
const int eRepeatLevel6 = eMaxRepeat;	// patternlen == 300, average 150M

AosDiskBlockTester::AosDiskBlockTester()
{
	mRepeatLevel1 = 30;
	mRepeatLevel2 = 60;
	mRepeatLevel3 = 80;
	mRepeatLevel4 = 90;
	mRepeatLevel5 = 97;
	mName = "Tester";
}


bool 
AosDiskBlockTester::start()
{
	cout << "    Start Tester ...";
	prepareEnv();
	normalTest();

	return true;
}

int mBlocksizeIdxs[6],mBlocksizes[6];

bool 
AosDiskBlockTester::prepareEnv()
{
	const int maxfilesize = 100000000;		// 100M
	const int largefilesize = 10000;		// 10G
	const int maxheaderperfile = 10000;	
	const int maxdocfiles = 10000;
	const int maxheaderfiles = 10000;
	const OmnString dirname = "/usr/AOS/Data";
	const OmnString prefix = "doc_";

	sgDocFileMgr.init(maxfilesize, largefilesize, maxheaderperfile, maxdocfiles, maxheaderfiles, 
			dirname, prefix);
    return true;
}
bool 
AosDiskBlockTester::normalTest()
{
	bool rslt;
	// for (int i=0; i<5000; i++)
	int i=0;
	while (mNumDocs < sgMaxDocs)
	{
		if(i++%5000==0)
		{
			cout << "Total Docs: " << mNumDocs << ":" << cn << ":" << mn << ":" << allDoc() << endl;
		}
		int idx = rand() % 100;
		if(idx < eCreateWeight)
		{
			rslt = createDoc();
			cn++;
		    aos_assert_r(rslt, false);
		}
		else if(idx < eDeleteWeight)
		{
			rslt = deleteDoc();
			dn++;
		    aos_assert_r(rslt, false);
		}
		else
		{
			rslt = modifyDoc();
			mn++;
		    aos_assert_r(rslt, false);
		}
	     readDoc();
	}
	cout << "Total Disk: " << allDoc() << endl;
	return true;
}


bool 
AosDiskBlockTester::setContents(
		char *data, 
		const int repeat, 
		const int pattern_len,
		char *pattern)
{
	// Determine the pattern contents
	for (int i=0; i<pattern_len; i++)
	{
		pattern[i] = (char)rand();
	}

	for (int i=0; i<repeat; i++)
	{
		memcpy(&data[i*pattern_len], pattern, pattern_len);
	}

	return true;
}


int 
AosDiskBlockTester::pickRepeat()
{
	int vv = rand() % 100;
	if (vv < mRepeatLevel1) return (rand() % eRepeatLevel1) + 1;
	if (vv < mRepeatLevel2) return (rand() % eRepeatLevel2) + 1;
	if (vv < mRepeatLevel3) return (rand() % eRepeatLevel3) + 1;
	if (vv < mRepeatLevel4) return (rand() % eRepeatLevel4) + 1;
	if (vv < mRepeatLevel5) return (rand() % eRepeatLevel5) + 1;
	return eRepeatLevel6 + 1;
}


bool 
AosDiskBlockTester::createDoc()
{
	if (mNumDocs >= sgMaxDocs) 
	{
		OmnScreen << "Max docs reached!" << endl;
		return true;
	}

	// 1. Determine pattern length and repeat
	int pattern_len = rand() % eMaxPatternLen + eMinPatternLen;
	if(pattern_len >= eMaxPatternLen) return true;  //enl:pattern_len must less than eMaxPatternLen(30) for sgPattern

	int repeat = pickRepeat();

	// 2. Set the contents
	setContents(sgData, repeat, pattern_len, sgPattern);

	// 3. Create the doc
	//u32 seqno; 
	u32 seqno = 0; 
	u64 offset = 0;
	bool rslt = sgDocFileMgr.saveDoc(seqno, offset, pattern_len * repeat, sgData);
	aos_assert_r(rslt, false);

	sgOffset[mNumDocs] = offset;
	sgSeqno[mNumDocs] = seqno;
	sgRepeat[mNumDocs] = repeat;
	sgPatternLen[mNumDocs] = pattern_len;
	memcpy(sgPatterns[mNumDocs], sgPattern, pattern_len);
	mNumDocs++;

	// OmnScreen << "Create doc: " << mNumDocs << ":" 
	// 	<< seqno << ":" << offset << ":" << pattern_len * repeat << endl;
	return true;
}


bool 
AosDiskBlockTester::modifyDoc()
{
	if (mNumDocs == 0) return true;
	if(mNumDocs >= sgMaxDocs - 1)return true;//enl

	int idx = rand() % mNumDocs;
	// 1. change pattern length and repeat
	int pattern_len = rand() % eMaxPatternLen + eMinPatternLen;
	if(pattern_len >= eMaxPatternLen) return true;  //enl:pattern_len must less than eMaxPatternLen(30) for sgPattern
	int repeat = pickRepeat();

	if(repeat > sgRepeat[idx])return true;
	if(pattern_len > sgPatternLen[idx])return true;

	// 2. modify the contents
	setContents(sgData, repeat, pattern_len, sgPattern);

	// 3. modify the doc and change the array.
	u32 seqno = sgSeqno[idx]; 
	u64 offset = sgOffset[idx];
	int repeatt = sgRepeat[idx];
	int pattern_lenn = sgPatternLen[idx];

	if(pattern_len * repeat < pattern_lenn * repeatt) return true;

	bool rslt = sgDocFileMgr.saveDoc(seqno, offset, pattern_len * repeat, sgData);
	aos_assert_r(rslt, false);

	aos_assert_r(offset != 0, false);
	sgOffset[idx] = offset;
	sgSeqno[idx] = seqno;
	sgRepeat[idx] = repeat;
	sgPatternLen[idx] = pattern_len;
	memcpy(sgPatterns[idx], sgPattern, pattern_len);

	OmnScreen << "Modify doc: " << seqno << ":" << offset << ":" << pattern_len * repeat << endl;

	return true;
}


bool 
AosDiskBlockTester::deleteDoc()
{
	
	if (mNumDocs == 0) return true;

	int idx = rand() % mNumDocs;
	u32 seqno = sgSeqno[idx];
	u64 offset = sgOffset[idx];

	bool rslt = sgDocFileMgr.deleteDoc(seqno, offset);
	aos_assert_r(rslt, false);

	if(idx == mNumDocs-1)
	{
	   mNumDocs--;
	   return true;
	}
	sgOffset[idx] = sgOffset[mNumDocs-1];
	sgSeqno[idx] = sgSeqno[mNumDocs-1];
	sgRepeat[idx] = sgRepeat[mNumDocs-1];
	sgPatternLen[idx] = sgPatternLen[mNumDocs-1];
	memcpy(sgPatterns[idx], sgPatterns[mNumDocs-1], sgPatternLen[mNumDocs-1]);
	mNumDocs--;
	
	return true;
}


bool 
AosDiskBlockTester::readDoc()
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

		bool rslt = sgDocFileMgr.readDoc(seqno, offset, sgData, pattern_len * repeat);

		aos_assert_r(rslt, false);
		for (int k=0; k<repeat; k++)
		{
			int ret = memcmp(&sgData[k*pattern_len], sgPatterns[idx], pattern_len);
			if(ret != 0)
			{
			    int fd = creat("1",0777);
			    write(fd,&sgData[k*pattern_len],pattern_len);
			    close(fd);
			    fd = creat("2",0777);
			    write(fd,sgPatterns[idx],pattern_len);
			    close(fd);
				printf("data error\n");
				exit(0);
			    aos_assert_r(ret == 0, false);
			}
		}
	}
	return true;
}


u64
AosDiskBlockTester::allDoc()
{
	u64 allbytes = 0;
	for (int k=0; k<mNumDocs; k++)
	{
		if (sgSeqno[k] % 2 == 1) continue; 
		allbytes += sgRepeat[k]*sgPatternLen[k];
	}
	return allbytes;
}

