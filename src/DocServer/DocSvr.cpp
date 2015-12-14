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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocMgr/DocMgr.h"
//#include "DocProc/DocProc.h"
#include "DocLock/DocLock.h"
#include "DocServer/DocSvrUtil.h"
#include "DocServer/DocSvrProc.h"
#include "DocFileMgr/DocFileMgrKeys.h"
#include "DocFileMgr/RawFilesMgr.h"
#include "DocServer/DocBatchReaderMgr.h"
#include "DocTrans/NotifyToClientTrans.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeErrors.h"
#include "SEUtil/Passwords.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/TagMgr.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/Docid.h"
#include "StorageMgrUtil/DevLocation.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/StorageEngineObj.h"
#include "StorageEngine/SengineFixedDocReq.h"
#include "StorageEngine/SengineCSVDocReq.h"
#include "StorageEngine/SengineCommonDocReq.h"
#include "StorageEngine/SengineCommonDocNewReq.h"
#include "StorageEngine/SengineGroupedDoc.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "SEBase/SecUtil.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

extern int gAosLogLevel;
static AosDevLocationPtr sgDocDevLocation;

static AosDfmConfig sgDocDfmConf(AosDfmDocType::eDatalet,
		"Doc", AosModuleId::eDoc, true, 10, "gzip", false);

static AosDfmConfig sgAccessDfmConf(AosDfmDocType::eDatalet,
		"Access", AosModuleId::eAccessRcd, true, 10);

static AosDfmConfig sgBinayrDocDfmConf(AosDfmDocType::eDatalet,
		"BinaryDoc", AosModuleId::eBinaryDoc, true, 10);

OmnSingletonImpl(AosDocSvrSingleton,
                 AosDocSvr,
                 AosDocSvrSelf,
                "AosDocSvr");

AosDocSvr::AosDocSvr()
:
mLock(OmnNew OmnMutex()),
mReadWriteLock(OmnNew AosReadWriteLock()),
mIsCaChe(true),
mIsGroupedDocWithComp(false),
mDfmConfigs(0)
{
	/*
	mDfmConfigs = OmnNew AosDfmConfig[eMaxDfmModuleIDs];

	
	// Normal Doc DFM
	mDfmConfigs[AosModuleId::eDoc] = 
		AosDfmConfig(AosDfmDocType::eDatalet, "Doc", AosModuleId::eDoc, "gzip");

	// Access Record DFM
	mDfmConfigs[AosModuleId::eAccessRcd] = 
		AosDfmConfig(AosDfmDocType::eDatalet, "Doc", AosModuleId::eAccessRcd, "gzip");

	// Binary Doc DFM
	mDfmConfigs[AosModuleId::eAccessRcd] = 
		AosDfmConfig(AosDfmDocType::eDatalet, "Doc", AosModuleId::eBinaryDoc, "gzip");
		*/
}


static AosVfsMgrObj* sgVfsMgr = 0;


AosDocSvr::~AosDocSvr()
{
}


bool
AosDocSvr::start()
{
	return true;
}


bool
AosDocSvr::config(const AosXmlTagPtr &config)
{
	if (!config)
	{
		OmnAlarm << "Missing configuration" << enderr;
		exit(-1);
	}

	AosXmlTagPtr svrconfig = config->getFirstChild("docserver");
	if (svrconfig)
	{
		mIsCaChe = svrconfig->getAttrBool("is_cache", true);
		mIsGroupedDocWithComp = svrconfig->getAttrBool("is_groupeddoc_compress", false);

		//  <config ...>
		//      <docserver 
		//          num_virtuals="xxx"
		//          dirname="xxx"
		//          fname="xxx"
		//      </docserver>
		//      ...
		//  </config>
	}
	
	sgDocDevLocation = OmnNew AosDevLocation1();

	return true;
}


bool
AosDocSvr::stop()
{
	return true;
}


u64
AosDocSvr::parseDocid(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	//1. process access docid
	AosDocType::E type = AosDocType::getDocType(docid);
	u64 id = AosXmlDoc::getOwnDocid(docid);
	if (gAosLogLevel >= 3)
	{
		OmnScreen << "Access Record Owndocid: " << id << endl;
	}

	u64 local_id = id/AosGetNumCubes();
	AosDocType::setDocidType(type, local_id);
	if (gAosLogLevel >= 3)
	{
		OmnScreen << "Global Docid: " << docid 
			<< "  Local Docid:" << local_id << "  Doc Type: " << type << endl;
	}
	return local_id;
}


bool
AosDocSvr::createDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &newdoc,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	u64 docid = newdoc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	// 1. Store the doc into the file. 
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	u64 local_id = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = AosXmlDoc::saveToFile(
			local_id, newdoc, true, docfilemgr, 
			snap_id, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_save_file") << docid << enderr;
		return false;
	}

	//add to doc manager;
	if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, newdoc, rdata);
	rdata->setOk();
	return true;
}

bool
AosDocSvr::addSmallDocs(
		const u64 &docid,
		const AosBuffPtr &buff,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid(rdata, docid);
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = AosXmlDoc::saveBinaryDoc(did, buff, docfilemgr, 0, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "file_save_to_file") << enderr;
		return false;
	}
	return true;
}


AosBuffPtr
AosDocSvr::getSmallDocs(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid(rdata, docid);
	aos_assert_rr(docfilemgr, rdata, 0);

	AosBuffPtr buff = AosXmlDoc::readBinaryDoc(did, docfilemgr, 0, rdata);
	if (!buff)
	{
		AosSetErrorU(rdata, "failed_read_file") << docid << enderr;
		return 0;
	}

	return buff;
}


bool	
AosDocSvr::deleteSmallDocs(
		const AosRundataPtr &rdata, 
		const u64 &docid,
		const AosTransId &trans_id)
{
	return deleteBinaryDoc(docid, rdata, trans_id, 0);
}


bool
AosDocSvr::createDoc(
		const AosRundataPtr &rdata, 
		const u64 &docid,
		const char *doc,
		const int64_t &doc_len,
		const AosTransId &trans_id,
		const u64 &snap_id)
{
	aos_assert_r(docid, false);
	aos_assert_r(doc_len > 0, false);

	// 1. Store the doc into the file. 
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	u64 local_id = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = AosXmlDoc::saveToFile(
			local_id, doc, doc_len, docfilemgr, 
			snap_id, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_save_file") << docid << enderr;
		return false;
	}

	rdata->setOk();
	return true;
}


bool	
AosDocSvr::deleteObj(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	aos_assert_rr(docid, rdata, false);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	u64 local_id = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = false;
	isDocDeleted(docid, rslt, rdata);
	if (rslt)
	{
		OmnMark;
	}

	OmnScreen << "deleteDoc " << docid << endl;
	if (!AosXmlDoc::deleteDoc(local_id, docfilemgr, snap_id, trans_id, rdata))
	{
		AosSetErrorU(rdata, "failed_remove_doc_file") << docid << enderr;
		return false;
	}
		
	// Delete doc from docmgr
	if (mIsCaChe)
	{
		OmnScreen << "deleteDoc In Cache: " << docid << endl;
		if (!AosDocMgr1::getSelf()->deleteDoc(docid, rdata))
		{
			AosSetErrorU(rdata, "failed_remove_doc_DocMgr") << docid << enderr;
			return false;
		}
	}

	rdata->setOk();	
	return true;
}


bool	
AosDocSvr::modifyObj(
		const AosRundataPtr &rdata,
 		const AosXmlTagPtr &newxml,
		const u64 &newdid,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	// This function assumes:
	// 1. 'newdid' is valid or the doc is a template
	// 2. 'newdoc' is the doc to modify.
	//
	// It modifies the object 'newxml':
	aos_assert_rr(newdid, rdata, false);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(newdid, rdata);
	u64 local_id = parseDocid(rdata, newdid);
	aos_assert_rr(docfilemgr, rdata, false);

	if (!AosXmlDoc::saveToFile(local_id, newxml, false, docfilemgr, snap_id, trans_id, rdata))
	{
		//save To Cached ; save To File
		AosSetErrorU(rdata, "failed_save_doc") << newdid<< enderr;
		return false;
	}
	if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(newdid, newxml, rdata);
	//notifyToClients(newxml, rdata);
	rdata->setOk();
	return true;
}



/*
bool
AosDocSvr::incrementValue(
		const AosRundataPtr &rdata,
 		const u64 &docid,
		const OmnString &aname,
		const u64 &initvalue,
		const u64 &incValue,
		u64 &newvalue,
		const AosTransId &trans_id) 
{
	aos_assert_rr(docid , rdata, false);
	mLock ->lock();
	AosXmlTagPtr doc = getDoc(docid, rdata);
	if (!doc)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "failed_reading_doc") << docid << enderr;
		return false;
	}

	bool exist;
	u64 oldvalue = doc->getAttrU64(aname, initvalue, exist);
	newvalue = oldvalue + incValue;
	doc->setAttr(aname, newvalue);
	bool rslt = modifyObj(rdata, doc, docid, trans_id);	
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true; 
}
*/



bool
AosDocSvr::saveToFile(
		const u64 &docid,
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	aos_assert_r(docid, false);
	aos_assert_r(doc, false);

	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	u64 local_id = parseDocid(rdata, docid);
	aos_assert_rr(docfilemgr, rdata, false);
	bool rslt = AosXmlDoc::saveToFile(local_id, doc, false, docfilemgr, snap_id, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_reading_doc") << docid << enderr;
		return false;
	}
	if (mIsCaChe) AosDocMgr1::getSelf()->addDoc(doc, docid, rdata);  
	notifyToClients(doc, rdata);
	return true;
}



bool
AosDocSvr::createDocSafe(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &doc,
		const bool &saveDocFlag,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);
	bool rslt;
	if (saveDocFlag)
	{
		if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, doc, rdata);

		// 5. Store the doc into the file. 
		AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
		u64 local_id = parseDocid(rdata, docid);
		aos_assert_rr(docfilemgr, rdata, false);
		rslt = AosXmlDoc::saveToFile(local_id, doc, true, docfilemgr, snap_id, trans_id, rdata);
		if (!rslt)
		{
			AosSetErrorU(rdata, "failed_saving_to_file") << docid << enderr;
			return false;
		}
	}

	rdata->setOk();
	return true;
}

bool
AosDocSvr::isDocDeleted(
		const u64 &docid,
		bool &result,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(docid, rdata, false);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	u64 local_id = parseDocid(rdata, docid);
	aos_assert_rr(docfilemgr, rdata, false);
	result = AosXmlDoc::isDocDeleted(local_id, docfilemgr, 0,  rdata);
	rdata->setOk();
	return true;
}


AosDocFileMgrObjPtr
AosDocSvr::getDocFileMgr(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocType::E type = AosDocType::getDocType(docid);
	return getDocFileMgrPriv(vid, type, rdata);
}


AosDocFileMgrObjPtr
AosDocSvr::getDocFileMgrPriv(
		const int vid,
		const AosDocType::E type,
		const AosRundataPtr &rdata)
{
	switch(type)
	{
	case AosDocType::eNormalDoc:
		 return getDocFileMgrByVid(vid, sgDocDfmConf, rdata);

	case AosDocType::eAccessDoc:
		 return getDocFileMgrByVid(vid, sgAccessDfmConf, rdata); 

	case AosDocType::eGroupedDoc:
		 return getGroupedDocFileMgr(vid, true, rdata); 
	
	default:
		 OmnAlarm << "Unrecognized  type :" << type << enderr;
		 break;
	}
	return 0;
}


AosDocFileMgrObjPtr
AosDocSvr::getDocFileMgrByVid(
		const int vid, 
		const AosDfmConfig &config,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(sgDocDevLocation, 0);

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	AosDocFileMgrObjPtr dfm = vfsMgr->retrieveDocFileMgr(rdata.getPtr(), vid, config);
	if (dfm) return dfm;

	dfm = vfsMgr->createDocFileMgr(rdata.getPtr(), vid, config);
	if (!dfm)
	{
		OmnAlarm << "Failed creating docfilemgr. This is a serious problem!"
			<< enderr;
		return 0;
	}
	return dfm;
}


AosDocFileMgrObjPtr 
AosDocSvr::getGroupedDocFileMgr(
		const int vid,
		const bool create_flag,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	if (siteid <= 0)
	{
		AosSetErrorU(rdata, "invalid_siteid") << ": " << siteid;
		OmnAlarm << rdata->getErrmsg() << enderr;               
		return 0;                                           
	}
	
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, 0);

	OmnString filekey = AosDocFileMgrkey::getGroupedMapFileKey(vid, siteid);
	AosDocFileMgrObjPtr doc_mgr = engine->retrieveDocFileMgrByKey(
		vid, filekey, create_flag, rdata);
	if (!doc_mgr)
	{
		aos_assert_r(!create_flag, 0);
		return 0;
	}
	return doc_mgr;
}

/*
bool
AosDocSvr::writeUnlock(
		AosXmlTagPtr &doc,
		xxx)
{
	bool expired;
	mReadWriteLock->unlockWrite(expired, ...);
	if (expired)
	{
		errmsg = xxx;
		return true;
	}
}
*/

bool
AosDocSvr::readLockDoc(
		const u64 &docid,	
		const u64 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		int &status,
		AosRundata *rdata)
{
	status = 0;

	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type != AosDocType::eGroupedDoc, 0);

	bool wait = false;
	bool rslt = mReadWriteLock->readLock(docid, userid, waittimer, holdtimer, trans, wait, rdata);
	//aos_assert_r(rslt, 0);
	if (!rslt) 
	{
//OmnScreen << "@@@@@@ readLockDocerror userid : " << userid << endl;
		return false;
	}

	if(wait)
	{
		status = 1;
//OmnScreen << "@@@@@@ readLockDocwait userid : " << userid << endl;
		return false;
	}

	return true;
}


bool
AosDocSvr::writeLockDoc(
		const u64 &docid,	
		const u64 &userid,
		const int waittimer,
		const int holdtimer,
		const AosTransPtr &trans,
		int &status,
		AosRundata *rdata)
{
	status = 0;

	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type != AosDocType::eGroupedDoc, 0);

	bool wait = false;
	bool rslt = mReadWriteLock->writeLock(docid, userid, waittimer, holdtimer, trans, wait, rdata);
	//aos_assert_r(rslt, 0);
	if (!rslt) 
	{
//OmnScreen << "@@@@@@ writeLockDocerror userid : " << userid << endl;
		return false;
	}

	if(wait)
	{
		status = 1;
//OmnScreen << "@@@@@@ writeLockDocwait userid : " << userid << endl;
		return false;
	}

	return true;
}
	
bool		
AosDocSvr::readUnLockDoc(
		const u64 &docid,
		const u64 &userid,
		AosRundata *rdata)
{
	aos_assert_r(docid, false);

	bool rslt = mReadWriteLock->unLockRead(docid, userid, rdata);
	return rslt;
}

bool
AosDocSvr::writeUnLockDoc(
		const u64 &docid,
		const u64 &userid,
		const AosXmlTagPtr &newdoc,
		const AosTransId &trans_id,
		AosRundata *rdata)
{
	aos_assert_r(docid, false);

	bool rslt = mReadWriteLock->unLockWrite(docid, userid, newdoc, trans_id, rdata);
	return rslt;
}

AosXmlTagPtr
AosDocSvr::getDoc(const u64 &docid,	const u64 snap_id, const AosRundataPtr &rdata)
{
	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type != AosDocType::eGroupedDoc, 0);

	AosXmlTagPtr doc;
	if (mIsCaChe) doc = AosDocMgr1::getSelf()->getDocByDocid(docid, rdata);
	if (doc) 
	{
		//OmnScreen << "getDoc in cahce.:" << docid << endl;
		aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, 0);
		return doc;
	}

	AosDocFileMgrObjPtr docfilemgr =  getDocFileMgr(docid, rdata);
	if (!docfilemgr)
	{
		AosSetErrorU(rdata, "file_not_exist") << enderr;
		return 0;
	}

	u64 local_id = parseDocid(rdata, docid);
	doc = AosXmlDoc::readFromFile(local_id, docfilemgr, snap_id, rdata AosMemoryCheckerArgs);
	if (doc) 
	{
		if (docid != doc->getAttrU64(AOSTAG_DOCID, 0))
		{
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< doc->toString() << enderr;
			return 0;
		}

		if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, doc, rdata);
	}

	return doc;
}


AosBuffPtr
AosDocSvr::getFixedDocs(
		const AosRundataPtr &rdata,
		const vector<u64> &docids,
		const int record_size)
{
	aos_assert_r(docids.size() > 0, 0);
	
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, 0);

	AosBuffPtr doc_buff = engine->readFixedDocsFromFile(rdata, docids, record_size);
	aos_assert_r(doc_buff, 0);
	
	return doc_buff;
}

AosBuffPtr
AosDocSvr::getCommonDocs(
		const AosRundataPtr &rdata,
		const vector<u64> &docids,
		const int record_size)
{
	aos_assert_r(docids.size() > 0, 0);
	
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, 0);

	AosBuffPtr doc_buff = engine->readCommonDocsFromFile(rdata, docids, record_size);
	aos_assert_r(doc_buff, 0);
	
	return doc_buff;
}




AosBuffPtr
AosDocSvr::getCSVDocs(
		const AosRundataPtr &rdata,
		const vector<u64> &docids,
		const int record_size)
{
	aos_assert_r(docids.size() > 0, 0);
	
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, 0);

	AosBuffPtr doc_buff = engine->readCSVDocsFromFile(rdata, docids, record_size);
	aos_assert_r(doc_buff, 0);
	
	return doc_buff;
}


bool
AosDocSvr::getDocs(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff,
		const u32 num_docids,
		const AosBuffPtr &docids_buff,
		map<u64, int> &sizeid_len,
		u64 &end_docid,
		const u64 snap_id)
{
	aos_assert_r(docids_buff && docids_buff->dataLen() > 0, false);
	bool rslt = AosDocSvrUtil::getDocs(rdata, buff, num_docids, docids_buff, 
			sizeid_len, end_docid, snap_id);
	aos_assert_r(rslt, false);
	return true;
}


AosXmlTagPtr
AosDocSvr::getFixedDoc(
		const u64 &docid,
		const int record_len,
		const u64 snap_id,
		const AosRundataPtr &rdata)
{
	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type == AosDocType::eGroupedDoc, 0);

	AosXmlTagPtr doc;
	if (mIsCaChe) doc = AosDocMgr1::getSelf()->getDocByDocid(docid, rdata);
	if (doc) 
	{
		//OmnScreen << "getDoc in cahce.:" << docid << endl;
		aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, 0);
		return doc;
	}

	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	if (!docfilemgr)
	{
		AosSetErrorU(rdata, "file_not_exist") << enderr;
		return 0;
	}

	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(type);
	aos_assert_r(engine, 0);
	
	AosSengineGroupedDoc * eg = dynamic_cast<AosSengineGroupedDoc *>(engine.getPtr());
	aos_assert_r(eg, 0);

	doc = eg->readFixedDocFromFile(docid, record_len, docfilemgr, rdata);
	if (doc) 
	{
		if (docid != doc->getAttrU64(AOSTAG_DOCID, 0))
		{
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< doc->toString() << enderr;
			return 0;
		}

		if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, doc, rdata);
	}

	return doc;
}

AosXmlTagPtr
AosDocSvr::getCommonDoc(
		const u64 &docid,
		const int record_len,
		const u64 snap_id,
		const AosRundataPtr &rdata)
{
	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type == AosDocType::eGroupedDoc, 0);

	AosXmlTagPtr doc;
	if (mIsCaChe) doc = AosDocMgr1::getSelf()->getDocByDocid(docid, rdata);
	if (doc) 
	{
		//OmnScreen << "getDoc in cahce.:" << docid << endl;
		aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, 0);
		return doc;
	}

	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	if (!docfilemgr)
	{
		AosSetErrorU(rdata, "file_not_exist") << enderr;
		return 0;
	}

	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(type);
	aos_assert_r(engine, 0);
	
	AosSengineGroupedDoc * eg = dynamic_cast<AosSengineGroupedDoc *>(engine.getPtr());
	aos_assert_r(eg, 0);

	doc = eg->readCommonDocFromFile(docid, record_len, docfilemgr, rdata);
	if (doc) 
	{
		if (docid != doc->getAttrU64(AOSTAG_DOCID, 0))
		{
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< doc->toString() << enderr;
			return 0;
		}

		if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, doc, rdata);
	}

	return doc;
}



AosXmlTagPtr
AosDocSvr::getCSVDoc(
		const u64 &docid,
		const int record_len,
		const u64 snap_id,
		const AosRundataPtr &rdata)
{
	AosDocType::E type = AosDocType::getDocType(docid);
	aos_assert_r(type == AosDocType::eGroupedDoc, 0);

	AosXmlTagPtr doc;
	if (mIsCaChe) doc = AosDocMgr1::getSelf()->getDocByDocid(docid, rdata);
	if (doc) 
	{
		//OmnScreen << "getDoc in cahce.:" << docid << endl;
		aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, 0);
		return doc;
	}

	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	if (!docfilemgr)
	{
		AosSetErrorU(rdata, "file_not_exist") << enderr;
		return 0;
	}

	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(type);
	aos_assert_r(engine, 0);
	
	AosSengineGroupedDoc * eg = dynamic_cast<AosSengineGroupedDoc *>(engine.getPtr());
	aos_assert_r(eg, 0);

	doc = eg->readCSVDocFromFile(docid, record_len, docfilemgr, rdata);
	if (doc) 
	{
		if (docid != doc->getAttrU64(AOSTAG_DOCID, 0))
		{
			OmnAlarm << "Docid mismatch: " << docid << ":"
				<< doc->toString() << enderr;
			return 0;
		}

		if (mIsCaChe) AosDocMgr1::getSelf()->addCopy(docid, doc, rdata);
	}

	return doc;
}


bool
AosDocSvr::batchSaveGroupedDoc(
		const AosTransPtr &trans, 
		const int virtual_id,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_len,
		const AosBuffPtr &docids_buff,
		const AosBuffPtr &raw_data,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineFixedDocReq( trans, virtual_id, sizeid, 
			num_docs, record_len, docids_buff, raw_data, snap_id);

	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDocSvr::batchInsertDocNew(
		const AosTransPtr &trans,
		const AosBuffPtr &buff,
		const int virtual_id,
		const u64 group_id,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCommonDocNewReq(AosSengineDocReqObj::eBatchInsertCommonDoc, 
			trans, virtual_id, group_id, buff, snap_id);
	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocSvr::batchUpdateDocNew(
		const AosTransPtr &trans,
		const AosBuffPtr &buff,
		const int virtual_id,
		const u64 group_id,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCommonDocNewReq(AosSengineDocReqObj::eBatchUpdateCommonDoc, 
			trans, virtual_id, group_id, buff, snap_id);
	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDocSvr::batchInsertCommonDoc(
		const AosTransPtr &trans, 
		const int virtual_id,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_len,
		const u64 &data_len,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCommonDocReq(
		AosSengineDocReqObj::eBatchInsertCommonDoc, trans, virtual_id, 
		sizeid, num_docs, record_len, data_len, buff, snap_id);

	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}




bool
AosDocSvr::batchSaveCSVDoc(
		const AosTransPtr &trans, 
		const int virtual_id,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_len,
		const u64 &data_len,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCSVDocReq(
		AosSengineDocReqObj::eCSVDoc, trans, virtual_id, 
		sizeid, num_docs, record_len, data_len, buff, snap_id);

	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDocSvr::batchDeleteCSVDoc(
		const AosTransPtr &trans, 
		const int virtual_id,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_len,
		const u64 &data_len,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCSVDocReq(
		AosSengineDocReqObj::eDeleteCSVDoc, trans, virtual_id, 
		sizeid, num_docs, record_len, data_len, buff, snap_id);

	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDocSvr::batchUpdateCSVDoc(
		const AosTransPtr &trans, 
		const int virtual_id,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_len,
		const u64 &data_len,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	aos_assert_r(engine, false);

	AosSengineDocReqObjPtr seng_req;
	seng_req = OmnNew AosSengineCSVDocReq(
		AosSengineDocReqObj::eUpdateCSVDoc, trans, virtual_id, 
		sizeid, num_docs, record_len, data_len, buff, snap_id);

	bool rslt = engine->addReq(seng_req, rdata);
	aos_assert_r(rslt, false);
	return true;
}



bool
AosDocSvr::notifyToClients(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	return true;
	u32 siteid = rdata->getSiteid();
	aos_assert_r(doc->getAttrU32(AOSTAG_SITEID, 0) == siteid, false);

	for (int serverid = 0; serverid < AosGetNumPhysicals(); serverid++)
	{
		// Ketty 2014/03/28
		if(!isSvrHasFrontEnd(serverid))	continue;

		AosTransPtr trans = OmnNew AosNotifyToClient(
				serverid, doc->getAttrU64(AOSTAG_DOCID, 0), siteid);	
		bool rslt = AosSendTrans(rdata, trans);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosDocSvr::isSvrHasFrontEnd(const int svr_id)
{
	// Ketty 2014/03/28
	AosServerInfoPtr svr_info = AosGetSvrInfo(svr_id);
	aos_assert_r(svr_info, false);
	vector<ProcInfo> &total_procs = svr_info->getDefProcs();
	for(u32 i=0; i<total_procs.size(); i++)
	{
		ProcInfo proc_conf = total_procs[i]; 
		if(proc_conf.mType == AosProcessType::eFrontEnd)	return true;;
	}
	return false;
}


/*
bool
AosDocSvr::batchGetDocs(
			const OmnString &scanner_id,
			AosBuffPtr &resp_buff,
			const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	//bool rslt = AosDocBatchReaderMgr::getSelf()->retrieveDocs(scanner_id, resp_buff, rdata);
	//aos_assert_r(rslt, false);
	//return true;
}

bool
AosDocSvr::batchReader(
		const OmnString &scanner_id,
		const AosDocBatchReaderReq::E type,
		const AosBuffPtr &cont,
		const u32 &client_id,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	//bool rslt = AosDocBatchReaderMgr::getSelf()->proc(scanner_id, type, cont, client_id, rdata);
	//aos_assert_r(rslt, false);
	//return true;
}
*/


bool
AosDocSvr::saveBinaryDoc(
		const AosXmlTagPtr &doc,
		const AosBuffPtr &buff,
		OmnString &signature,
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	aos_assert_r(buff->dataLen() > 0, false);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	// 1. The signature is SH1(aname, secret_key). The results are then converted
	// through base64 and saved in 'sign_name'.
	signature = AosSecUtil::signValue(buff->data(), buff->dataLen());
	aos_assert_r(signature != "", false);

	OmnString blockfile = doc->getAttrStr("zky_binarydoc", ""); 
	if(blockfile == "blockfile")
	{
		AosRawFilesMgr::getSelf()->saveFile(doc, buff, rdata);
		return true;
	}
	
	// 2. Store the doc into the file. 
	//AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	bool rslt = AosXmlDoc::saveBinaryDoc(did, buff, docfilemgr, snap_id, trans_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "file_save_to_file") << enderr;
		return false;
	}
	return true;
}

bool
AosDocSvr::deleteBinaryDoc(
		const u64 &docid,
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const u64 snap_id) 
{
	aos_assert_r(docid, false);
	// 1. Store the doc into the file. 
	//AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	if (!AosXmlDoc::deleteDoc(did, docfilemgr, snap_id, trans_id, rdata))
	{
		AosSetErrorU(rdata, "failed_remove_doc_from_file") << docid << enderr;
		return false;
	}
	return true;
}


bool
AosDocSvr::retrieveBinaryDoc(
		const AosXmlTagPtr &doc, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	OmnString blockfile = doc->getAttrStr("zky_binarydoc", ""); 
	if(blockfile == "blockfile")
	{
		u64 id = doc->getAttrU64("zky_binarydoc_id", 0);
		buff = AosRawFilesMgr::getSelf()->getFile(doc, id, rdata);
		return true;
	}

	// 1. read. 
	//AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	int vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrByVid(vid, sgBinayrDocDfmConf, rdata);
	u64 did = parseDocid(rdata, docid); 
	aos_assert_rr(docfilemgr, rdata, false);

	buff = AosXmlDoc::readBinaryDoc(did, docfilemgr, snap_id, rdata);
	if (!buff)
	{
		//AosSetErrorU(rdata, "failed_read_file") << docid << enderr;
		return false;
	}
	// 3. signature
	//OmnString signature = AosSecUtil::signValue(buff->data(), buff->dataLen());
	//aos_assert_r(signature != "", false);
	//if (doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != signature)
	//{
	//	AosSetErrorU(rdata, "doc_failed_security_checking") << enderr;
	//	return false;
	//}
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);
	return true;
}


/*
bool
AosDocSvr::deleteBatchDocs(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
{
	bool rslt = AosDocBatchReaderMgr::getSelf()->deleteDocs(scanner_id, rdata);
	aos_assert_r(rslt, false);
	return true;
}
*/

bool
AosDocSvr::mergeSnapshot(
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 target_snap_id,
		const u64 merge_snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{              
	if (doc_type == AosDocType::eGroupedDoc)
	{
		AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(doc_type);
		aos_assert_r(engine, false);
		engine->flushContents();
	}
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrPriv(virtual_id, doc_type, rdata);
	aos_assert_r(docfilemgr, 0);

	return docfilemgr->mergeSnapshot(rdata, target_snap_id, merge_snap_id, trans_id);    
}              


u64
AosDocSvr::createSnapshot(
		const int virtual_id,
		const u64 snap_id,
		const AosDocType::E doc_type,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	if (doc_type == AosDocType::eGroupedDoc)
	{
		 AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(doc_type);
		 aos_assert_r(engine, false);
		 engine->flushContents();
	}
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrPriv(virtual_id, doc_type, rdata);
	aos_assert_r(docfilemgr, 0);

	docfilemgr->createSnapshot(rdata, snap_id, trans_id);
	
	return snap_id;
}


bool
AosDocSvr::commitSnapshot(
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	if (doc_type == AosDocType::eGroupedDoc)
	{
		 AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(doc_type);
		 aos_assert_r(engine, false);
		 engine->flushContents();
	}
	aos_assert_r(snap_id, false);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrPriv(virtual_id, doc_type, rdata);
	aos_assert_r(docfilemgr, false);

	return docfilemgr->commitSnapshot(rdata, snap_id, trans_id);
}


bool
AosDocSvr::rollbackSnapshot(
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	if (doc_type == AosDocType::eGroupedDoc)
	{
		 AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(doc_type);
		 aos_assert_r(engine, false);
		 engine->flushContents();
	}

	aos_assert_r(snap_id, false);
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgrPriv(virtual_id, doc_type, rdata);
	aos_assert_r(docfilemgr, false);

	return docfilemgr->rollbackSnapshot(rdata, snap_id, trans_id);
}





















#if 0
// Ketty 2013/01/29
bool
AosDocSvr::startTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocSvr::commitTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	/*
	AosDfmProcPtr dfm_proc(this, false);
	AosDocFileMgrObjPtr dfm;
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	int svr_id = AosGetSelfServerId();
	aos_assert_r(svr_id >=0, false);

	vector<u32> total_vids;
	AosGetVirtualIds(total_vids, svr_id);
	for(u32 i=0; i < total_vids.size(); i++)
	{
		u32 vid = total_vids[i];
		dfm = vfsMgr->retrieveDocFileMgr(vid, AosModuleId::eDoc, dfm_proc, rdata);	
		if(dfm)
		{
			dfm->commitTrans(rdata);
		}
		
		dfm = vfsMgr->retrieveDocFileMgr(vid, AosModuleId::eAccessRcd, dfm_proc, rdata);	
		if(dfm)
		{
			dfm->commitTrans(rdata);
		}
		
		dfm = vfsMgr->retrieveDocFileMgr(vid, AosModuleId::eBinaryDoc, dfm_proc, rdata);	
		if(dfm)
		{
			dfm->commitTrans(rdata);
		}
	
		OmnString filekey = AosDocFileMgrkey::getGroupedDocMapFileKey(vid, rdata->getSiteid()); 
		dfm= vfsMgr->retrieveDocFileMgrByKey(vid, filekey, false, dfm_proc, rdata);
		if(dfm)
		{
			dfm->commitTrans(rdata);
		}
	}
	return true;
	*/
}


bool
AosDocSvr::rollbackTrans(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

AosXmlTagPtr 
AosDocSvr::createExclusiveDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


AosBuffPtr
AosDocSvr::getDocTester(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// This function is used by torturer. 
	AosDocFileMgrObjPtr docfilemgr = getDocFileMgr(docid, rdata);
	if (!docfilemgr)
	{
		AosSetErrorU(rdata, "file_not_exist") << enderr;
		return 0;
	}

	u64 local_id = parseDocid(rdata, docid);
	AosBuffPtr doc_buff = AosXmlDoc::readFromFileTester(local_id, docfilemgr, rdata AosMemoryCheckerArgs);
	return doc_buff;
}


bool
AosDocSvr::procCheckLock(
		const AosRundataPtr &rdata,
		const u64 &docid,
		const OmnString &type)
{
	aos_assert_r(docid, false);
	aos_assert_r(type != "", false);
	if (type == "checkread") 
	{
		 return  AosDocLock::getSelf()->checkRead(rdata, docid);
	}

	return AosDocLock::getSelf()->checkModify(rdata, docid);
}


bool
AosDocSvr::procDocLock(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans,
		const u64 &docid,
		const OmnString &lock_type,
		const u64 &lock_timer,
		const u64 &lockid)
{
	OmnNotImplementedYet;
	return false;
	
	/*
	aos_assert_r(docid, false);
	aos_assert_r(lock_type != "", false);
	aos_assert_r(rdata->getUserid(), false);
	AosDocLock::E opr = AosDocLock::toEnum(lock_type);
	u64 transid = 0;
	bool rslt = false;
	switch(opr)
	{
		case AosDocLock::eReadLock:
			 {
			 aos_assert_r(lock_timer, false);
			 AosXmlTagPtr transxml = trans->getXmlData();
			 aos_assert_r(transxml, false);
			 AosXmlTagPtr transroot = transxml->getParentTag();
			 aos_assert_r(transroot, false);
			 transid = transroot->getAttrU64(AOSTAG_TRANSID, 0);
			 aos_assert_r(transid, false);
			 rslt = AosDocLock::getSelf()->readLock(rdata, docid, lock_timer, transid);
			 if (rdata->isOk()) trans->setFinishLater();
			 }
			 break;

		case AosDocLock::eReadUnLock:
			 aos_assert_r(lockid, false);
			 rslt = AosDocLock::getSelf()->readUnLock(rdata, docid, lockid);
			 break;

		case AosDocLock::eWriteLock:
			 {
			 aos_assert_r(lock_timer, false);
			 AosXmlTagPtr transxml = trans->getXmlData();
			 aos_assert_r(transxml, false);
			 AosXmlTagPtr transroot = transxml->getParentTag();
			 aos_assert_r(transroot, false);
			 transid = transroot->getAttrU64(AOSTAG_TRANSID, 0);
			 aos_assert_r(transid, false);
			 rslt = AosDocLock::getSelf()->writeLock(rdata, docid, lock_timer, transid);
			 if (rdata->isOk()) trans->setFinishLater();
			 }
			 break;

		case AosDocLock::eWriteUnLock:
			 aos_assert_r(lockid, false);
			 rslt = AosDocLock::getSelf()->writeUnLock(rdata, docid, lockid);
			 break;
		default:
			 break;
	}
	aos_assert_rr(rslt, rdata, false);
	return true;
	*/
}


/*
bool
AosDocSvr::notifyToClients(const AosXmlTagPtr &doc)
{
	u64 mt_epoch = doc->getAttrU64(AOSTAG_MT_EPOCH, 0);
	if (!mt_epoch) 
	{
		return true;
	}

	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid, false);

	OmnString docstr = "<update ";
	docstr << AOSTAG_DOCID << "=\"" << docid << "\" " 
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_MT_EPOCH << "=\"" << mt_epoch << "\" />"; 

	aos_assert_r(docstr != "", false);
	AosXmlParser parser;
	AosXmlTagPtr transxml = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(transxml, false);

	//mTransServer->notifyToClients(transxml);
	AosTransServerObjPtr trans_svr = AosTransServerObj::getTransServer();
	aos_assert_r(trans_svr, false);
	trans_svr->notifyToClients(transxml);
	return true;
}
*/


// Ketty 2012/07/30
/*
bool
AosDocSvr::removeDocFileMgrBySizeid(
			const u64 sizeid, 
			const AosRundataPtr &rdata)
{
	AosVirtualFileSysPtr filesys;
	mLock->lock();
	VirtFileSysMapItr itr = mVirtFileSysMap.find(sizeid);
	// Not in the cache.
	if(itr != mVirtFileSysMap.end())
	{
		filesys	= itr->second;
		mVirtFileSysMap.erase(sizeid);
	}
	else 
	{
		OmnString filekey = AosFilekey::getSizeIdVirtFileSysFileKey(sizeid);
		filesys = OmnNew AosVirtualFileSys(filekey, AOSPASSWD_SYSTEM_CALL, false, rdata);
	}

	mLock->unlock();
	aos_assert_r(filesys, false);
	if (!filesys->isGood()) 
	{
		return false;
	}
	bool rslt = filesys->removeVirtualFileSys(rdata);
	return rslt;
}
*/	

#endif
