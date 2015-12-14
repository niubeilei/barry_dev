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
// Modification History:
// 2013/11/27 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCube/Jimos/DataCubeHadoopFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "HadoopUtil/HadoopApi.h" 
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
#include "Util/DiskStat.h"
#include "MetaData/FileMetaData.h"
#include <signal.h>

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeHadoopFile_0(
 		const AosRundataPtr &rdata, 
 		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeHadoopFile(version);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDataCubeHadoopFile::AosDataCubeHadoopFile(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_HADOOP_FILE, version),
mFile(0),
mFS(0)
{
}


AosDataCubeHadoopFile::~AosDataCubeHadoopFile()
{
}


bool
AosDataCubeHadoopFile::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	
	aos_assert_rr(worker_doc, rdata, false);
	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);

	AosXmlTagPtr file_doc = worker_doc->getFirstChild("file");
	mIP = file_doc->getAttrStr("ip");
	mPort = file_doc->getAttrInt("port", 9000);
	aos_assert_rr(mIP!="", rdata, false);
	aos_assert_rr(mPort>0, rdata, false);
	mCharset = file_doc->getAttrStr("charaset");
	mBlockSize = file_doc->getAttrInt64("blocksize", 0);
	if(mBlockSize<=0)
	{
		mBlockSize = eMaxSize;
	}
	mCrtOffset = file_doc->getAttrInt("offset", 0);	

	mFileName = file_doc->getNodeText();
	if (mFileName == "")
	{
		AosSetErrorUser(rdata, "missing_filename") << worker_doc->toString() << enderr;
		return false;
	}
	try{

		if(!mFS)
		{
	 		signal(SIGPIPE, SIG_IGN);
			mFS = hdfsConnect(mIP.data(), mPort); 
			if (!mFS) {                                                            
				AosSetErrorUser(rdata, "Failed to connect to hdfs!\n : ") << enderr;
				return false;
			}
		}
		if(!mFile)
		{
	 		signal(SIGPIPE, SIG_IGN);
			mFile = hdfsOpenFile(mFS, mFileName.data(), O_RDONLY, mBlockSize, 0, 0);
			if (!mFile) {                                                          
				AosSetErrorUser(rdata, "Failed to open this file : ") << mFileName  << enderr;
				return false;
			}
		}
		mFileLength = AosGetHadoopFileLength(rdata.getPtr(), mFS, mFile);
		aos_assert_rr(mFileLength>0, rdata, false);
	}
	catch(...)
	{
		AosSetErrorUser(rdata, "Failed to connect hadoop!") << worker_doc->toString() << enderr;
		return false;
	}
	return true;
}

void
AosDataCubeHadoopFile::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;	
}

void
AosDataCubeHadoopFile::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		bool finish)
{
	mCaller->callBack(reqid, buff, finish);
}

AosDataConnectorObjPtr 
AosDataCubeHadoopFile::cloneDataConnector()
{
	return OmnNew AosDataCubeHadoopFile(*this);
}

AosJimoPtr 
AosDataCubeHadoopFile::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeHadoopFile(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

bool 
AosDataCubeHadoopFile::readData(const u64 reqid, AosRundata *rdata) 
{
	aos_assert_r(mFileLength > 0, false); 
	char * block = OmnNew char[mFileLength];
	AosDiskStat disk_stat;
	int num = AosReadHadoopFile(rdata, block, mFS, mFile, mCrtOffset, mBlockSize);
	aos_assert_r(num>0, false);
	AosBuffPtr buff = OmnNew AosBuff(mFileLength AosMemoryCheckerArgs);
	buff->setData1(block, num, true);
	AosMetaDataPtr meta = OmnNew AosFileMetaData(mFileName, mCrtOffset);
	AosBuffDataPtr buff_data = OmnNew AosBuffData(meta, buff, disk_stat);
	mCrtOffset += mBlockSize;                             
	bool finish = false;
	if(num!=mBlockSize)
	{
		finish = true;
		hdfsCloseFile(mFS, mFile);
		hdfsDisconnect(mFS);
		mFS = 0;
		mFile = 0;
	}

	callBack(reqid, buff_data, finish);
	return true;
}


#endif
