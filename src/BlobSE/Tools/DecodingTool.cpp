////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 27 Apr 2015 created by White
////////////////////////////////////////////////////////////////////////////

#include <getopt.h>
#include <sstream>

#include "aosUtil/Types.h"
#include "AppMgr/App.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Entry.h"
#include "Porting/Mutex.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "util2/global_data.h"

u32 	g_ulBuffLen = 1000000;		//1M
int 	gAosShowNetTraffic = 1;		//for the aos framework
bool	gSanityCheck = true;

void
usage(char* p)
{
	printf("Usage: %s\n", p);
	printf("\t-c header custom data size\n");
	printf("\t-f file name with full path to decode\n");
	printf("\t-o offset in decimal, eg:5678\n");
	printf("\t-O offset in hex, eg:0x1234\n");
	printf("\t-t file type: header or body\n");
}


int
main(int argc, char **argv)
{
	int opt = 0;
	OmnString sFileName = "";
	OmnString sFileType = "";
	u64 ullOffset = 0;
	u32 ulHeaderCustomDataSize = 0;
	std::stringstream ss;
	aos_global_data_init();
	OmnApp::appStart(argc, argv);
	OmnFile* pFile = OmnNew OmnFile(AosMemoryCheckerArgsBegin);
	if (!pFile)
	{
		cerr << "create file obj failed." << endl;
		return -1;
	}
	while((opt = getopt(argc, argv, "f:o:t:c:O:")) != -1)
	{
		switch (opt)
		{
		case 'c':
			ulHeaderCustomDataSize = atoi(optarg);
			break;
		case 'f':
			sFileName = optarg;
			break;
		case 'o':
			ullOffset = atoi(optarg);
			break;
		case 'O':
			ss.str("");
			ss.clear();
			ss << std::hex << optarg;
			ss >> ullOffset;
			break;
		case 't':
			sFileType = optarg;
			break;
		default:
			std::cout << "Invalid argument." << endl;
			usage(argv[0]);
			return -99;
			break;
		}
	}
	OmnApp theApp(argc, argv);
	if (sFileName == "")
	{
		cerr << "invalid file name" << endl;
		usage(argv[0]);
		return -2;
	}
	if (!pFile->openFile1(sFileName, OmnFile::eReadWrite))
	{
		cerr << "can not open file:'" << sFileName << "'" << endl;
		usage(argv[0]);
		return -3;
	}
	if (ullOffset > pFile->getLength())
	{
		cerr << "invalid offset:" << ullOffset << " file name:'" << sFileName
				<< "' file length:" << pFile->getLength() << endl;
		usage(argv[0]);
		return -4;
	}
	cout << "file name:" << sFileName << " offset:" << ullOffset
			<< " offset in hex:" << std::hex << ullOffset << std::dec << endl;
	AosBlobHeader* pHeader = OmnNew AosBlobHeader(0, ulHeaderCustomDataSize);
	if (!pHeader)
	{
		cerr << "failed to create AosBlobHeader obj, ulHeaderCustomDataSize:" << ulHeaderCustomDataSize << endl;
		return -5;
	}
	if (pHeader->getHeaderInCacheAndHeaderFileLength() > g_ulBuffLen)
	{
		g_ulBuffLen = pHeader->getHeaderInCacheAndHeaderFileLength();
	}
	AosBuff* pBuff = OmnNew AosBuff(g_ulBuffLen AosMemoryCheckerArgs);
	if (!pBuff)
	{
		cerr << "can not create buff obj" << endl;
		return -6;
	}
	int64_t illReadLen = pFile->readToBuff(ullOffset, g_ulBuffLen, pBuff->data());
	if (0 > illReadLen)
	{
		cerr << "can not read from file, ret=" << illReadLen << endl;
		return -7;
	}
	int iRet = 0;
	if (sFileType == "header")
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(pBuff->data());
		if (0 != iRet)
		{
			cerr << "pHeader->unserializeFromCacheAndHeaderFileBuff, iRet=" << iRet << endl;
			return -8;
		}
		pHeader->dump();
		pBuff->setDataLen(pHeader->getHeaderInCacheAndHeaderFileLength());
		pBuff->dumpData(true, "header data:'");
	}
	else if (sFileType == "body")
	{
		BodyEntryHead* p = (BodyEntryHead*)pBuff->data();
		if ((u32)illReadLen < p->ulLength)
		{
			illReadLen = pFile->readToBuff(ullOffset, p->ulLength, pBuff->data());
			if (0 > illReadLen)
			{
				cerr << "can not read from file, ret=" << illReadLen << endl;
				return -9;
			}
			p = (BodyEntryHead*)pBuff->data();
		}
		cout << "dumping body entry:" << endl;
		cout << "entry length: " << p->ulLength << endl
			 << "        type: " << (int)p->ucType << endl
			 << "      opr id: " << p->ullOprID << endl
			 << "       docid: " << p->ullDocid << endl
			 << "     snap id: " << p->ullSnapID << endl;
		iRet = pHeader->unserializeFromACLandHCLBuff(pBuff->data() + sizeof(BodyEntryHead));
		if (0 != iRet)
		{
			cerr << "pHeader->unserializeFromACLandHCLBuff, iRet=" << iRet << endl;
			return -10;
		}
		pHeader->dump();
		AosBuff* pTempBuff = OmnNew AosBuff(pBuff->data() + sizeof(BodyEntryHead),
				p->ulLength - sizeof(BodyEntryHead), p->ulLength - sizeof(BodyEntryHead) AosMemoryCheckerArgs);
		pTempBuff->dumpData(true, "body data:'");
	}
	else
	{
		std::cout << "Invalid file type." << endl;
		usage(argv[0]);
		return -99;
	}
	return 0;
}
