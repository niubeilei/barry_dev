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
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TaskMgr/Task.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/CodeConvertion.h"
#include "VirtualFile/VirtualFile.h"
#include "Debug/Debug.h"
#include "NetFile/NetFile.h"
#include "MetaData/FileMetaData.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataCubeFile_0(
		const AosRundataPtr &rdata,
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataCubeFile(version);
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


AosDataCubeFile::AosDataCubeFile(
		const AosFileInfo &file_info,
		const int64_t readBlockSize,
		const AosXmlTagPtr &metadata)
:
mFileInfo(file_info),
mMetadata(metadata),
mReadBlockSize(readBlockSize), 
mIsSkipFirstLine(false),
mFlag(true)
{
	OmnTagFuncInfo << endl;
	
	mIsSkipFirstLine = mMetadata->getAttrBool("skip_first_line", false);
	AosFileReadListenerPtr thisptr(this, false);
	if (mFileInfo.mFileId != 0)
	{
		mFile = OmnNew AosIdNetFile(mFileInfo);
	}
	else if (mFileInfo.mFileName != "")
	{
		mFile = OmnNew AosNameNetFile(mFileInfo);
	}
	else if (!mFileInfo.mFileBlocks.empty())
	{
		mFile = OmnNew AosCompNetFile(mFileInfo);
	}
	aos_assert(mFile);
	mFile->setCaller(thisptr);
}



AosDataCubeFile::AosDataCubeFile(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_FILE, version),
mFlag(true)
{
}


AosDataCubeFile::~AosDataCubeFile()
{
}


bool
AosDataCubeFile::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	// 'worker_doc' serves as the metadata
	aos_assert_r(worker_doc, false);

	mMetadata = worker_doc->clone(AosMemoryCheckerArgsBegin);
	mReadBlockSize = worker_doc->getAttrInt64("read_block_size", eDftReadBlockSize);
	mIsSkipFirstLine = worker_doc->getAttrBool("skip_first_line", false);

	AosXmlTagPtr file_doc = worker_doc->getFirstChild("file");
	aos_assert_r(file_doc, false);

	mFileInfo.serializeFrom(file_doc);

	if (mFileInfo.mFileName == "")
	{
		if (mFileInfo.mFileId == 0)
		{
			AosSetErrorUser(rdata, "missing_fileidname_and_fileid") << mMetadata->toString() << enderr;
			return false;
		}
	}
		
	if (!AosIsValidPhysicalIdNorm(mFileInfo.mPhysicalId))
	{
		AosSetErrorUser(rdata, "invalid_physical_id") << mMetadata->toString() << enderr;
		return false;
	}

	if (mFileInfo.mFileName != "")
	{
		mFile = OmnNew AosNameNetFile(mFileInfo);
	}
	else if(mFileInfo.mFileId != 0)
	{
		mFile = OmnNew AosIdNetFile(mFileInfo);
	}
	else if (!mFileInfo.mFileBlocks.empty())
	{
		mFile = OmnNew AosCompNetFile(mFileInfo);
	}

	if (!mFile)
	{
		AosSetErrorUser(rdata, "faild_create_virtual_file,") << mFileInfo.mFileName << "," << mFileInfo.mFileId << enderr;
		return false;
	}

	if (mFileInfo.mFileLen <= 0) 
	{
		mFileInfo.mFileLen = mFile->length(rdata.getPtr());
	}

	AosFileReadListenerPtr thisptr(this, false);
	mFile->setCaller(thisptr);

	OmnTagFuncInfo << "read file data: " << mFileInfo.mFileName << endl;
	return true;
}


int 
AosDataCubeFile::getPhysicalId() const
{
	return mFileInfo.mPhysicalId;
}


int64_t
AosDataCubeFile::getFileLength() const
{
	return mFileInfo.mFileLen;
}


OmnString
AosDataCubeFile::getFileName() const
{
	return mFileInfo.mFileName;
}


AosDataConnectorObjPtr 
AosDataCubeFile::cloneDataConnector()
{
	return OmnNew AosDataCubeFile(*this);
}

void
AosDataCubeFile::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;	
}

bool
AosDataCubeFile::readData(
		const u64 reqid,
		AosRundata *rdata)
{
	aos_assert_rr(mReadBlockSize > 0, rdata, false);
	if (mFileInfo.mFileLen <= 0)
	{
		mFileInfo.mFileLen = mFile->length(rdata);
	}
	
	int64_t bytes_to_read = mReadBlockSize;
	aos_assert_r(mFile, false);
	OmnTagFuncInfo << endl;
	mFile->readData(reqid, bytes_to_read, rdata);
	return true;
}


void
AosDataCubeFile::fileReadCallBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished, 
		const AosDiskStat &disk_stat)
{
	if (disk_stat.getError() > 0)                                                           
	{
		OmnScreen << "=====================disk error1111111111111111 : " << endl;
		OmnString msg = " disk error.";
		//report(__FILE__, __LINE__, "read", msg);
		//mDiskError = true;                                                        
		AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, OmnApp::getRundata());
		aos_assert(task);
		task->setErrorType(AosTaskErrorType::eInPutError);
		task->taskFailed(0);
		return;
	}   

	AosBuffPtr buff = mFile->getBuff();  
	if (!buff)
	{
		OmnString msg = ", Alarm file info";
		msg << ", fileId: " << mFileInfo.mFileId 
			<< ", fileName: " << mFileInfo.mFileName
			<< ", physicalId: " << mFileInfo.mPhysicalId
			<< ", crtblockIdx: " << mFileInfo.mCrtBlockIdx
			<< ", start_offset: " << mFileInfo.mStartOffset
			<< ", reqId: " << reqId;
		//report(__FILE__, __LINE__, "read", msg);
	}
	aos_assert(buff);

	// modify by Young, 2015/04/07
	// some case need skip the first line(\n, \r\n)
	if (mIsSkipFirstLine && mFileInfo.mStartOffset == 0)
	{
		char *data = buff->data();
		i64 len  = buff->dataLen();
		i64 i = 0;
		for (; i<len; i++)
		{
			if (*data != '\n')
			{
				data++;
			}
			else
			{
				data++;
				break;
			}
		}

		i64 first_line_len = data - buff->data();
		AosBuffPtr newbuff = OmnNew AosBuff(data, len, len - first_line_len, true AosMemoryCheckerArgs);
		buff = newbuff;
	}

	u32 bytes_read = buff->dataLen();
	
	OmnString defaulttype = CodeConvertion::mDefaultType;
	if (mFileInfo.mCharset.toLower() != defaulttype.toLower())
	{
		//Jackie 2015/11/17 jimodb-1150
		i64 datalen = buff->dataLen();
		char * d = buff->data();
		int i=1;
		while(i<=3)
		{
			if(d[datalen-i]>127 || d[datalen-i]<0)
			{
				mFile->moveToFront(1);
				buff->setDataLen(datalen-i);
			}
			else
			{
				break;
			}
			i++;
		}
		int64_t len = buff->dataLen() * 2;
		AosBuffPtr newbuff = OmnNew AosBuff(len AosMemoryCheckerArgs);
		OmnString to_type = CodeConvertion::mDefaultType;
		int64_t newlen = CodeConvertion::convert(mFileInfo.mCharset.data(), to_type.data(),
				buff->data(), buff->dataLen(), newbuff->data(), len);
		aos_assert(newlen >= 0);
		newbuff->setDataLen(newlen);
		buff = newbuff;
	}
/*
	AosXmlTagPtr xml = mMetadata->clone(AosMemoryCheckerArgsBegin);
	xml->setAttr(AOSTAG_SOURCE_FNAME, mFileInfo.mFileName);
	xml->setAttr("source_fileid", mFileInfo.mFileId);
	xml->setAttr(AOSTAG_SOURCE_LENGTH, mFileInfo.mFileLen);
	xml->setAttr("start_pos", mFileInfo.mStartOffset);
	//xml->setAttr("data_len", data_len);
	xml->setAttr("data_len", bytes_read);

	if (mFileInfo.mFileName != "")
	{
		int idx = mFileInfo.mFileName.indexOf(0, '/', true);
		aos_assert(idx >= 0);
		OmnString name = mFileInfo.mFileName.substr(idx + 1);
		xml->setAttr(AOSTAG_SOURCE_NAME, name);
		OmnString path = mFileInfo.mFileName.substr(0, idx - 1);
		xml->setAttr(AOSTAG_SOURCE_PATH, path);
	}
*/

	AosMetaDataPtr meta;
	if (mFileInfo.mFileName != "") 
	{
		meta = OmnNew AosFileMetaData(mFileInfo.mFileName, mFileInfo.mStartOffset);
	}
 	else 
	{
		OmnString field = "";
		field << mFileInfo.mFileId;
		meta = OmnNew AosFileMetaData(field, mFileInfo.mStartOffset);
	}
	AosBuffDataPtr buff_data = OmnNew AosBuffData(meta, buff, disk_stat);
	buff_data->setOffset(mFileInfo.mStartOffset);
	
	bool isfinished = finished;
	//if (mFileInfo.mStartOffset >= mFileInfo.mFileLen)
	if (isfinished)
	{
OmnScreen << " ##### reqid : " << reqId << " mStartPos : " << mFileInfo.mStartOffset << " file_length : " << mFileInfo.mFileLen << endl;
		buff_data->setEOF();
		isfinished = true;
	}
	mFileInfo.mStartOffset += bytes_read;
	
	// Chen Ding, 2014/06/23
	OmnString str01, str02, str03;
	str01 << mFileInfo.mFileLen;
	str02 << mFileInfo.mStartOffset;
	str03 << bytes_read;
	// OmnScreen << "\n========================================" 
	// 		  << "\n== file name: " << mFileInfo.mFileName
	// 		  << "\n== file length: " << mFileInfo.mFileLen
	// 	  	  << "\n== total read block: " << mFileInfo.mFileLen
	// 	  	  << "\n== already read length: " << mFileInfo.mStartOffset
	// 		  << "\n== current read bytes: " << bytes_read
	// 		  << "\n== reqid : " << reqId
	// 		  << "\n========================================\n" << endl;

	OmnScreen << "\n========================================" 
			  << "\n== file id: " << mFileInfo.mFileId 
			  << "\n== file name: " << mFileInfo.mFileName
			  << "\n== file length: " << str01 
		  	  << "\n== total read block: " << str01 
		  	  << "\n== already read length: " << str02 
			  << "\n== current read bytes: " << str03 
			  << "\n== reqid : " << reqId
			  << "\n========================================\n" << endl;

	OmnString msg;
	msg << ", file id: " << mFileInfo.mFileId 
		<< ", file name: " << mFileInfo.mFileName
		<< ", file length: " << str01 
		<< ", already read length: " << str02 
		<< ", current read bytes: " << str03 
		<< ", reqid : " << reqId;
	//report(__FILE__, __LINE__, "read", msg);

	updateCounter("datacube_read", bytes_read);
	//for debugging purpose. Delete the file
	//if (bytes_read > 0 && mTask->isService())
	if (false)
	{
		OmnTagFuncInfo << "Delete temp files" << endl;
		bool svr_death;
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		AosNetFileCltObj::getObj()->deleteFileStatic(
			mFileInfo.mFileName, 0, svr_death, rdata.getPtr());
	}
	
	aos_assert(mCaller);
	mCaller->callBack(reqId, buff_data, isfinished);
}   


bool 
AosDataCubeFile::appendBlock(
		AosRundata *rdata, 
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeFile::writeBlock(
		AosRundata *rdata, 
		const int64_t pos,
		AosBuffDataPtr &buff_data)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeFile::copyData(
		AosRundata *rdata, 
		const OmnString &from_name,
		const OmnString &to_name)
{
	OmnShouldNeverComeHere;
	return false;
}

bool 
AosDataCubeFile::removeData(
		AosRundata *rdata, 
		const OmnString &name)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosDataCubeFile::serializeTo(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	buff->setOmnStr(mMetadata->toString());
	buff->setOmnStr(mFileInfo.mFileName);
	buff->setI64(mFileInfo.mFileId);
	buff->setI64(mFileInfo.mStartOffset);
	buff->setI64(mFileInfo.mFileLen);
	buff->setInt(mFileInfo.mPhysicalId);
	return true;
}


bool
AosDataCubeFile::serializeFrom(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	// mMetadata = AosXmlParser::parse(buff->getOmnStr("") AosMemoryCheckerArgs);
	// aos_assert_rr(mMetadata, rdata, false);
	mFileInfo.mFileName = buff->getOmnStr("");
//	aos_assert_rr(mFileInfo.mFileName != "", rdata, false);
	mFileInfo.mFileId = buff->getI64(0);
	mFileInfo.mStartOffset = buff->getI64(0);
	mFileInfo.mFileLen = buff->getI64(0);
	mFileInfo.mPhysicalId = buff->getInt(0);
	return true;
}


AosJimoPtr 
AosDataCubeFile::cloneJimo() const
{
	try
	{
		return OmnNew AosDataCubeFile(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}

AosDataConnectorObjPtr
AosDataCubeFile::nextDataConnector()
{
	if(mFlag)
	{
		mFlag = false;
		return this;	
	}
	return 0;
}
