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
// This class manages key files. It uses a metafile to store the information
// about the key files it manages. The meta file has the following info:
//    	num_entries (int)
//    	[sdocid_block_id, file_id]
//    	[sdocid_block_id, file_id]
//    	...
//    	[sdocid_block_id, file_id]
//
// Each sdocid block maps to one key file. Each key file keeps a fixed number
// of keys. Collectively, it can store arbitrarily big key table.
//
// Modification History:
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StatServer/StatKeyConn.h"

#include "API/AosApi.h"
#include "StatServer/StatKeyFile.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Debug/Debug.h"



AosStatKeyConn::AosStatKeyConn(
		const u64 cube_id,
		const AosRlbFileType::E file_type)
:
mCubeId(cube_id),
mMetaFileID(0),
mRowsPerFile(0),
mFileType(file_type),
mKeyFileIDsArray(0),
mNumKeyFiles(0)
{
	initCounters();
}


AosStatKeyConn::~AosStatKeyConn()
{
}


bool
AosStatKeyConn::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	bool rslt;
	
	mRowsPerFile = conf->getAttrU32("rows_per_file", eDftRowsPerFile);

	mMetaFileID = conf->getAttrU64("meta_fileid", 0);
	aos_assert_r(mMetaFileID, false);
	
	mMetaFile = openFile(rdata.getPtr(), mMetaFileID);
	aos_assert_r(mMetaFile, false);

	if(mMetaFile->getLength() > eMetaFileStart)
	{
		rslt = readMetaFile(rdata.getPtr());
		aos_assert_r(rslt, false);	
	}
	else
	{
		mNumKeyFiles = 0;	
		mKeyFileIDsBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
		mKeyFileIDsArray = (u64 *)mKeyFileIDsBuff->data();
	}
	return true;
}


bool
AosStatKeyConn::readMetaFile(AosRundata *rdata)
{
	// 2. Key File ID Block
	//    This block is a map that maps sdocid block IDs to
	//    key file IDs. Its format is:
	//    	num_entries (int)
	//    	[sdocid_block_id, file_id]
	//    	[sdocid_block_id, file_id]
	//    	...
	//    	[sdocid_block_id, file_id]
	//

	u64 offset = eMetaFileStart;
	mNumKeyFiles = mMetaFile->readInt(offset, -1, rdata);
	aos_assert_rr(mNumKeyFiles >= 0, rdata, false);

	u32 size_to_read = (u32)mNumKeyFiles * eKeyFileEntrySize;
	mKeyFileIDsBuff = OmnNew AosBuff(size_to_read AosMemoryCheckerArgs);
	
	offset += sizeof(int);
	char *data = (char *)mKeyFileIDsBuff->data();
	bool rslt = mMetaFile->readToBuff(offset, size_to_read, data, rdata);
	aos_assert_rr(rslt, rdata, false);
	mKeyFileIDsBuff->setDataLen(size_to_read);
	mKeyFileIDsArray = (u64 *)mKeyFileIDsBuff->data();
		
	for(u32 i=0; i<mNumKeyFiles; i++)
	{
		mKeyFiles.push_back(0);
		mKeyFilesRaw.push_back(0);
	}

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatKeyConn::saveMetaFile(AosRundata *rdata)
{
	aos_assert_rr(mMetaFile, rdata, false);

	// Save 'mNumKeyFiles'
	u64 offset = eMetaFileStart;
	mMetaFile->setInt(offset, mNumKeyFiles, false, rdata);

	// Save 'mKeyFileIDsBuff'
	offset += sizeof(int);
	const char *data = mKeyFileIDsBuff->data();
	i64 data_len = mKeyFileIDsBuff->dataLen();
	aos_assert_rr(data_len == mNumKeyFiles * eKeyFileEntrySize, rdata, false);
	mMetaFile->write(offset, data, data_len, true);

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosStatKeyConn::saveKey(
		AosRundata *rdata, 
		const u64 sdocid, 
		const OmnString &key)
{
	AosStatKeyFile *keyfile = getKeyFile(rdata, sdocid);
	aos_assert_rr(keyfile, rdata, false);

	bool rslt = keyfile->appendKey(rdata, sdocid, key);
	aos_assert_r(rslt, false);
	
	OmnTagFuncInfo << endl;
	return true;
}


AosReliableFilePtr
AosStatKeyConn::openFile(
		AosRundata *rdata, 
		const u64 file_id)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rr(vfsMgr, rdata, 0);
	OmnTagFuncInfo << endl;
	return vfsMgr->openRlbFile(file_id, mFileType, rdata);
}

void
AosStatKeyConn::initCounters()
{
	mReadTime1 = 0;
	mReadTime2 = 0;
	mReadTime3 = 0;

	mReadNum1 = 0;
	mReadNum2 = 0;
	mReadNum3 = 0;
}

void
AosStatKeyConn::outputCounters()
{
	//output local counters
	OmnScreen << "(Statistics counters : StatKeyFile ) getKeyFile --- Time1 : "  
		            << mReadTime1 << ", Num: " << mReadNum1 << endl;

	OmnScreen << "(Statistics counters : StatKeyFile ) readKey --- Time2 : "  
					 << mReadTime2 << ", Num: " << mReadNum2 << endl;
	//output key file counters
	// Pay   2015-11-13
	// JIMODB-1186
	AosStatKeyFile *file ;
	for (u32 i = 0; i < mKeyFilesRaw.size(); i++)
	{
		file = mKeyFilesRaw[i];
		if(file)
			mKeyFilesRaw[i]->outputCounters();
		else
			OmnScreen << "The no." << i
				<< " file not read this time." << endl;
	}
}

OmnString
AosStatKeyConn::readKey(
		AosRundata *rdata, 
		const u64 sdocid)
{
	// Ketty 2014/09/02
	u64 tStart, tEnd;
	tStart = OmnGetTimestamp();
	AosStatKeyFile *keyfile = getKeyFile(rdata, sdocid);
	aos_assert_rr(keyfile, rdata, "");
	tEnd = OmnGetTimestamp();
	mReadTime1 += tEnd - tStart; 
	mReadNum1++;
	
	OmnString key;
	tStart = OmnGetTimestamp();
	bool rslt = keyfile->readKey(rdata, sdocid, key);
	tEnd = OmnGetTimestamp();
	mReadTime2 += tEnd - tStart; 
	mReadNum2++;

	aos_assert_r(rslt, "");
	OmnTagFuncInfo << endl;
	return key;
}


bool
AosStatKeyConn::readKeys(
		AosRundata *rdata, 
		const u64 sdocid, 
		const int num_records,
		AosBuffArrayVar *keys)
{
	// This function reads keys starting from 'sdocid' for 'num_records'.
	// Keys are saved in AosBuffArrayVar. 
	AosStatKeyFile *keyfile = getKeyFile(rdata, sdocid);
	if (!keyfile) 
	{
		// The requested key file does not exist.
		return true;
	}

	OmnNotImplementedYet;
	return false;
	//return keyfile->readKeys(rdata, sdocid, num_records, keys);
}


AosStatKeyFile *
AosStatKeyConn::getKeyFile(
		AosRundata *rdata, 
		const u64 sdocid)
{
	// Keys are saved in key files. Each key file keeps 'mRowsPerFile'
	// number of keys. This function retrieves the key file for
	// 'sdocid'. If the file has not been opened yet, it will open
	// it.
	u32 idx = sdocid / mRowsPerFile;

	AosStatKeyFilePtr ff;

	if (idx < mNumKeyFiles)
	{
		aos_assert_r(idx < mKeyFilesRaw.size(), 0);
		AosStatKeyFile *file = mKeyFilesRaw[idx];
		if (file) return file;

		u64 file_id = mKeyFileIDsArray[idx];
		aos_assert_r(file_id, 0);

		ff = OmnNew AosStatKeyFile(rdata, mCubeId, file_id,
			sdocid / mRowsPerFile * mRowsPerFile, mRowsPerFile,
			mFileType);

		mKeyFiles[idx] = ff;
		mKeyFilesRaw[idx] = ff.getPtr();
		return mKeyFilesRaw[idx];
	}

	// is new file.
	//aos_assert_rr(idx == mNumKeyFiles, rdata, 0);
#if 0
	ff = OmnNew AosStatKeyFile(rdata, mCubeId, 0,
			sdocid / mRowsPerFile * mRowsPerFile, mRowsPerFile,
			mFileType);

	mKeyFiles.push_back(ff);
	mKeyFilesRaw.push_back(ff.getPtr());

	mKeyFileIDsBuff->gotoEnd();
	mKeyFileIDsBuff->setU64(mKeyFilesRaw[idx]->getFileId());

	mNumKeyFiles++;

	bool rslt = saveMetaFile(rdata);
	aos_assert_rr(rslt, rdata, 0);
	OmnTagFuncInfo << endl;
#endif
	//arvin 2015.11.04
	//JIMODB-1075
	//this code is to preveny sdocid discontinuous results in an error,
	//this is not the final solution,we will solve this bug later
	u32 new_idx = mKeyFilesRaw.size();
    if((idx-new_idx) > 3)
	{
		OmnAlarm << "discontinuous sdocid :" << sdocid << "the previous sdocid" << mTestPrevSdocid << enderr;
	}
	mTestPrevSdocid = sdocid;
	while(new_idx <= idx)
	{
		ff = OmnNew AosStatKeyFile(rdata, mCubeId, 0,
				new_idx * mRowsPerFile, mRowsPerFile,
				mFileType);

		mKeyFiles.push_back(ff);
		mKeyFilesRaw.push_back(ff.getPtr());

		mKeyFileIDsBuff->gotoEnd();
		mKeyFileIDsBuff->setU64(mKeyFilesRaw[new_idx]->getFileId());

		mNumKeyFiles++;

		bool rslt = saveMetaFile(rdata);
		aos_assert_rr(rslt, rdata, 0);
		OmnTagFuncInfo << endl;
		new_idx++;

	}
	return mKeyFilesRaw[idx];
}

