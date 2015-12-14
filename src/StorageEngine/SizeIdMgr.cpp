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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SizeIdMgr.h"

#include "API/AosApi.h"
#include "IILUtil/IILId.h"
#include "IILUtil/IILSystemKeys.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosSizeIdMgrSingleton,
		         AosSizeIdMgr,
		         AosSizeIdMgrSelf,
		        "AosSizeIdMgr");


AosSizeIdMgr::AosSizeIdMgr()
:
mLock(OmnNew OmnMutex())
{
	mMap.clear();
}


AosSizeIdMgr::~AosSizeIdMgr()
{
	mMap.clear();
}


bool 
AosSizeIdMgr::config(const AosXmlTagPtr &conf)
{
	aos_assert_r(conf, false);
	
	AosXmlTagPtr sizeid = conf->getFirstChild("sizeid");
	aos_assert_r(sizeid, false);

	return AosSizeIdIDGen::config(sizeid);
}


bool 
AosSizeIdMgr::start()
{
	return true;
}


bool 
AosSizeIdMgr::stop()
{
	return true;
}


AosSizeIdIDGenPtr
AosSizeIdMgr::getSizeIdIDGen(const u32 siteid, const AosRundataPtr &rdata)
{
	aos_assert_r(siteid > 0, 0);
	
	AosSizeIdIDGenPtr idgen;
	mLock->lock();
	AosSizeIdGenMapItr itr = mMap.find(siteid);
	if(itr == mMap.end())
	{
		idgen = OmnNew AosSizeIdIDGen(siteid);
		mMap[siteid] = idgen;
	}
	else
	{
		idgen = itr->second;
	}
	mLock->unlock();
	
	return idgen;
}


u64
AosSizeIdMgr::createSizeId(
		const int size,
		const AosXmlTagPtr &record,
		const AosRundataPtr &rdata)
{
	// SizeIds are managed through the system IIL 'AOSIILID_SIZEIDS'. 
	// It is a u64 IIL. This function does the following:
	// 1. Get a SizeID
	// 2. Creates the record doc 'record', 
	// 3. Add an entry [SizeId, docid] to AOSIILID_SIZEIDS
	aos_assert_r(size > 0, 0);
	aos_assert_r(record, 0);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, 0);

	// Create the record doc.
	// Note that all record doc must be in 'AOSCTNR_SIZEID'
	record->setAttr(AOSTAG_PARENTC, AOSCTNR_SIZEID);
	record->setAttr(AOSTAG_OTYPE, AOSOTYPE_FORMATTER);
	record->setAttr(AOSTAG_RECORDSIZE, size);

	//Jozhi 2014-04-16 mutilple thread to create the same container pop error
	record->setAttr("zky_public_ctnr", "true");
	record->setAttr("zky_public_doc", "true");
	
	AosXmlTagPtr doc = AosCreateDoc(record, rdata);
	aos_assert_r(doc, 0);

	u64 record_docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(record_docid, 0);

	AosSizeIdIDGenPtr idgen = getSizeIdIDGen(siteid, rdata);
	aos_assert_r(idgen, 0);
	
	return idgen->createSizeId(record_docid, rdata);
}

	
u64
AosSizeIdMgr::createSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	// SizeIds are managed through the system IIL 'AOSIILID_SIZEIDS'. 
	// It is a u64 IIL. This function does the following:
	// 1. Get a SizeID
	// 2. Add an entry [SizeId, docid] to AOSIILID_SIZEIDS
	aos_assert_r(record_docid > 0, 0);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, 0);
	
	AosSizeIdIDGenPtr idgen = getSizeIdIDGen(siteid, rdata);
	aos_assert_r(idgen, 0);
	
	return idgen->createSizeId(record_docid, rdata);
}


u64
AosSizeIdMgr::getNextDocid(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record_docid > 0, 0);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, 0);
	
	AosSizeIdIDGenPtr idgen = getSizeIdIDGen(siteid, rdata);
	aos_assert_r(idgen, 0);
	
	return idgen->getNextDocid(record_docid, rdata);
}


u64
AosSizeIdMgr::getCrtSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(record_docid > 0, 0);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, 0);
	
	AosSizeIdIDGenPtr idgen = getSizeIdIDGen(siteid, rdata);
	aos_assert_r(idgen, 0);
	
	return idgen->getCrtSizeId(record_docid, rdata);
}


bool
AosSizeIdMgr::getDocids(
		const u64 &sizeid,
		u64 &start_docid,
		int &num_docids,
		bool &overflow,
		const AosRundataPtr &rdata)
{
	// This function retrieves 'num_docids'. The system uses the IIL
	// 'AOSIILID_SIZE_DOCIDS' to manage the docids:
	// 			[sizeid, current_docids]
	aos_assert_r(sizeid > 0, false);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid > 0, false);
	
	AosSizeIdIDGenPtr idgen = getSizeIdIDGen(siteid, rdata);
	aos_assert_r(idgen, 0);
	
	return idgen->getDocids(sizeid, start_docid, num_docids, overflow, rdata);
}


u64
AosSizeIdMgr::getDataRecordDocidBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(siteid == rdata->getSiteid(), 0);

	u64 docid = getDataRecordDocidFromMap(siteid, sizeid, rdata);
	if (docid) return docid;

	bool is_unique;
	docid = AosGetKeyedU64Value(AOSIILID_SIZEIDS, sizeid, 0, is_unique, rdata);
	if (docid == 0)
	{
		AosSetErrorU(rdata, "sizeid_not_found") << ": " << sizeid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	aos_assert_r(is_unique, 0);

	setDataRecordDocidToMap(siteid, sizeid, docid, rdata);

	return docid;
}


AosDataRecordObjPtr
AosSizeIdMgr::getDataRecordBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		const AosRundataPtr &rdata)
{
	// Retrieve the docid of the recordter's doc.
	aos_assert_r(siteid == rdata->getSiteid(), 0);

	u64 docid = getDataRecordDocidBySizeId(siteid, sizeid, rdata);
	if (docid == 0)
	{
		AosSetErrorU(rdata, "sizeid_not_found") << ": " << sizeid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosDataRecordObjPtr record = getDataRecordFromMap(docid, rdata);
	if (record) return record;

	AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
	if (!doc)
	{
		AosSetErrorU(rdata, "record_doc_not_found") << ": " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	//JACKIE-HADOOP
	bool ishadoop = doc->getAttrBool("ishadoop", false);
	if(ishadoop)
	{
		OmnString newobjid = doc->getAttrStr("recordofhadoop", "");
		aos_assert_r(newobjid != "", 0);
		doc = AosGetDocByObjid(newobjid, rdata);
	}

	record = AosDataRecordObj::createDataRecordStatic(doc, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(record, 0);

	setDataRecordToMap(docid, record, rdata);

	return record;
}


AosDataRecordObjPtr
AosSizeIdMgr::getDataRecordByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	u64 sizeid = getSizeIdByDocid(docid);
	return getDataRecordBySizeId(siteid, sizeid, rdata);
}


int
AosSizeIdMgr::getDataRecordLenByDocid(
		const u32 siteid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	u64 sizeid = getSizeIdByDocid(docid);
	return getDataRecordLenBySizeId(siteid, sizeid, rdata);
}


int
AosSizeIdMgr::getDataRecordLenBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		const AosRundataPtr &rdata)
{
	AosDataRecordObjPtr record = getDataRecordBySizeId(siteid, sizeid, rdata);
	if (!record) return -1;
	//JACKIE-HADOOP
	AosXmlTagPtr record_doc = record->getRecordDoc();
	aos_assert_r(record_doc, 0);
	if (record_doc->getAttrStr("data_source") == "hadoop")
	{
		return sizeof(u16) + sizeof(u64) + sizeof(u32);
	}
	return record->getEstimateRecordLen();
}


bool
AosSizeIdMgr::removeDataRecordBySizeId(
		const u32 siteid,
		const u64 &sizeid,
		const bool flag,
		const AosRundataPtr &rdata)
{
	//Linda, 2012/05/26
	aos_assert_r(siteid == rdata->getSiteid(), 0);

	u64 docid = 0;
	bool found, is_unique;
	//bool rslt = AosIILClient::getSelf()->getDocid(
	bool rslt = AosIILClientObj::getIILClient()->getDocid(
			AOSIILID_SIZE_DOCIDS, sizeid, docid, 0, found, is_unique, rdata);
	if (!rslt || !docid)
	{
		AosSetErrorU(rdata, "Failed to get the docid") << ":" << sizeid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(
			AOSIILID_SIZE_DOCIDS, sizeid, docid, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "Failed to remove the docid") << ":" << sizeid << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
		
	rslt = AosIILClientObj::getIILClient()->getDocid(
			AOSIILID_SIZEIDS, sizeid, docid, 0, found, is_unique, rdata);
	if (!rslt || !docid)
	{
		AosSetErrorU(rdata, "Failed to get the recordid") << ":" << sizeid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(
			AOSIILID_SIZEIDS, sizeid, docid, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "Failed to remove the docid") << ":" << sizeid << ":" << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!flag) return true;

	OmnString str_docid;
	str_docid << docid;
	rslt = AosDocClientObj::getDocClient()->deleteObj(rdata, str_docid, "", "", false);
	if (!rslt)
	{
		AosSetErrorU(rdata, "Failed to remove the record doc") << ": " << docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


u64
AosSizeIdMgr::getDataRecordDocidFromMap(
		const u32 siteid,
		const u64 &sizeid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(siteid, 0);
	aos_assert_r(sizeid, 0);

	u64 docid = 0;
	AosSizeIdKey key(siteid, sizeid);

	mLock->lock();
	AosDataRecordDocidMapItr itr = mDataRecordDocidMap.find(key);
	if (itr != mDataRecordDocidMap.end())
	{
		docid = itr->second;	
	}
	mLock->unlock();
	return docid;
}


bool
AosSizeIdMgr::setDataRecordDocidToMap(
		const u32 siteid,
		const u64 &sizeid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(siteid, false);
	aos_assert_r(sizeid, false);
	aos_assert_r(docid, false);

	AosSizeIdKey key(siteid, sizeid);

	mLock->lock();
	mDataRecordDocidMap[key] = docid;
	mLock->unlock();
	return true;
}

	

AosDataRecordObjPtr
AosSizeIdMgr::getDataRecordFromMap(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, 0);

	AosDataRecordObjPtr record;
	mLock->lock();
	AosDataRecordMapItr itr = mDataRecordMap.find(docid);
	if (itr != mDataRecordMap.end())
	{
		record = itr->second;	
	}
	mLock->unlock();
	return record;
}


bool
AosSizeIdMgr::setDataRecordToMap(
		const u64 &docid,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, false);
	aos_assert_r(record, false);

	mLock->lock();
	AosDataRecordMapItr map_itr = mDataRecordMap.find(docid);
	if (map_itr == mDataRecordMap.end())
	{
		if (mDataRecordList.size() >= eMaxDataRecordNum)
		{
			AosDataRecordListItr list_itr = mDataRecordList.begin();
			if (list_itr != mDataRecordList.end())
			{
				map_itr = *(list_itr);
				mDataRecordList.pop_front();
				mDataRecordMap.erase(map_itr);
			}
		}
	
		mDataRecordMap[docid] = record;
		map_itr = mDataRecordMap.find(docid);
		aos_assert_rl(map_itr != mDataRecordMap.end(), mLock, false);
		mDataRecordList.push_back(map_itr);
	}
	mLock->unlock();
	return true;
}

