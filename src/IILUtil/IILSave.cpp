////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 09/06/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "IILUtil/IILSave.h"

#include "Alarm/Alarm.h"
#include "DfmUtil/DfmDoc.h"
#include "XmlUtil/XmlTag.h"
#include "IILUtil/IILDfmDocs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosIILSaveSingleton, 
				 AosIILSave, 
				 AosIILSaveSelf, 
				"AosIILSave");

AosIILSave::AosIILSave()
:
mLock(OmnNew OmnMutex()),
mShowLog(false),
mCrtSnapId(0)
{
	 mCrtSnapId = 0x80000000;
}


AosIILSave::~AosIILSave()
{
}

bool
AosIILSave::start()
{
	return true;
}


bool
AosIILSave::stop()
{
	return true;
}


bool
AosIILSave::config(const AosXmlTagPtr &config)
{
	return true;
}


void
AosIILSave::addEntryToMapsPriv(const u64 &root_iilid, u64 &snap_id)
{
	mLock->lock();
	AosIILDfmDocsPtr dfmdocs = OmnNew AosIILDfmDocs(root_iilid, mShowLog);
	if (!dfmdocs)
	{
		OmnAlarm << "dfmdocs!!" << enderr;
		mLock->unlock();
		return;
	}
	//snap_id = mCrtSnapId | 0x80000000;
	snap_id = ++ mCrtSnapId;
	mIILTransMaps.insert(make_pair(snap_id, dfmdocs));	
	mSnapIds.insert(make_pair(root_iilid, snap_id));
	mLock->unlock();
}


void
AosIILSave::removeEntryFromMapsPriv(const u64 &root_iilid, const u64 &snap_id)
{
	mLock->lock();
	mapitr_t itr = mIILTransMaps.find(snap_id);
	if (itr == mIILTransMaps.end())
	{
		OmnAlarm << "dfmdocs!!" << enderr;
		mLock->unlock();
		return;
	}

	mIILTransMaps.erase(itr);	
	mSnapIds.erase(root_iilid);
	mLock->unlock();
}


AosIILDfmDocsPtr
AosIILSave::getEntryFromMapsPriv(const u64 &snap_id)
{
	mLock->lock();
	mapitr_t itr = mIILTransMaps.find(snap_id);
	if (itr == mIILTransMaps.end()) 
	{
		mLock->unlock();
		OmnAlarm << "dfmdocs!!" << enderr;
		return 0;
	}
	AosIILDfmDocsPtr dfmdocs  = itr->second;
	mLock->unlock();
	return dfmdocs;
}


bool
AosIILSave::saveDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &doc_mgr,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const u64 &snap_id)
{
	/*
	if (OmnApp::eNewDFMVersion == OmnApp::getDFMVersion())
	{
		// This is to use DocStore as IIL's storage. 
		// Encode:
		// 	doc,
		// 	snap_id, 
		// 	docid (from doc)
		// 	body (from doc)
		// 	customer data (from doc)
		u64 docid = doc->getDocid();
		int body_len = doc->getOrigLen();	//there is another len called getCompressLen
		AosBuffPtr bodyBuff = doc->getBodyBuff();	//body
		int aseid = AosDocFileMgrObj::eASEID_IIL;
		AosBuffPtr custom_data = doc->getHeaderBuff(); 
		return Jimo::jimoStoreCreateDatalet(rdata.getPtr(), aseid, docid, 
				AOSDOCTYPE_XML, bodyBuff->data(), body_len, snap_id, custom_data);
	}
	*/

	if (snap_id == 0)
	{
		return doc_mgr->saveDoc(rdata, trans_ids, doc, true);	
	}

	if (isSmallIILTrans(snap_id))
	{
		// small iil trans;
		AosIILDfmDocsPtr dfmdocs = getEntryFromMapsPriv(snap_id);
		aos_assert_r(dfmdocs, false);

		doc->setOpr(AosDfmDoc::eSave);
		return dfmdocs->addEntry(rdata, doc, trans_ids);
	}
	// dfm snap shot
	return doc_mgr->saveDoc(rdata, snap_id, doc, trans_ids);
}


AosDfmDocPtr
AosIILSave::readDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &doc_mgr,
		const u64 snap_id,
		const u64 local_iilid,
		const bool read_body)
{
	AosDfmDocPtr doc;
	if (snap_id == 0) 
	{
		doc = doc_mgr->readDoc(rdata, local_iilid);
		return doc;
	}
	if (isSmallIILTrans(snap_id))
	{
		AosIILDfmDocsPtr dfmdocs = getEntryFromMapsPriv(snap_id);
		aos_assert_r(dfmdocs, 0);

		doc = dfmdocs->readDoc(rdata, local_iilid);
		if (doc)
		{
			if (doc->getOpr() != AosDfmDoc::eDelete) 
			{
				return 0;
			}
		}
		else
		{
			doc = doc_mgr->readDoc(rdata, local_iilid);
		}
		return doc;
	}

	doc = doc_mgr->readDoc(rdata, snap_id, local_iilid, read_body);
	return doc;
}


bool
AosIILSave::deleteDoc(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &doc_mgr,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const u64 snap_id)
{
	if (snap_id == 0)
	{
		return doc_mgr->deleteDoc(rdata, trans_ids, doc, true);	
	}

	if (isSmallIILTrans(snap_id))
	{
		AosIILDfmDocsPtr dfmdocs = getEntryFromMapsPriv(snap_id);
		aos_assert_r(dfmdocs, false);

		doc->setOpr(AosDfmDoc::eDelete);
		return dfmdocs->addEntry(rdata, doc, trans_ids);
	}
	return doc_mgr->deleteDoc(rdata, snap_id, doc, trans_ids);
}


bool
AosIILSave::add(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		u64 &snap_id)
{
	// small iil trans;
	if (isExist(root_iilid)) return false;

	addEntryToMapsPriv(root_iilid, snap_id);
//OmnScreen << "============ root_iilid:" << root_iilid << ";snap_id:" << snap_id << endl;
	aos_assert_r(isSmallIILTrans(snap_id), false);
	return true;
}


bool
AosIILSave::commit(
		const AosRundataPtr &rdata,
		const u64 root_iilid,
		const AosDocFileMgrObjPtr &doc_mgr,
		const u64 snap_id)
{
	// small iil trans;
//OmnScreen << "============ root_iilid:" << root_iilid << ";snap_id:" << snap_id << endl;
	u64 snapid = isExist(root_iilid);
	aos_assert_r(snapid == snap_id, false);
	aos_assert_r(isSmallIILTrans(snap_id), false);

	AosIILDfmDocsPtr dfmdocs = getEntryFromMapsPriv(snap_id);
	aos_assert_r(dfmdocs, false);

	removeEntryFromMapsPriv(root_iilid, snap_id);
	return dfmdocs->commit(rdata, doc_mgr);
}


u64
AosIILSave::isExist(const u64 &root_iilid)
{
	u64 snap_id = 0;
	mLock->lock();
	map<u64, u64>::iterator itr = mSnapIds.find(root_iilid);
	if (itr != mSnapIds.end())
	{
		snap_id = itr->second;
	}
	mLock->unlock();
	return snap_id;
}


bool	
AosIILSave::isSmallIILTrans(const u64 &snap_id)
{
	return ((snap_id & 0x80000000) == 0x80000000);
}
