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
// 07/15/2012 Created by Xu Lei
// 08/24/2012 Modified By Brian Zhang
////////////////////////////////////////////////////////////////////////////
#if 0 
#include "VirtualFile/VirtualFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "aosUtil/Types.h"
#include "DataCacher/Ptrs.h"
#include "DataCacher/DataCacher.h"
#include "DbQuery/Query.h"
#include "SEInterfaces/DocClientObj.h"
#include "IILClient/IILClient.h"
#include "SEUtil/IILName.h"
#include "Porting/File.h"
#include "Porting/LongTypes.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocServerCb.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/FileDesc.h"
#include "Util/File.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/KeyedNode.h"
#include "UtilComm/Ptrs.h"
#include "WordMgr/WordMgr1.h"
#include <list>


const OmnString sgNeedCompress = "true";

AosVirtualFile::AosVirtualFile(
		const OmnString &objid, 
		AosRundata *rdata)
:
mLock(OmnNew OmnMutex()),
mOperation(eRead),
mObjid(objid)
{
	// This constructor maybe open a virtual file with contents
	// or open an empty virtual file.
	// The file use to read contents or append contents.
	init();
	if (!getFileDoc(rdata))		// no such an file , return .
	{
		AosSetErrorU(rdata, "No such a file.") << enderr;
		OmnThrowException(rdata->getErrmsg());
	}
	
	if (!config(mFileDoc, 0, rdata))
	{
		AosSetErrorU(rdata, "config xml error.") << enderr;
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	
	mCachedBuff = OmnNew AosBuff(mSegmentSize + 10,  AosMemoryCheckerArgsBegin);
	aos_assert(mCachedBuff);
}


AosVirtualFile::AosVirtualFile(
		const AosXmlTagPtr &def, 
		const AosXmlTagPtr &file_doc,
		AosRundata *rdata)
:
mLock(OmnNew OmnMutex()),
mOperation(eWrite)
{
	// This function creates a new virtual file based on 'def' or 'file_doc'.
	// If file_doc exist, do it based on file_doc.
	init();
	if (!config(def, file_doc, rdata))
	{
		AosSetErrorU(rdata, "config xml error.") << enderr;
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	
	if (!createNewFileDoc(file_doc, rdata))
	{
		AosSetErrorU(rdata, "create_file_failed") << enderr;
		OmnThrowException(rdata->getErrmsg());
		return;
	}

	if (!getFileDoc(rdata))		// no such an file , return .
	{
		AosSetErrorU(rdata, "no_such_file") << enderr;
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	
	mCachedBuff = OmnNew AosBuff(mSegmentSize+10, AosMemoryCheckerArgsBegin);
	aos_assert(mCachedBuff);
}


AosVirtualFile::~AosVirtualFile()
{

}

bool
AosVirtualFile::init()
{
	mRetrieveLoopIdx = -1;
	mRecordLen = 0;
	mIsGood = false;
	mFileDoc = 0;
	mSegmentSize = 0;
	mReplicationPolicy = eNoReplication;
	mSegmentType = eInvalidSegmentType;
	mSeparator = "";
	mNeedOrder = false;
	mIsPublic = true;
	mSegmentVect.clear();
	return true;
}


bool
AosVirtualFile::config(
		const AosXmlTagPtr &def, 
		const AosXmlTagPtr &file_doc,
		AosRundata *rdata)
{
	if (!def && !file_doc)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}
	
	if (file_doc) mObjid = file_doc->getAttrStr(AOSTAG_VIRTUALFILE_OBJID);
	if (mObjid == "") mObjid = def->getAttrStr(AOSTAG_VIRTUALFILE_OBJID);
	if (mObjid == "")
	{
		AosSetErrorU(rdata, "missing_objid") << ": " << def->toString() << enderr;
		return false;
	}

	if (def) mNeedOrder = def->getAttrBool(AOSTAG_SEGMENT_ORDER_MODE,false);
	if (file_doc) mNeedOrder = file_doc->getAttrBool(AOSTAG_SEGMENT_ORDER_MODE,false);

	if (def) mIsPublic = def->getAttrBool(AOSTAG_IS_PUBLIC,false);
	if (file_doc) mIsPublic = file_doc->getAttrBool(AOSTAG_IS_PUBLIC,false);

	// Retrieve the segment size.
	if (file_doc) mSegmentSize = file_doc->getAttrInt(AOSTAG_SEGMENT_SIZE, 0);
	if (mSegmentSize == 0) mSegmentSize = def->getAttrInt(AOSTAG_SEGMENT_SIZE, 0);
	if(mSegmentSize < eMinBlockSize) mSegmentSize = eMinBlockSize;
	if(mSegmentSize > eMaxBlockSize) mSegmentSize = eMaxBlockSize;
	
	if (def) mNeedCompress = def->getAttrBool(AOSTAG_NEED_COMPRESS,false);
	if (file_doc) mNeedCompress = file_doc->getAttrBool(AOSTAG_NEED_COMPRESS,false);
	if (mNeedCompress)
	{
		if (file_doc) mCompressType = file_doc->getAttrStr(AOSTAG_COMPRESS_TYPE);
		if (mCompressType == "") mCompressType = def->getAttrStr(AOSTAG_COMPRESS_TYPE);
		if (mCompressType == "")
		{
			AosSetErrorU(rdata, "missing_compress type") << ": " << def->toString() << enderr;
			return false;
		}
	}
	
	if (!retrieveSegmentType(def, file_doc, rdata))
	{
		AosSetErrorU(rdata, "failed_to_retrieve_segment_type") << ": " << def->toString() << enderr;
		return false;
	}

	if (!retrieveReplicationPolicy(def, file_doc, rdata))
	{
		AosSetErrorU(rdata, "failed_to_retrieve_policy") << ": " << def->toString() << enderr;
		return false;
	}
	
	mIsGood = true;
	return true;
}


bool
AosVirtualFile::retrieveReplicationPolicy(
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &file_doc,
		AosRundata *rdata)
{
	// This function retrieves the replication policy from 'tag'. 
	// It should be called by config(...).
	OmnString ss;
	if (file_doc) ss = file_doc->getAttrStr(AOSTAG_REPLICATION_POLICY);
	if (ss == "") ss = def->getAttrStr(AOSTAG_REPLICATION_POLICY);

	if (ss == AOSREPLICATION_POLICY_TWO_COPIES)
	{
		mReplicationPolicy = eReplicationPolicy_Two;
	}
	else if (ss == AOSREPLICATION_POLICY_THREE_COPIES)
	{
		mReplicationPolicy = eReplicationPolicy_Three;
	}
	else if (ss == "" || AOSREPLICATION_POLICY_NO_REP)
	{
		mReplicationPolicy = eNoReplication;
	}
	else 
	{
		AosSetErrorU(rdata, "invalid_rep_policy") << ": " << def->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosVirtualFile::retrieveSegmentType(
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &file_doc,
		AosRundata *rdata)
{
	// This function retrieves the segment type. It should be called
	// by 'config(...)'.
	if (!def && !file_doc)
	{
		AosSetErrorU(rdata, "invalid_configuration") << ": " << def->toString() << enderr;
		return false;
	}

	OmnString segmentType;
	if (file_doc) segmentType = file_doc->getAttrStr(AOSTAG_SEGMENTTYPE);
	if (segmentType == "") segmentType = def->getAttrStr(AOSTAG_SEGMENTTYPE);
	if (segmentType == "")
	{
		AosSetErrorU(rdata, "Invalid segement type!") << ": " << def->toString() << enderr;
		return false;
	}

	if (segmentType == AOSSEGMENTTYPE_BYLINEDELIMITRT)
	{
		AosXmlTagPtr septag = def->getFirstChild(AOSTAG_SEPARATOR);
		if (!septag)
		{
			AosSetErrorU(rdata, "no separator. error") << ": " << mSeparator << enderr;
			return false;	
		}
		mSeparator = septag->getNodeText();
		if (mSeparator=="")
		{
			AosSetErrorU(rdata, "no separator. error") << ": " << mSeparator << enderr;
			return false;	
		}
		if (mSeparator.length() > eMaxDelimiterLength)
		{
			AosSetErrorU(rdata, "bad separator. error") << ": " << mSeparator << enderr;
			return false;
		}
		mSegmentType = eSegmentType_ByLineDelimiter;
		return true;
	}

	if (segmentType == AOSSEGMENTTYPE_BYSEGMENT_SIZE)
	{
		if (mSegmentSize <= 0)
		{
			AosSetErrorU(rdata, "invalid_segment_size") << ": " << def->toString() << enderr;
			return false;
		}
		mSegmentType = eSegmentType_BySegmentSize;
		return true;
	}

	if (segmentType == AOSSEGMENTTYPE_BYRECORDLEN)
	{
		mRecordLen = def->getAttrInt(AOSTAG_RECORD_LEN, 0);
		if (mRecordLen <= 0)
		{
			AosSetErrorU(rdata, "record_len_error") << enderr;
			return false;
		}
		if(mSegmentSize <= 0)
		{
			AosSetErrorU(rdata, "segment size error") << enderr;
			return false;
		}
		mSegmentType = eSegmentType_ByRecordLen;
		return true;
	}
	
	AosSetErrorU(rdata, "invalid_segment_type") << ": " << def->toString() << enderr;
	return false;
}


bool
AosVirtualFile::createEmptyFile(AosRundata *rdata)
{
	return createNewFileDoc(0, rdata);
}

void
AosVirtualFile::changeMode()
{
	aos_assert(mIsGood);
	if (mOperation == eWrite)
	{
		mOperation = eRead;
	}
	else
	{
		mOperation = eWrite;
	}
}

bool
AosVirtualFile::deleteFromDisk(AosRundata *rdata)
{
	// This function is use to delete a virtual file from the disk.
	// It should delete:
	// 	1. All the segment docs
	// 	2. Virtual File Doc
	// 	3. The Container for the virtual file
	// 	4. The entry from composeVirtualFileSizeIILName()
	// 	5. The entry from composeVirtualFileSeqnoIILName()
	aos_assert_r(mFileDoc, false);
	aos_assert_r(mIsGood, false);
	int start_pos = 0;
	const u32 psize = 100000;
	bool finished = false;

	// 1. Delete all the segment docs. Segment doc objids are listed in the
	//    container objid listing. It retrieves the contents from the IIL
	//    and deletes all the docs one by one.
	while (!finished)
	{
		vector<u64> docids;
		AosQuery::getSelf()->retrieveContainerDocidsMembers(
				getContainerObjid(), docids, psize, start_pos, true, rdata); 
		if (docids.size() < psize) finished = true;
		start_pos += psize;

		int numdelete =0;
		OmnString segError= "";
	
		for(u32 i =0; i < docids.size(); i++)
		{
			bool deletedoc = AosDeleteBinaryDoc(docids[i], rdata);
			if (deletedoc)
			{
				numdelete++;
			}
			else
			{
				saveErrorLog("delete segment doc error", docids[i], rdata);
			}
		}
	}

	// 2. Delete file doc
	bool rslt = AosDocClientObj::getDocClient()->deleteDoc(mFileDoc, rdata);
	if (!rslt)
	{
		saveErrorLog("delete file doc error", 0, rdata);
		return false;
	}

	// 3. Delete container doc
	OmnString ctnr_objid = getContainerObjid();
	OmnString objid = getContainerObjid();
	aos_assert_r(ctnr_objid != "" && objid != "", false);
	rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, "", objid, ctnr_objid, false);
	if (!rslt)
	{
		saveErrorLog("delete file doc error", 0, rdata);
		return false;
	}

	// 	4. The entry from composeVirtualFileSizeIILName()
	u64 docid;
	bool isunique;
	OmnString iilname = AosIILName::composeVirtualFileSizeIILName();
	aos_assert_r(iilname != "", false);
	rslt = AosIILClient::getSelf()->getDocid(iilname, mObjid, docid, isunique, rdata);
	aos_assert_r(rslt && docid > 0, false);
	rslt = AosIILClient::getSelf()->removeValueDoc(iilname, mObjid, docid, rdata);
	if (!rslt)
	{
		saveErrorLog("delete file size iil failed", 0, rdata);
		return false;
	}
	
	// 	5. The entry from composeVirtualFileSeqnoIILName()
	iilname = AosIILName::composeVirtualFileSizeIILName();
	aos_assert_r(iilname != "", false);
	rslt = AosIILClient::getSelf()->getDocid(iilname, mObjid, docid, isunique, rdata);
	aos_assert_r(rslt && docid > 0, false);
	rslt = AosIILClient::getSelf()->removeValueDoc(iilname, mObjid, docid, rdata);
	if (!rslt)
	{
		saveErrorLog("delete file size iil failed", 0, rdata);
		return false;
	}

	mIsGood = false;
	return true;
}


AosXmlTagPtr
AosVirtualFile::getFileDoc(AosRundata *rdata)
{
	// This funtction retrieves the doc for this virtual file (identified by mObjid)
	aos_assert_rr(mObjid != "", rdata, 0);
	mFileDoc = AosDocClientObj::getDocClient()->getDocByObjid(mObjid, rdata);
	aos_assert_rr(mFileDoc, rdata, 0);
	
	return mFileDoc;
}


bool
AosVirtualFile::fileExist(const OmnString &objid, AosRundata *rdata)
{
	aos_assert_rr(objid=="", rdata, false);
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if (!doc) return false;
	return true; 
}


u64 
AosVirtualFile::getFileLength(AosRundata *rdata) const
{
	aos_assert_r(mFileDoc, 0);
	aos_assert_r(mIsGood, 0);
	u64 file_length;
	bool rslt = AosIILClient::getSelf()->getDocid(
		AosIILName::composeVirtualFileSizeIILName(), mObjid, file_length, rdata);
	aos_assert_r(rslt, 0);
	return file_length;
}


bool
AosVirtualFile::close()
{
	mIsGood = false;
	mFileDoc = 0;
	return true;
}


bool
AosVirtualFile::readSegment(
		const u64 &segment_id, 
		AosBuffPtr &buff, 
		AosRundata *rdata)
{
	// This function reads in the segment 'segment_id'. 
	// Note: the segment id begin with 0, not 1. Only ordered
	// virtual files can read segment by segment id. 
	if (mOperation != eRead)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		return false;
	}
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mNeedOrder, rdata, false);
	aos_assert_rr(mFileDoc, rdata, false);

	OmnString segment_objid = composeSegmentObjid(segment_id);
	aos_assert_rr(segment_objid != "", rdata, false);

	AosXmlTagPtr doc = AosRetrieveBinaryDoc(segment_objid, buff, rdata);
	if (!doc) return false;
	return true;
}


vector<u64> &
AosVirtualFile::getAllSegment(AosRundata *rdata)
{
	// This function will retrieve all docids in the segment container, and return the vector.
	if ((mOperation != eRead) && mIsGood)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		mSegmentVect.clear();
		return mSegmentVect;
	}
		
	const u32 psize = 100000;
	u32 start_pos = 0;
	bool finished = false;
	while (!finished)
	{
		vector<u64> docids;
		bool rslt = AosQuery::getSelf()->retrieveContainerDocidsMembers(
				getContainerObjid(), docids, psize, start_pos, true, rdata); 
		if (!rslt)
		{
			AosSetErrorU(rdata, "operation_wrong") << enderr;
			mSegmentVect.clear();
			return mSegmentVect;
		}

		if (docids.size() < psize) finished = true;
		for (u32 i=0; i<docids.size(); i++)
		{
			mSegmentVect.push_back(docids[i]);
		}
		start_pos += docids.size();

	}

	return mSegmentVect;
}


bool
AosVirtualFile::getFirstSegment(
		AosBuffPtr &buff, 
		AosRundata *rdata)
{
	// This function reads in the first segment in the virtual file. 
	// Segments are listed by the container's objid IIL. This function
	// will retrieve the objids first, and then reads in the first one.
	if (mOperation != eRead)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		return false;
	}
	aos_assert_rr(mIsGood, rdata, false);
	aos_assert_rr(mFileDoc, rdata, false);
	
	mRetrieveLoopIdx = 0;
	mSegmentVect.clear();
	
	const u32 psize = 100000;
	u32 start_pos = 0;
	bool finished = false;

	// Reads in all the objids into 'mSegmentVect'. If there are too many, 
	// it will keep on reading until all objids are retrieved. 
	//
	// NOTE: There is a potential problem for this function: if there are
	// too many objids for a virtual file, it may blow up the memory. 
	// In future improvements, we will retrieve just one page. When the
	// current page runs out, it checks whether there are more (there are
	// more if the number of objids is less than the given page size).
	while (!finished)
	{
		vector<u64> docids;
		bool rslt = AosQuery::getSelf()->retrieveContainerDocidsMembers(
				getContainerObjid(), docids, psize, start_pos, true, rdata); 
		aos_assert_r(rslt, false);

		if (docids.size() < psize) finished = true;
		for (u32 i=0; i<docids.size(); i++)
		{
			mSegmentVect.push_back(docids[i]);
		}
		start_pos += docids.size();

	}
	
	AosXmlTagPtr doc = AosRetrieveBinaryDoc(mSegmentVect[0], buff, rdata);
	if (!doc) return false;
	mRetrieveLoopIdx++;
	return true;
}


bool
AosVirtualFile::nextSegment(
		AosBuffPtr &buff, 
		bool &finished,
		AosRundata *rdata)
{
	if (mOperation != eRead)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		return false;
	}
	aos_assert_r(mIsGood, false);

	aos_assert_r(mRetrieveLoopIdx > 0, false);
	
	if (mRetrieveLoopIdx >= mSegmentVect.size())
	{
		buff = 0;
		finished = true;
		return true;
	}

	aos_assert_rr(mSegmentVect[mRetrieveLoopIdx] > 0, rdata, false);
	AosXmlTagPtr doc = AosRetrieveBinaryDoc(mSegmentVect[mRetrieveLoopIdx], buff, rdata);
	if (!doc) 
	{
		AosSetErrorU(rdata, "No doc") << ": " << mSeparator << enderr;
		return false;
	}
	
	mRetrieveLoopIdx++;
	finished = false;
	return true;
}


bool
AosVirtualFile::exportFile(
		u64 &file_len,
		const OmnString &fileName, 
		AosRundata *rdata)
{
	// This function exports a virture file to a local file. 
	aos_assert_r(mIsGood, false);
	
	OmnFilePtr file = OmnNew OmnFile(fileName, OmnFile::eCreate AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		AosSetErrorU(rdata, "failed_create_file") << ": " << fileName << enderr;
		return false;
	}
	
	if (mSegmentSize <= 0)
	{
		file->closeFile();
		AosSetErrorU(rdata, "failed_create_file") << ": " << fileName << enderr;
		return false;
	}
	
	const u32 psize = 100000;
	u32 start_pos = 0;
	bool finished = false;
	while (!finished)
	{
		vector<u64> docids;
		bool rslt = AosQuery::getSelf()->retrieveContainerDocidsMembers(
				getContainerObjid(), docids, psize, start_pos, true, rdata); 
		aos_assert_rf(rslt, file, false);

		if (docids.size() < psize) finished = true;
		
		for (u32 i=0; i<docids.size(); i++)
		{
			AosBuffPtr buff = OmnNew AosBuff(mSegmentSize + 10, AosMemoryCheckerArgsBegin);
			aos_assert_rf(buff, file, false);
			AosXmlTagPtr bdoc = AosDocClientObj::getDocClient()->retrieveBinaryDoc(
					docids[i], buff, rdata);
			aos_assert_rf(bdoc, file, false);
			file->append(buff->data(), buff->dataLen(), false);
		}
		start_pos += docids.size();
	}
	file->flushFileContents();
	file_len = file->getLength();
	file->closeFile();
	return true;
}


bool
AosVirtualFile::importFile(
		const OmnString &filename,
		AosRundata *rdata)
{
	// This function will create a virtual file based on an existing file. 
	// It does the following:
	// 1. Create the Virtual File Descriptor (VFD)
	// 2. Split the file as needed
	// 3. Create all the segments
	// this function can create a virtfile , will return false if faild .
	//  will try to use objid will return objid .
	//  other file infomation use the creator xml .
	if (mOperation != eWrite)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		return false;
	}
	aos_assert_r(mIsGood, false);
	aos_assert_r(mFileDoc, false);

	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		file->closeFile();
		AosSetErrorU(rdata, "no_file_or_bad") << enderr;
		return false;
	}
	
	u64 filesize = file->getFileCrtSize();
	if (filesize <= 0)
	{
		file->closeFile();
		AosSetErrorU(rdata, "empty_file") << enderr;
		return false;
	}

	// 1. eSegmentType_ByRecordLen and mSegmentSize is the same
	switch (mSegmentType)
	{
	case eSegmentType_ByRecordLen: 
		 return importFileByRecord(mRecordLen, file, rdata);

	case eSegmentType_BySegmentSize:
		 return importFileBySize(mSegmentSize, file, rdata);

	case eSegmentType_ByLineDelimiter:
		 return importFileByDelimiter(mSeparator, file, rdata);

	default:
		 AosSetErrorU(rdata, "segment type error") << enderr;
		 file->closeFile();
		 return false;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool
AosVirtualFile::importFileByRecord(
		const int &record_len,
		const OmnFilePtr &file, 
		AosRundata *rdata)
{
	// This function import file into virtual file.
	// When the file is too big, we will split it by
	// mSegmentSize.
	aos_assert_rf(record_len > 0, file, false);
	
	u64 startPos = 0;
	u64 total_length = 0;
	int loop = mSegmentSize / record_len + 1;
	u64 size = loop * record_len;	
	while (true)
	{
		AosBuffPtr buff = OmnNew AosBuff(size + 10, AosMemoryCheckerArgsBegin);
		aos_assert_rf(buff, file, false);
		
		int64_t readsize = file->readToBuff(startPos, size, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		bool rslt = createSegment(-1, buff, rdata);
		aos_assert_rf(rslt, file, false);
		startPos += readsize;
		total_length += readsize;
		if (readsize < (int64_t)size) break;
	}
	
	if (total_length > 0)
	{
		bool rslt = incrementFileSize(total_length, rdata);
		if(!rslt)
		{
			AosSetErrorU(rdata, "increment segment length error") << enderr;
			file->closeFile();
			return false;
		}
	}

	file->closeFile();
	return true;
}


bool
AosVirtualFile::importFileBySize(
		const u64 &segment_size,
		const OmnFilePtr &file, 
		AosRundata *rdata)
{
	// This function reads in 'mSegmentSize' size of contents, and creates
	// a new segment. 
	u64 startpos = 0;
	u64 total_length = 0;
	while (true)
	{
		AosBuffPtr buff = OmnNew AosBuff(segment_size+10, AosMemoryCheckerArgsBegin);
		aos_assert_rf(buff, file, false);
		int64_t readsize = file->readToBuff(startpos, segment_size, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		bool rslt = createSegment(-1, buff, rdata);
		aos_assert_rf(rslt, file, false);
		startpos += readsize;
		total_length += readsize;
		if (readsize < (int64_t)segment_size) break;
	}

	if (total_length > 0)
	{
		bool rslt = incrementFileSize(total_length, rdata);
		if(!rslt)
		{
			AosSetErrorU(rdata, "increment segment length error") << enderr;
			file->closeFile();
			return false;
		}
	}

	file->closeFile();
	return true;
}


bool
AosVirtualFile::importFileByDelimiter(
		const OmnString &separator,
		const OmnFilePtr &file, 
		AosRundata *rdata)
{
	aos_assert_r(file, false);
	aos_assert_rf(mSegmentSize > 0, file, false);
	
	u64 startpos = 0;
	u64 total_length = 0;
	while(true)
	{  
		// In this condition, all  the file has read. go out this while .
		AosBuffPtr buff = OmnNew AosBuff(mSegmentSize+10, AosMemoryCheckerArgsBegin);
		aos_assert_rf(buff, file, false);
		int64_t readsize = file->readToBuff(startpos, mSegmentSize, buff->data());
		if (readsize <= 0) break;
		buff->setDataLen(readsize);
		u64 len = countLastSeparator(buff, separator, rdata);
		aos_assert_rf(len >=0 && (int64_t)len <= readsize, file, false);
		buff->setDataLen(len);
		bool rslt = createSegment(-1, buff, rdata);
		aos_assert_rf(rslt, file, false);
		startpos += len;
		total_length += len;
		if (readsize < (int64_t)mSegmentSize) break;
	}

	if (total_length > 0)
	{
		bool rslt = incrementFileSize(total_length, rdata);
		if(!rslt)
		{
			AosSetErrorU(rdata, "increment segment length error") << enderr;
			file->closeFile();
			return false;
		}
	}
	
	file->closeFile();
	return true;
} 

	
OmnString
AosVirtualFile::getSegmentObjid(AosRundata *rdata) const
{
	aos_assert_r(mObjid != "", "");
	u64 seqno;
	bool rslt = AosIILClient::getSelf()->incrementDocid(
		AosIILName::composeVirtualFileSeqnoIILName(), 
		mObjid, seqno, true, 1, 1, true, rdata);
	aos_assert_r(rslt, "");
	aos_assert_r(seqno >= 0, "");
	OmnString str = AOSZTG_SEGMENT_OBJID;
	str << "_" << mObjid << "_" << seqno << "_" << (int)mReplicationPolicy;
	return str;
}


OmnString
AosVirtualFile::composeSegmentObjid(const u64 &seqno)
{
	aos_assert_r(mNeedOrder, "");
	aos_assert_r(mObjid != "", "");
	aos_assert_r(seqno >= 0, "");
	OmnString str = AOSZTG_SEGMENT_OBJID;
	str << "_" << mObjid << "_" << seqno << "_" << (int)mReplicationPolicy;
	return str;
}

bool
AosVirtualFile::createSegmentToAssignVir(
		const int &vid,
		const AosBuffPtr &buff, 
		AosRundata *rdata)
{
	aos_assert_r(mIsGood, false);
	aos_assert_r(vid > 0, false);
	aos_assert_r((u64)buff->dataLen() == mSegmentSize, false);
	bool rslt = createSegment(vid, buff, rdata);
	if (!rslt)
	{
		saveErrorLog("create segment binary doc error", 0, rdata);
		return false;
	}
	return true;
}

bool
AosVirtualFile::createSegment(
		const int &vid,
		const AosBuffPtr &buff, 
		AosRundata *rdata)
{
	// This function creates a new segment. A segment is a binary doc.
	OmnString xml = "";
	xml << "<filesegment " << AOSTAG_VIRTUALFILE_OBJID << "=\""<< mObjid <<"\" "
		<< AOSTAG_STYPE << "=\"" << AOSTAG_DOC_SEGMENT << "\" "
		<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_BINARYDOC << "\" "
		<< AOSTAG_BINARY_NODENAME << "=\"" << AOSTAG_VFBNAME << "\" "
		<< AOSTAG_PARENTC << "=\"" << getContainerObjid() << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"true\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	
	if (mNeedOrder)
	{
		OmnString objid = getSegmentObjid(rdata);
		xml << AOSTAG_OBJID << "=\"" << objid << "\" ";
	}
	xml << "/>";

	AosXmlTagPtr doc = AosCreateBinaryDoc(vid, xml, mIsPublic, buff, rdata);
	if (!doc)
	{
		saveErrorLog("create segment binary doc error", 0, rdata);
		return false;
	}

	return true;
}


OmnString
AosVirtualFile::getContainerObjid() const
{
	// Each virtual file has a container for its segment docs. This function
	// creates the container objid.
	aos_assert_r(mIsGood, "");
	aos_assert_r(mFileDoc, "");
	OmnString objid = AOSZTG_VIRTUALFILE_SEGMENT_CTNR;
	objid << "_" << mFileDoc->getAttrU64(AOSTAG_DOCID, 0);
	return objid;
}


OmnString
AosVirtualFile::getVFContainerName(const bool ispublic, AosRundata *rdata)
{
	OmnString name = AOSZTG_VIRTUALFILE_CTNR;
	if(!ispublic) 
	{
		name << "_" << rdata->getCid();
	}
	else 
	{
		name << "_public";
	}
	return name;
}


bool
AosVirtualFile::createNewFileDoc(
		const AosXmlTagPtr &file_doc, 
		AosRundata *rdata)
{
	// This function creates a new doc for a new file. The doc should have not
	// been created yet. 
	mFileDoc = file_doc;
	if (!mFileDoc)
	{
		OmnString ss = "<virtualfile/>";
		mFileDoc = AosXmlParser::parse(ss AosMemoryCheckerArgs);
		aos_assert_r(mFileDoc, false);
	}
	
	mFileDoc->setAttr(AOSTAG_PUBLIC_DOC, "true");
	mFileDoc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	mFileDoc->setAttr(AOSTAG_OTYPE, AOSTAG_VIRTUAL_FILE);
	mFileDoc->setAttr(AOSTAG_OBJID, mObjid);
	mFileDoc->setAttr(AOSTAG_VIRTUALFILE_OBJID, mObjid);
	mFileDoc->setAttr(AOSTAG_SITEID,rdata->getSiteid());
	mFileDoc->setAttr(AOSTAG_SEGMENT_SIZE, mSegmentSize);
	mFileDoc->setAttr(AOSTAG_SEGMENTTYPE, SegmentTypeEnumToString(mSegmentType));
	mFileDoc->setAttr(AOSTAG_REPLICATION_POLICY, mReplicationPolicy);
	mFileDoc->setAttr(AOSTAG_SEGMENT_ORDER_MODE, mNeedOrder ? "true" : "false");
	mFileDoc->setAttr(AOSTAG_IS_PUBLIC, mIsPublic ? "true" : "false");
	mFileDoc->setAttr(AOSTAG_PARENTC, getVFContainerName(mIsPublic, rdata)); 
	if (mNeedCompress)
	{
		mFileDoc->setAttr(AOSTAG_NEED_COMPRESS, "true");
		mFileDoc->setAttr(AOSTAG_COMPRESS_TYPE, mCompressType);
	}

	switch(mSegmentType)
	{
	case eSegmentType_BySegmentSize:
		break;
	
	case eSegmentType_ByRecordLen:
		{
			mFileDoc->setAttr(AOSTAG_RECORD_LEN, mRecordLen);
		}
		break;
	
	case eSegmentType_ByLineDelimiter:
		{
			OmnString xml;
			xml << "<" << AOSTAG_SEPARATOR << "><![CDATA[" << mSeparator 
				<< "]]></" << AOSTAG_SEPARATOR << ">";
			mFileDoc->addNode1(AOSTAG_SEPARATOR, xml);
		}
		break;
	
	default:
		{
			OmnAlarm << "Invalid segment type: " << mSegmentType << enderr;
			return false;
		}
	}

	rdata->setReceivedDoc(mFileDoc,true);
	mFileDoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata,              
		        mFileDoc, "", "", mIsPublic, true, false, false, true, true, true);
	aos_assert_r(mFileDoc, false);
	mObjid = mFileDoc->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_r(mObjid != "", false);

	return createMemberContainer(rdata);
}


bool
AosVirtualFile::createMemberContainer(AosRundata *rdata)
{
	// Each virtual file has a 'Member Container'. This is the 
	// container that manages all its segment docs (i.e., 
	// all segment docs' parent container should be this container).
	aos_assert_rr(mFileDoc, rdata, false);
	OmnString xml= "";
	xml << "<vfcontainer ";
	xml << AOSTAG_PUBLIC_DOC << "=\"true\" ";
	xml << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	xml	<< AOSTAG_OTYPE << "=\"" << AOSTAG_CONTAINER << "\" ";
	xml << AOSTAG_STYPE << "=\"" << AOSTAG_VIRTUAL_FILE_MEM_CTNR << "\" ";
	xml	<< AOSTAG_OBJID << "=\"" << getContainerObjid() << "\" ";
	xml	<< AOSTAG_SITEID << "=\"" << rdata->getSiteid() << "\" ";
	
	if (mNeedCompress)
	{
		xml	<< AOSTAG_COMPRESS_ALG << "=\"" << mCompressType << "\" ";
	}
	xml << AOSTAG_PARENTC << "=\"" << getContainerObjid() << "\"/>";
	
	AosXmlTagPtr ctnrdoc = AosXmlParser::parse(xml AosMemoryCheckerArgs);
	aos_assert_r(ctnrdoc, false);
	rdata->setReceivedDoc(ctnrdoc,true);
	ctnrdoc = AosDocClientObj::getDocClient()->createDocSafe3(rdata, 
			ctnrdoc, "", "", mIsPublic, true, false, false, true, true, true);
	aos_assert_r(ctnrdoc, false);
	
	return true;
}
	

bool
AosVirtualFile::appendData(const AosBuffPtr &buff, AosRundata *rdata)
{
	// this function can append data to a virt file .
	// will add the data to the last segment , if the segment is 
	// bigger than the segment size ,it will add a new segment ,
	// The contents of buff should be a contents of a record.
	if (mOperation != eWrite)
	{
		AosSetErrorU(rdata, "operation_wrong") << enderr;
		return false;
	}
	aos_assert_rr(mFileDoc, rdata, false);
	aos_assert_r(mIsGood, false);
	aos_assert_rr(mSegmentSize > 0, rdata, false);
	
	mLock->lock();
	u64 csize = mCachedBuff->dataLen();
	aos_assert_rl(csize >= 0 && csize < mSegmentSize, mLock, false);
	
	u64 buffsize = buff->dataLen();
	aos_assert_rl(buffsize > 0, mLock, false);
	
	mCachedBuff->addBuff(buff);
	
	if (csize + buffsize < mSegmentSize)
	{
		mLock->unlock();
		return true;
	}
	
	bool rslt = createSegment(-1, mCachedBuff, rdata);
	aos_assert_rl(rslt, mLock, false);
	
	rslt = incrementFileSize(mCachedBuff->dataLen(), rdata);
	aos_assert_rl(rslt, mLock, false);

	mCachedBuff->resetMemory(mSegmentSize + 10);
	mCachedBuff->reset();

	mLock->unlock();
	return true;
}


bool
AosVirtualFile::serializeFrom(const AosBuffPtr &buff, AosRundata *rdata)
{
	aos_assert_r(mObjid == "", -1);
	aos_assert_rr(buff, rdata, false);
	mObjid = buff->getOmnStr("mObjid");
	// how to get other members ?
	return true;
}


bool
AosVirtualFile::serializeTo(const AosBuffPtr &buff, AosRundata *rdata)
{
	aos_assert_r(mObjid == "", -1);
	aos_assert_rr(buff, rdata, false);
	
	buff->setOmnStr("mObjid");
	//buff->setBool(mIsGood);
	// what kind of member need to save ????
	return true;
}


bool
AosVirtualFile::incrementFileSize(
		const u64 &size,
		AosRundata *rdata)
{
	u64 value = 0;
	bool rslt = AosIncrementKeyedValue(AosIILName::composeVirtualFileSizeIILName(), mObjid, value, true, size, 0, true, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosVirtualFile::saveErrorLog(
		const OmnString &error, 
		const u64 &docid, 
		AosRundata *rdata)
{
	OmnString xml = "<vferror zky_public_doc=\"true\" zky_public_ctnr=\"true\" ";
	xml << " errormsg=\"" << error << "\" " ;
	xml << " vfobjid=\"" << mObjid << "\" " ;
	xml << " vfdocid=\"" << mFileDoc->getAttrU64(AOSTAG_DOCID, 0) << "\" " ;
	xml << " segment_docid=\"" << docid << "\" " ;
	xml << " />";
	AosXmlTagPtr doc = AosXmlParser::parse(xml AosMemoryCheckerArgs);
	aos_assert_r(doc, false);
	//bool rslt = AosCreateDoc(doc, rdata);
	//aos_assert_r(rslt, false);
	return true;
}


OmnString
AosVirtualFile::SegmentTypeEnumToString(const SegmentType &type)
{
	OmnString str = "";
	if(type == eInvalidSegmentType)					str = "error";
	else if(type == eSegmentType_BySegmentSize)		str = AOSSEGMENTTYPE_BYSEGMENT_SIZE;
	else if(type == eSegmentType_ByRecordLen)		str = AOSSEGMENTTYPE_BYRECORDLEN;
	else if(type == eSegmentType_ByLineDelimiter)	str = AOSSEGMENTTYPE_BYLINEDELIMITRT;
	else if(type == eMaxSegmentType)				str = "error";
	else str = "error";
	return str;
}


u64
AosVirtualFile::countLastSeparator(
		const AosBuffPtr &buff, 
		const OmnString &separator,
		AosRundata *rdata)
{
	aos_assert_r(buff, -1);
	aos_assert_r(separator != "", -1);
	
	int len = separator.length();
	aos_assert_r(len>0, -1);
	
	const char *data = buff->data();
	aos_assert_r(data, -1);

	u64 datalen = buff->dataLen();
	for (u64 i=datalen; i>=0; i--)
	{
		if (strncmp(&data[i-1-len], separator.data(), len) == 0) 
		{
			return (i-1-len-1);
		}
	}

	return -1;
}


bool
AosVirtualFile::append(
		const char *buff,
		const int len,
		const bool flush)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 10/30/2012
bool
AosVirtualFile::read(
		const AosBuffPtr &buff,
		const int64_t &len,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}

#endif
