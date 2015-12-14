////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DfmWrapper/DfmWrapper.h"
#include "DfmUtil/DfmDocNorm.h"
#include "JimoAPI/JimoDocStore.h"
#include "UtilData/JPID.h"
#include "UtilData/JSID.h"
#include "API/AosApiG.h"

AosDfmWrapper::AosDfmWrapper(
		AosRundata			*rdata,
		const u32			cubeid,
		const AosDfmConfig	&config)
:
mCubeID(cubeid),
mConfig(config)	
{
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	if (!Jimo::jimoStoreCreateSE(rdata, config))
	{
		OmnAlarm << "Jimo::jimoStoreCreateSE failed!" << enderr;
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDfmWrapper::~AosDfmWrapper()
{
}

bool
AosDfmWrapper::init()
{
	OmnNotImplementedYet;
	return true;
}


u64
AosDfmWrapper::getASEID()
{
	return mConfig.mModuleId;
	/*
	switch (mConfig.mDocType)
	{
		case AosDfmDocType::eDatalet:
			return AosASEID::eASEID_Datalet;
		
		case AosDfmDocType::eNormal:
			return AosASEID::eASEID_Doc;

		case AosDfmDocType::eIIL:
			return AosASEID::eASEID_IIL;

		default:
			OmnAlarm << "Invalia DocType: " << mConfig.mDocType << enderr;
			break;
	}

	return AosASEID::eInvalidASEID;
	*/
}

bool 
AosDfmWrapper::saveDoc(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	// This is to use DocStore as IIL's storage. 
	// Encode:
	//  doc,
	//  snap_id, 
	//  docid (from doc)
	//  body (from doc)
	//  customer data (from doc)
	
	//TODO
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	
	u64 docid = getGlobalDocidByLocalDocid(doc->getDocid());
	//int body_len = doc->getOrigLen();   //there is another len called getCompressLen
	//int body_len = doc->getHeaderSize();
	AosBuffPtr bodyBuff = doc->getBodyBuff();   //body

	//int aseid = AosDocFileMgrObj::eASEID_IIL;
	u64 snap_id = 0;
	u64 aseid = getASEID();
	AosBuffPtr custom_data = doc->getHeaderBuff(); 

	OmnScreen << "custome data size:" << custom_data->dataLen() << endl;
	return Jimo::jimoStoreCreateDatalet(rdata.getPtr(), aseid, docid, 
	        bodyBuff, snap_id, custom_data);
}

bool 
AosDfmWrapper::deleteDoc(
		const AosRundataPtr &rdata,
		vector<AosTransId> &trans_ids,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	//TODO
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	//TODO:needs to clearify the logic
	u64 docid = getGlobalDocidByLocalDocid(doc->getDocid());
	u64 snap_id = 0;
	u64 aseid = getASEID();
	AosBuffPtr custom_data = doc->getHeaderBuff(); 

	OmnScreen << "custome data size:" << custom_data->dataLen() << endl;
	return Jimo::jimoStoreDeleteDataletByDocid(rdata.getPtr(), aseid, docid, 
	         snap_id, custom_data);
}

bool 
AosDfmWrapper::saveDoc(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	vector<AosTransId> trans_ids;                                      
	if(trans_id != AosTransId::Invalid) trans_ids.push_back(trans_id);

	return saveDoc(rdata, trans_ids, doc, flushflag);
}

bool 
AosDfmWrapper::deleteDoc(
		const AosRundataPtr &rdata,
		const AosTransId &trans_id,
		const AosDfmDocPtr &doc,
		const bool flushflag)
{
	vector<AosTransId> trans_ids;
	if(trans_id != AosTransId::Invalid) trans_ids.push_back(trans_id);

	return deleteDoc(rdata, trans_ids, doc, flushflag);
}

AosDfmDocPtr
AosDfmWrapper::readDoc(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body)
{
	//TODO
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	AosDfmDocPtr doc;
	doc = readDocFromDfm(rdata, docid, read_body);
	return doc;
}

bool                                
AosDfmWrapper::addHeaderFile(           
		const AosRundataPtr &rdata, 
		const AosDfmHeaderFilePtr &file)       
{                                   
	OmnNotImplementedYet;           
	return true;                    
}                                   

bool                                
AosDfmWrapper::addBodyFile(           
		const AosRundataPtr &rdata, 
		const AosDfmFilePtr &file)       
{                                   
	OmnNotImplementedYet;           
	return true;                    
} 

	bool
AosDfmWrapper::removeFromList()
{
	OmnNotImplementedYet;
	return true;
}


	bool
AosDfmWrapper::moveToFront(const AosDfmWrapperPtr &DfmWrapper)
{
	OmnNotImplementedYet;
	return true;
}


	bool
AosDfmWrapper::insertAt(const AosDfmWrapperPtr &DfmWrapper)
{
	OmnNotImplementedYet;
	return true;
}

	bool
AosDfmWrapper::removeAllFiles(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}

	bool
AosDfmWrapper::recoverDfmLog(const AosRundataPtr &rdata, const u64 &file_id)
{
	OmnNotImplementedYet;
	return true;
}

	bool
AosDfmWrapper::stop()
{
	OmnNotImplementedYet;
	return true;
}

bool
AosDfmWrapper::startStop()
{
	OmnNotImplementedYet;
	return true;
}

u32
AosDfmWrapper::getId()
{
	return mConfig.mModuleId;
}

u32
AosDfmWrapper::getVirtualId()
{
	OmnNotImplementedYet;
	return true;
}

bool
AosDfmWrapper::saveDocs(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff)
{
	// This function is used to save multiple docs (for IIL). 'index_map'
	// is a map between a docid to its position in 'buff'. 'buff' is an 
	// array of 'body_buffs'. Each body_buff stores one doc. 

	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	//Gavin 2015/09/06
	AosBuffPtr doc_buff = buff;
	u64 aseid = getASEID();
	return Jimo::jimoStoreCreateDatalets(rdata.getPtr(), mCubeID, aseid, doc_buff);

	/*
	map<u64, u64> ::iterator itr;
	for (itr = index_map.begin(); itr != index_map.end(); ++itr)
	{
		u64 docid = itr->first;
		u64 idx = itr->second;
		DocFileInfo info;
		info.offset = offset + idx;
		info.fileid = mFileId;

		mDocMap[docid] = info;
	}
	
	//u64 docid = doc->getDocid();
	//int body_len = doc->getOrigLen();
	//AosBuffPtr bodyBuff = doc->getBodyBuff();
	u64 snap_id = 0;
	AosBuffPtr custom_data = doc->getHeaderBuff();

	return Jimo::jimoStoreCreateDatalet(rdata.getPtr(), mConfig.mDocType, docid,
			buff->data(), buff->dataLen(), snap_id, custom_data);
	*/
}

bool
AosDfmWrapper::saveHeader(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc)
{
	OmnNotImplementedYet;
	return true;
}

u64
AosDfmWrapper::createSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)
{
	OmnNotImplementedYet;
	return true;
}

bool
AosDfmWrapper::commitSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)
{
	OmnNotImplementedYet;
	return true;
}

bool
AosDfmWrapper::rollbackSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosTransId &trans_id)
{
	OmnNotImplementedYet;
	return true;
}

bool
AosDfmWrapper::mergeSnapshot(
		const AosRundataPtr &rdata,
		const u64 target_snap_id,
		const u64 merger_snap_id, 
		const AosTransId &trans_id)
{
	OmnNotImplementedYet;
	return true;
}

AosDfmDocPtr
AosDfmWrapper::readDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,         
		const u64 docid,           
		const bool read_body)
{
	//TODO
	rdata->setJPID(AOSJPID_SYSTEM);
	rdata->setJSID(AOSJSID_SYSTEM);
	AosDfmDocPtr doc;
	doc = readDocFromDfm(rdata, docid, read_body);
	return doc;
}

bool
AosDfmWrapper::saveDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,           
		const AosDfmDocPtr &doc,     
		vector<AosTransId> &trans_ids)
{
	//TODO
	return saveDoc(rdata, trans_ids, doc, false); 
}

bool
AosDfmWrapper::deleteDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id,           
		const AosDfmDocPtr &doc,     
		vector<AosTransId> &trans_ids)
{
	//if (!snap_id)
		return deleteDoc(rdata, trans_ids, doc); 
	//return true;
}


AosDfmDocPtr
AosDfmWrapper::readDocFromDfm(
		const AosRundataPtr &rdata,
		const u64 docid,
		const bool read_body)
{
	bool need_binarydata = false;
	u64 snap_id = 0;
	AosBuffPtr body;
	u64 aseid = getASEID();
	//AosBuffPtr custom_data = doc->getHeaderBuff();
	AosBuffPtr header_custom_data;
	Jimo::jimoStoreGetDataletByDocid(rdata.getPtr(), aseid, getGlobalDocidByLocalDocid(docid), need_binarydata, snap_id, body, header_custom_data);
	//Xuqi 2015/10/29
	AosDfmDocPtr dfm_doc = AosDfmDoc::cloneDoc(mConfig.mDocType, docid);
	//AosDfmDocNormPtr dfm_doc = OmnNew AosDfmDocNorm(docid);   
	
	/*if (body->dataLen() == 0 && header_custom_data.isNull())
	{
		return 0;
	}*/
	if(body.isNull() && header_custom_data.isNull())
	{
		        return 0;
	}
	dfm_doc->setBodyBuff(body);

	if (body.isNull())
	{
		        dfm_doc->setOrigLen(0);
	}
	else 
	{
		        dfm_doc->setOrigLen(body->dataLen());
	}

	if (header_custom_data.notNull())
	{
		dfm_doc->setHeaderBuff(header_custom_data);
		u32 body_seq = 0;
		u64 body_off = 0;
		dfm_doc->serializeFrom(header_custom_data, body_seq, body_off);
	}

	return dfm_doc;
}


u64
AosDfmWrapper::getGlobalDocidByLocalDocid(const u64 docid)
{
	return docid * AosGetNumCubes() + mCubeID;
}
