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

const static  int sgMaxDocs = 100000;
const static int eMaxPatternLen = 300;
static int eMinPatternLen = 2;
static int mNumDocs = 0;
static int eMaxRepeat = 1000;
static int eMinRepeat = 10;
static int eMaxReads = 100;
static AosDocFileMgr sgDocFileMgr;
static u32 sgOffset[sgMaxDocs];
static u32 sgSeqno[sgMaxDocs];
static int sgRepeat[sgMaxDocs];
static int sgPatternLen[sgMaxDocs];
static char sgPattern[sgMaxDocs][eMaxPatternLen];
static int cn = 0;
static int dn = 0;
static int mn = 0;
static char data[30000];//eMaxPatternLen*eMaxRepeat
static char pattern[eMaxPatternLen];

AosDiskBlockTester::AosDiskBlockTester()
{
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
	const int maxheaderperfile = 10000;	
	const int maxdocfiles = 10000;
	const int maxheaderfiles = 10000;
	const OmnString dirname = "/usr/AOS/Data";
	const OmnString prefix = "doc_";

	sgDocFileMgr.init(maxfilesize, maxheaderperfile, maxdocfiles, maxheaderfiles, 
			dirname, prefix);
	mBlocksizeIdxs[0] = 10000; 		mBlocksizes[0] = 500000;    //0.5M
	mBlocksizeIdxs[1] = 100000; 	mBlocksizes[1] = 2000000;   //2M
	mBlocksizeIdxs[2] = 500000; 	mBlocksizes[2] = 5000000;   //5M
	mBlocksizeIdxs[3] = 1000000; 	mBlocksizes[3] = 20000000;  //20M
	mBlocksizeIdxs[4] = 5000000; 	mBlocksizes[4] = 40000000;  //40M
	mBlocksizeIdxs[5] = 10000000;	mBlocksizes[5] = 50000000;  //50M
    return true;
}
bool 
AosDiskBlockTester::normalTest()
{
	/*
	for(int i=0;i<9999;i++)
	{
		if(i==9999)
		{
		    printf("%d\n",i);
		}
		createDoc();
	}
	for(int i=0;i<1;i++)
	{
        modifyDoc();
	}
	for(int i=0;i<1;i++)
	{
        readDoc();
	}
	*/
	bool rslt;
	for (int i=0; ; i++)
	{
		if(i%100==0)
		{
			printf("%8d%5d cn=%d mn=%d\n",i,mNumDocs,cn,mn);
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
			modifyDoc();
			mn++;
		    aos_assert_r(rslt, false);
		}
	    readDoc();
	}
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
		//pattern[i] = (char)rand();
		pattern[i] = (char)rand()%256;
	}

	for (int i=0; i<repeat; i++)
	{
		memcpy(&data[i*pattern_len], pattern, pattern_len);
	}

	return true;
}


bool 
AosDiskBlockTester::createDoc()
{
	if (mNumDocs >= sgMaxDocs) return true;
	if(mNumDocs >= sgMaxDocs - 1)return true;//enl

	int k = open("/usr/AOS/Data/doc__99",0);//enl: seqno < 100
	if(k>0)
	{
		close(k);
		return true;
	}
	// 1. Determine pattern length and repeat
	int pattern_len = rand() % eMaxPatternLen + eMinPatternLen;
	if(pattern_len >= eMaxPatternLen) return true;  //enl:pattern_len must less than eMaxPatternLen(30) for sgPattern
	int repeat = rand() % eMaxRepeat +eMinRepeat;

	int docsize=pattern_len*repeat;
	int blocksizeIdx;
	for (int i=0; i<6; i++)
	{
		if (docsize <= mBlocksizeIdxs[i]) 
		{
			blocksizeIdx=i;
			break;
		}
	}

	// 2. Set the contents
	setContents(data, repeat, pattern_len, pattern);

	// 3. Create the doc
	//u32 seqno; 
	u32 seqno = 0; 
	u64 offset = 0;
	bool rslt = sgDocFileMgr.saveDoc(seqno, offset, pattern_len * repeat, data);
	aos_assert_r(rslt, false);

	sgOffset[mNumDocs] = offset;
	sgSeqno[mNumDocs] = seqno;
	sgRepeat[mNumDocs] = repeat;
	sgPatternLen[mNumDocs] = pattern_len;
	memcpy(sgPattern[mNumDocs], pattern, pattern_len);
	mNumDocs++;

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
	int repeat = rand() % eMaxRepeat + eMinRepeat;

	if(repeat > sgRepeat[idx])return true;
	if(pattern_len > sgPatternLen[idx])return true;

	// 2. modify the contents
	setContents(data, repeat, pattern_len, pattern);

	// 3. modify the doc and change the array.
	u32 seqno = sgSeqno[idx]; 
	u64 offset = sgOffset[idx];
	bool rslt = sgDocFileMgr.saveDoc(seqno, offset, pattern_len * repeat, data);
	aos_assert_r(rslt, false);

	aos_assert_r(offset != 0, false);
	sgOffset[idx] = offset;
	sgSeqno[idx] = seqno;
	sgRepeat[idx] = repeat;
	sgPatternLen[idx] = pattern_len;
	memcpy(sgPattern[idx], pattern, pattern_len);

	return true;
}


bool 
AosDiskBlockTester::deleteDoc()
{
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

		bool rslt = sgDocFileMgr.readDoc(seqno, offset, data, pattern_len * repeat);

		aos_assert_r(rslt, false);
		for (int k=0; k<repeat; k++)
		{
			int ret = memcmp(&data[k*pattern_len], sgPattern[idx], pattern_len);
			if(ret != 0)
			{
			    int fd = creat("1",0777);
			    write(fd,&data[k*pattern_len],pattern_len);
			    close(fd);
			    fd = creat("2",0777);
			    write(fd,sgPattern[idx],pattern_len);
			    close(fd);
				printf("data error\n");
				exit(0);
			    aos_assert_r(ret == 0, false);
			}
		}
	}

	return true;
}

