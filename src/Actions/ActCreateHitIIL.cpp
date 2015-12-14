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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActCreateHitIIL.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/NetFileObj.h" 
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SmartDoc/SmartDoc.h"
#include "UtilData/ModuleId.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "NetFile/NetFile.h"
#include "XmlUtil/XmlTag.h"
#include "SEModules/SnapshotIdMgr.h"
#include "SEUtil/IILName.h"


AosActCreateHitIIL::AosActCreateHitIIL(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATEHITIIL, AosActionType::eCreateHitIIL, flag)
{
}


AosActCreateHitIIL::AosActCreateHitIIL(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_CREATEHITIIL, AosActionType::eCreateHitIIL, false)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActCreateHitIIL::~AosActCreateHitIIL()
{
	OmnScreen << "~~~ act create hit iil deleted" << endl;
}


bool
AosActCreateHitIIL::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mComposeIILName = def->getAttrBool("zky_compose_iilname", false);
	if(mComposeIILName)
	{
		mAttrName = def->getAttrStr("zky_attrname", "");
		mTableName = def->getAttrStr("zky_tablename", "");
	}

	//AosXmlTagPtr dataCol_tag = def->getFirstChild("data_collector");
	AosXmlTagPtr dataCol_tag = def->getFirstChild("datacollector");
	aos_assert_r(dataCol_tag, false);

	//AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("iilasm");
	AosXmlTagPtr iiltag = dataCol_tag->getFirstChild("asm");
	aos_assert_r(iiltag, false);

	AosXmlTagPtr record_tag = iiltag->getFirstChild("datarecord");
	aos_assert_r(record_tag, false);
	
	if (record_tag->getAttrStr("type") == "ctnr")
	{
		record_tag = record_tag->getFirstChild("datarecord");
		aos_assert_r(record_tag, false);
	}
	mRcdLen = record_tag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(mRcdLen > 0, false);

	mIILType = iiltag->getAttrStr("zky_asmtype");
	aos_assert_r(mIILType!= "", false);

	if (mIILType == "stradd")
	{
		mIILNameOffset = iiltag->getAttrInt("zky_iilname_offset", -1);
		aos_assert_r(mIILNameOffset != -1, false);

		mIILNameLen = iiltag->getAttrInt("zky_iilname_len", -1);
		aos_assert_r(mIILNameLen != -1, false);

		mValueOffset = iiltag->getAttrInt("zky_value_offset", -1);
		aos_assert_r(mValueOffset != -1, false);

		mValueLen = iiltag->getAttrInt("zky_value_len", -1);
		aos_assert_r(mValueLen != -1, false);
	}
	
	mBuildBitmap = def->getAttrBool("build_bitmap", false);
	return true;
}


bool
AosActCreateHitIIL::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);

	if(!task)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mTask = task;

	AosXmlTagPtr files_tag = sdoc->getFirstChild("files");
	aos_assert_r(files_tag, false);

	AosXmlTagPtr file_tag = files_tag->getFirstChild(true);
	aos_assert_r(file_tag, false);

	mCrtPos = file_tag->getAttrInt64(AOSTAG_START_POS, -1);
	aos_assert_r(mCrtPos != -1, false);

	mMaxLength = file_tag->getAttrInt64(AOSTAG_LENGTH, -1);
	aos_assert_r(mMaxLength != -1, false);
	aos_assert_r(mMaxLength % mRcdLen == 0, false);

	mProcLength = 0;
	
	mFileId = file_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(mFileId != 0, false);
		
	mPhysicalId = file_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(mPhysicalId != -1, false);
	
	AosNetFileObjPtr file = OmnNew AosIdNetFile(mPhysicalId, mFileId);
	aos_assert_r(file, false);

	file->seek(mCrtPos);

	vector<AosNetFileObjPtr> virFiles;
	virFiles.push_back(file);

	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	mScanner->setFiles(virFiles);
	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	int64_t buffsize = eMaxBuffSize / mRcdLen;
	buffsize *= mRcdLen;

	// ????
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}
	
	aos_assert_r(buffsize % mRcdLen == 0, false);
	mScanner->addRequest(0, buffsize);
	return true;
}

void
AosActCreateHitIIL::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert(buff);
	int64_t bufflen = buff->dataLen();

	if (bufflen > 0)
	{
		aos_assert(bufflen % mRcdLen == 0);
		if (mIILType == "hitremove")
		{
			OmnAlarm << "Not Complete Yet !" << enderr;
			return;
		}
		else if(mIILType == "hitadd")
		{
			bool rslt = createHitIIL(buff, mRundata);
			aos_assert(rslt);
		}
		else if(mIILType == "stradd")
		{
			bool rslt = strAddIIL(buff, mRundata);
			aos_assert(rslt);
		}
		else
		{
			OmnAlarm << "Invalid type : " << mIILType << enderr;
		}

		mCrtPos += bufflen;
		mProcLength += bufflen;
		aos_assert(mProcLength <= mMaxLength);
	}

	if (finished || mProcLength == mMaxLength)
	{
		AosActionObjPtr thisptr(this, true);
		mTask->actionFinished(thisptr, mRundata);
		mTask = 0;
	
		aos_assert(mScanner);
		mScanner->destroyedMember();
		mScanner = 0;

		return;
	}

	int64_t buffsize = eMaxBuffSize / mRcdLen;
	buffsize *= mRcdLen;
	
	if (buffsize > mMaxLength - mProcLength)
	{
		buffsize = mMaxLength - mProcLength;
	}

	aos_assert(buffsize % mRcdLen == 0);
	mScanner->addRequest(reqId, buffsize); 
}

bool
AosActCreateHitIIL::strAddIIL(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	aos_assert_r(buff->dataLen() % mRcdLen == 0, false);
	char * entry = buff->data();
	char * crt_entry = &entry[mRcdLen];
	aos_assert_r(entry, false);
	
	AosBuffPtr new_buff = OmnNew AosBuff(buff->dataLen() AosMemoryCheckerArgs);
	char *data = new_buff->data();
	int crt_pos = 0;
	memcpy(&data[crt_pos], &entry[mValueOffset], mValueLen);
	crt_pos += mValueLen;

	u32 idx = 1;
//	u64 docid = 0;
//	int64_t pos = 0;
	u32 num_entries = buff->dataLen()/mRcdLen;
	bool finished = false;
	u64 executor_id = 0;
	bool rslt;
	for (u32 i=1; i<num_entries; i++)
	{
		if (strncmp(&entry[mIILNameOffset], &crt_entry[mIILNameOffset], mIILNameLen) != 0 || i == num_entries-1)
		{
			OmnString iilname(&entry[mIILNameOffset], mIILNameLen);
			
			if (i == num_entries-1 && strncmp(&entry[mIILNameOffset], &crt_entry[mIILNameOffset], mIILNameLen) == 0)
			{
				memcpy(&data[crt_pos], &entry[mValueOffset], mValueLen);
				crt_pos += mValueLen;
				finished = true;
			}
			
			u64 snapshot_id = getSnapShotId(iilname);
			aos_assert_r(snapshot_id != 0, false);
			new_buff->setDataLen(crt_pos);
			rslt = AosIILClientObj::getIILClient()->StrBatchAdd(iilname, mValueLen, new_buff, executor_id, snapshot_id, mTaskDocid, rdata);
			aos_assert_r(rslt, false);

			new_buff->reset();
			crt_pos = 0;
			idx = 0;
			entry = &entry[mRcdLen];
			if (entry != crt_entry)
			{
				strncpy(entry, crt_entry, mRcdLen);
			}	
		}

		idx++;
		crt_entry = &crt_entry[mRcdLen];
	}

	if (!finished)
	{
		// the last entry
		aos_assert_r(idx == 1, false);
		
		OmnString iilname(&entry[mIILNameOffset], mIILNameLen);

		u64 snapshot_id = getSnapShotId(iilname);
		aos_assert_r(snapshot_id != 0, false);
		new_buff->setDataLen(crt_pos);
		rslt = AosIILClientObj::getIILClient()->StrBatchAdd(iilname, mValueLen, new_buff, executor_id, snapshot_id, mTaskDocid, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}

bool
AosActCreateHitIIL::createHitIIL(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, false);
	aos_assert_r(buff->dataLen() % mRcdLen == 0, false);
	char * entry = buff->data();
	char * crt_entry = &entry[mRcdLen];
	aos_assert_r(entry, false);
	
	vector<u64> docids;
	u64 entry_docid = *(u64 *)&entry[mRcdLen - sizeof(u64)];
	docids.push_back(entry_docid);

	AosBuffPtr new_buff = OmnNew AosBuff(buff->dataLen() AosMemoryCheckerArgs);
	char *data = new_buff->data();
	int64_t crt_pos = 0;
	memcpy(&data[crt_pos], entry, mRcdLen);
	crt_pos += mRcdLen;

	u32 idx = 1;
	u64 docid = 0;
//	int64_t pos = 0;
	u32 num_entries = buff->dataLen()/mRcdLen;
	bool finished = false;
	u64 executor_id = 0;
	if(mBuildBitmap)
	{
		executor_id = 1;
	}
	for (u32 i=1; i<num_entries; i++)
	{
		if (strcmp(entry, crt_entry) != 0 || i == num_entries-1)
		{
			OmnString iilname(entry);
			if(mComposeIILName)
			{
				iilname = AosIILName::composeTableAttrWordIILName(mTableName, mAttrName, iilname);
			}
			if (i == num_entries-1 && strcmp(entry, crt_entry) == 0)
			{
				idx++;
				docid = *(u64 *)&crt_entry[mRcdLen - sizeof(u64)];
				aos_assert_r(docid >= entry_docid, false);
				if (docid > entry_docid)
				{
					docids.push_back(docid);
					memcpy(&data[crt_pos], crt_entry, mRcdLen);
					crt_pos += mRcdLen;
				}
				finished = true;
			}
			
			bool rslt = sanitycheck(docids);
			aos_assert_r(rslt, false);

			new_buff->setDataLen(crt_pos);

			u64 snapshot_id = getSnapShotId(iilname);
			aos_assert_r(snapshot_id != 0, false);
			//rslt = AosIILClientObj::getIILClient()->HitBatchAdd(iilname, docids, snapshot_id, mTaskDocid, rdata);
			rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
					iilname, mRcdLen, new_buff, executor_id, snapshot_id, mTaskDocid, mRundata);
			aos_assert_r(rslt, false);
			

			idx = 0;
			docids.clear();
			new_buff->reset();
			crt_pos = 0;

			entry = &entry[mRcdLen];
			if (entry != crt_entry)
			{
				strncpy(entry, crt_entry, mRcdLen);
			}
			*(u64 *)&entry[mRcdLen - sizeof(u64)] = *(u64 *)&crt_entry[mRcdLen - sizeof(u64)];
			entry_docid = *(u64 *)&entry[mRcdLen - sizeof(u64)];
			docids.push_back(entry_docid);
			memcpy(&data[crt_pos], entry, mRcdLen);
			crt_pos += mRcdLen;
		}

		docid = *(u64 *)&crt_entry[mRcdLen - sizeof(u64)];
		aos_assert_r(docid >= entry_docid, false);
		if (docid > entry_docid)
		{
			docids.push_back(docid);
			entry_docid = docid;
			memcpy(&data[crt_pos], crt_entry, mRcdLen);
			crt_pos += mRcdLen;
		}

		idx++;
		crt_entry = &crt_entry[mRcdLen];
	}

	if (!finished)
	{
		// the last entry
		aos_assert_r(idx == 1, false);
		
		OmnString iilname(entry);
		if(mComposeIILName)
		{
			iilname = AosIILName::composeTableAttrWordIILName(mTableName, mAttrName, iilname);
		}

		u64 snapshot_id = getSnapShotId(iilname);
		aos_assert_r(snapshot_id != 0, false);
		new_buff->setDataLen(crt_pos);
		//bool rslt = AosIILClientObj::getIILClient()->HitBatchAdd(iilname, docids, snapshot_id, mTaskDocid, rdata);
		bool rslt = AosIILClientObj::getIILClient()->StrBatchAdd(
				iilname, mRcdLen, new_buff, executor_id, snapshot_id, mTaskDocid, mRundata);
		aos_assert_r(rslt, false);
	}

	return true;
}

u64
AosActCreateHitIIL::getSnapShotId(const OmnString &iilname)
{
	u32 virtual_id = AosGetCubeId(iilname);
	map<u32, u64>::iterator itr = mSnapMaps.find(virtual_id);
	if(itr == mSnapMaps.end()) return 0;
	return itr->second;
}

bool
AosActCreateHitIIL::sanitycheck(const vector<u64> &docids)
{
	for(u32 i=0; i<docids.size(); i++)
	{
		if (i>0)
		{
			aos_assert_r(docids[i] > docids[i-1], false);
		}
	}
	return true;
	
/*
	aos_assert_r(buff, false);
	int64_t bufflen = buff->dataLen();
	aos_assert_r(bufflen % mRcdLen == 0, false);

	char *entry = buff->data();	
	for(u32 i=0; i < bufflen/mRcdLen - 1; i++)
	{
		char *crt = entry + mRcdLen;
		aos_assert_r(strcmp(entry, crt) == 0, false);
		u64 docid1 = *(u64 *)&entry[mRcdLen - sizeof(u64)];
		u64 docid2 = *(u64 *)&crt[mRcdLen - sizeof(u64)];
		aos_assert_r(docid1 < docid2, false);
		entry = crt;
	}
	return true;
	*/
}

AosActionObjPtr
AosActCreateHitIIL::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateHitIIL(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

bool
AosActCreateHitIIL::createSnapShot(const AosTaskObjPtr &task, const AosRundataPtr &rdata)
{
	vector<AosTaskDataObjPtr> snapshots;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	aos_assert_r(task, false);
	mTaskDocid = task->getTaskDocid();
	AosTaskDataObjPtr snapshot;
	vector<u32> virtualids = AosGetTotalCubeIds();
	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		OmnString key;
		key << AOSTASKDATA_IIL_SNAPSHOT << "__"
			<< virtualids[i]<< "__"
			<< mTaskDocid;
		AosTaskDataObjPtr snapshot = task->getSnapShot(key, rdata);
		aos_assert_r(!snapshot, false);
		snapshot_id = iil_client->createSnapshot(virtualids[i], snapshot_id, mTaskDocid, rdata);
		set<OmnString> iilnames;
		snapshot = AosTaskDataObj::createTaskDataIILSnapShot(virtualids[i], snapshot_id, iilnames, mTaskDocid, false);
		aos_assert_r(snapshot, false);
		
		mSnapMaps[virtualids[i]] = snapshot_id;
		snapshots.push_back(snapshot);
	}
	return task->updateTaskSnapShots(snapshots, rdata);
}

#endif
