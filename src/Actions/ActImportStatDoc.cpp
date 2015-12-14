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
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActImportStatDoc.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DataStructs/StructProc.h"
#include "DataStructs/Vector2D.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "NetFile/NetFile.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

AosActImportStatDoc::AosActImportStatDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_IMPORTSTATDOC, AosActionType::eImportStatDoc, flag),
mScanner(0),
mRundata(0),
mMainLock(OmnNew OmnMutex()),
mMainCondVar(OmnNew OmnCondVar()),
mExitThrd(false),
mThread(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosActImportStatDoc::~AosActImportStatDoc()
{
	OmnScreen << "act import stat doc deleted" << endl;
}


bool
AosActImportStatDoc::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!mThread)
	{
		OmnThreadedObjPtr thisptr(this, false); 
		mThread = OmnNew OmnThread(thisptr, "importstatdoc_1", 1, false, true, __FILE__, __LINE__);
		mThread->start(); 
	}
	
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	if (!sdoc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task)
	{
		AosSetError(rdata, AosErrmsgId::eMissingSmartDoc);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mTask = task;
	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert_r(mScanner, false);

	bool rslt = configRecordLen(sdoc, rdata);
	aos_assert_r(rslt, false);

	//rslt = configAsm(sdoc, rdata);
	//aos_assert_r(rslt, false);

	rslt = configVector2D(sdoc, rdata);
	aos_assert_r(rslt, false);

	rslt = configFileInfo(sdoc, rdata);
	aos_assert_r(rslt, false);

	u64 blocksize = eMaxBlockSize - (eMaxBlockSize % mRecordLen);

	AosActionCallerPtr thisptr(this, true);
	mScanner->setActionsCaller(thisptr);

	u64 reqId = 0;
	mScanner->addRequest(reqId, blocksize);

	while(1)
	{
		mMainLock->lock();
		if (mReqIds.empty())
		{
			if (allFinished()) return true;

			mMainCondVar->wait(mMainLock);
			mMainLock->unlock();
			continue;
		}
		
		reqId = mReqIds.front();
		mReqIds.pop();
		mMainLock->unlock();

		mScanner->addRequest(reqId, blocksize);
	}

	return true;
}

bool
AosActImportStatDoc::configRecordLen(
        const AosXmlTagPtr &sdoc,
        const AosRundataPtr &rdata)
{
	aos_assert_r(sdoc, false);
	AosXmlTagPtr datacol_tag = sdoc->getFirstChild("data_collector");
	aos_assert_r(datacol_tag, false);

	AosXmlTagPtr iiltag = datacol_tag->getFirstChild("iilasm");
	aos_assert_r(iiltag, false);

	mRecordLen = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	if (mRecordLen < 0)
	{
		AosXmlTagPtr rcd_xml = iiltag->getFirstChild(AOSTAG_RECORD);
		aos_assert_r(rcd_xml, false);

		OmnString rcd_objid = rcd_xml->getAttrStr(AOSTAG_OBJID, "");
		aos_assert_r(rcd_objid != "", false);

		AosXmlTagPtr record = AosGetDocByObjid(rcd_objid, rdata);                                   
		aos_assert_r(record, false);

		mRecordLen = record->getAttrInt("record_len", -1);
		aos_assert_r(mRecordLen > 0, false);
	}

	return true;
}


bool
AosActImportStatDoc::configAsm(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//felicia, 2014/01/22
	OmnString dclid = sdoc->getAttrStr("zky_new_datacolid");
	aos_assert_r(dclid != "", false);
	
	AosXmlTagPtr datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);
	
	OmnString asm_key = datacol_tag->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	aos_assert_r(asm_key != "", false);

	mNewIILAsm = AosCreateIILAssembler(rdata, asm_key, mTask, datacol_tag);
	aos_assert_r(mNewIILAsm, false);
	
	dclid = sdoc->getAttrStr("zky_old_datacolid", "");
	aos_assert_r(dclid != "", false);
	
	datacol_tag = mTask->getDataColTag(dclid, rdata);
	aos_assert_r(datacol_tag, false);
	
	asm_key = datacol_tag->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	aos_assert_r(asm_key != "", false);

	mOldIILAsm = AosCreateIILAssembler(rdata, asm_key, mTask, datacol_tag);
	aos_assert_r(mOldIILAsm, false);	
	return true;
}


bool
AosActImportStatDoc::configFileInfo(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	//OmnString file_objid = sdoc->getAttrStr("zky_file_objid", 0);
	//aos_assert_r(file_objid != "", false);

	//AosXmlTagPtr file_conf = AosGetDocByObjid(file_objid, rdata);
	//aos_assert_r(file_conf, false);

	//AosXmlTagPtr filestag = file_conf->getFirstChild("files");
	AosXmlTagPtr filestag = sdoc->getFirstChild("files");
	aos_assert_r(filestag, false);

	AosXmlTagPtr entry = filestag->getFirstChild(true);
	aos_assert_r(entry, false);

	int data_serverid = entry->getAttrInt(AOSTAG_PHYSICALID, -1);
	OmnString datacol_id = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID, "");
	aos_assert_r(datacol_id != "", false);

	u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	AosNetFileObjPtr file = OmnNew AosIdNetFile(data_serverid, file_id);
	mFinished = false;

	vector<AosNetFileObjPtr> files;
	files.push_back(file);

	mScanner->setFiles(files);
	return true;
}

bool
AosActImportStatDoc::configVector2D(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnString objid = sdoc->getAttrStr("stat_objid", "");
	aos_assert_r(objid != "", false);

	AosXmlTagPtr stat_xml_doc = AosGetDocByObjid(objid, rdata);
	aos_assert_r(stat_xml_doc, false);

	AosXmlTagPtr tag = stat_xml_doc->getFirstChild();
	aos_assert_r(tag, false);

	//OmnString stat_type = tag->getAttrStr("zky_stat_type", "");
	//aos_assert_r(stat_type != "", false);

	//AosXmlTagPtr conf = stat_xml_doc->getChildByAttr("zky_stat_type", stat_type);
	AosXmlTagPtr conf = tag->getFirstChild();
	aos_assert_r(conf, false);

	//mVector2D = OmnNew AosVector2D(rdata, conf);
	aos_assert_r(mVector2D, false);
	return true;
}
	

void
AosActImportStatDoc::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mTask->setErrorType(AosTaskErrorType::eInPutError);
		actionFailed(mRundata);
		return;
	}

	mLock->lock();
	Req req = {reqId, expected_size, finished};
	mReq.push(req);
	mCondVar->signal();
	mLock->unlock();
	return;
}
	

bool
AosActImportStatDoc::allFinished()
{
	if (!mFinished)  return false;

	mExitThrd = true;

	mLock->lock();
	mCondVar->signal();
	mLock->unlock();

	mThread = 0;
	actionFinished(mRundata);
	mTask = 0;

	aos_assert_r(mScanner, false);
	mScanner->destroyedMember();
	mScanner = 0;
	return true;
}


AosActionObjPtr
AosActImportStatDoc::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportStatDoc(false);
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
AosActImportStatDoc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mExitThrd)
		{
			state = OmnThrdStatus::eExit;
			mLock->unlock();
			return true;
		}

		if (mReq.empty())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		Req req = mReq.front();
		mReq.pop();
		mLock->unlock();
		modifyLocal(req);
	}
	return true;
}


bool
AosActImportStatDoc::modifyLocal(const Req &req)
{
	AosBuffPtr buff = mScanner->getNextBlock(req.reqId, req.expectedSize);
	aos_assert_r(buff, false);
	
u64 t1 = OmnGetSecond();
	vector<AosValueRslt> oldvalue_rslt; 
	vector<AosValueRslt> newvalue_rslt;
	bool rslt = mVector2D->modifyLocal(mRundata, buff, mRecordLen, oldvalue_rslt, newvalue_rslt);
	aos_assert_r(rslt, false);
OmnScreen << "BBBBBBBBBBBBB cost:" << OmnGetSecond() - t1 << endl;
	aos_assert_r(oldvalue_rslt.size() != 0 || newvalue_rslt.size() != 0, false);

	//felicia, 2014/01/22
//	for(size_t i=0; i<oldvalue_rslt.size(); i++)
//	{
//		mOldIILAsm->appendEntry(oldvalue_rslt[i], mRundata);
//	}
//
//	for(size_t i=0; i<newvalue_rslt.size(); i++)
//	{
//		mNewIILAsm->appendEntry(newvalue_rslt[i], mRundata);
//	}

	mMainLock->lock();
	if (req.finished)
	{
		mFinished = req.finished;
	}
	else
	{
		mReqIds.push(req.reqId);
	}

	mMainCondVar->signal();
	mMainLock->unlock();
	return true;
}

bool
AosActImportStatDoc::signal(const int threadLogicId)
{
	return true;
}


bool
AosActImportStatDoc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

#endif
