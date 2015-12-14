////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// A document is stored in two forms: header and whole body. This class
// handles the header. There are two groups of fields: integral and 
// string fields. Header storage is fixed size (determined by 
// 'eDocHeaderSize'). This size is guaranteed to fit all integral 
// fields. If the total header size is not bigger than this value, 
// the entire header is stored in the header block. Otherwise, only
// the integral fields are stored in the header block. 
//
// When retrieving a header, it checks the 'mStorageFlag' field. If it
// is 'c', it means the string fields are stored in the Doc itself. 
// It needs to retrieve the doc and then set the header contents. 
//
//
// docid : the higthest 8 bytes used to identify the type of doc:
//    	0 : normal doc;
//    	1 : access record.
//    	2 : system record.
//    	3 : counter;
//    	...
//
// header:
//  0-3			// version (4bytes)
//	4-7  		// site (4bytes)
//	8-15		// docid (8bytes)
//	16-19       // doclocation seqno (4bytes)
//	20-27       // doclocation offset (8bytes)
//	27-33		// docsize (8bytes)
//	34-37	  	// system info location seqno (4bytes)
//	38-45	    // system info location offset (8bytes)
//	46-53		// system info location (8bytes)
// 	    
// body:
// 	normldoc :  docbody 
// 	systemdoc: 
// 		0-3:				countersize (4bytes)
// 		4-countersize:		counterbody (8bytes*n) 
//      next-next+4:    	admin doc size (4byte)
//      next-admindocsize   admin doc (nbyte)
//      next-next+4:    	access doc size (4byte)
//      next-accessdocsize  access doc (nbyte)
//   	
//
//
// Modification History:
// 12/07/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/XmlDoc.h"

#include "alarm_c/alarm.h"
// #include "IdGen/U64IdGen.h"
// #include "IdGen/IdGen.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "DfmUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/DocTags.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "XmlUtil/SeXmlParser.h"
// #include "XmlInterface/XmlRc.h"
#include "Util/File.h"
#include "Util/Buff.h"
// #include "UtilComm/ConnBuff.h"


const u64 sgDocLocationMask = 0x00ffffffffffffffULL;
OmnMutexPtr 	 AosXmlDoc::smLock = new OmnMutex();

bool	AosXmlDoc::smNeedCompress = true;		
u32		AosXmlDoc::smCompressSize = AosXmlDoc::eCompressSize;

bool
AosXmlDoc::staticInit(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr tag = config->getFirstChild("xmldoc");
	if (!tag) return true;
	smNeedCompress = tag->getAttrBool("need_compress", true);
	smCompressSize = tag->getAttrInt("compress_min_size", eCompressSize);
	if (smCompressSize < eCompressSize) smCompressSize = eCompressSize;
	return true;
}


bool
AosXmlDoc::stop()
{
    return true;
}


bool
AosXmlDoc::saveToFile(
		const u64 &docid,
		const AosXmlTagPtr &doc,
		const bool savenew,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc->getAttrStr(AOSTAG_FULLDOC_SIGNATURE) == "", false);

	AosDocType::E type = AosDocType::getDocType(docid);
	u64 did = getOwnDocid(docid);

	u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
	char *data = (char *)doc->getData();
	u32 data_len = doc->getDataLength();

	AosBuffPtr doc_buff = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs);
	doc_buff->setBuff(data, data_len);

	bool rslt = saveToSnapshotFilePriv(did, type, siteid, doc_buff, docfilemgr, snap_id, trans_id, rdata);
/*
	if (snap_id) {
		rslt = saveToSnapshotFilePriv(did, type, siteid, doc_buff, docfilemgr, snap_id, trans_id, rdata);
	}
	else {
		rslt = saveToFilePriv(did, type, siteid, doc_buff, docfilemgr, trans_id, rdata);
	}
*/
	return rslt;
}


bool
AosXmlDoc::saveToFile(
		const u64 &docid,
		const char *doc,
		const int64_t &data_len,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	//Linda, 2013/06/03
	aos_assert_r(doc, false);
	aos_assert_r(data_len > 0, false);

	AosDocType::E type = AosDocType::getDocType(docid);
	u64 did = getOwnDocid(docid);

	AosBuffPtr doc_buff = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs);
	doc_buff->setBuff(doc, data_len);
	u32 siteid = rdata->getSiteid();

/*
	if (snap_id == 0)
	{
		return saveToFilePriv(did, type, siteid, doc_buff, docfilemgr, trans_id, rdata);
	}
*/
	return saveToSnapshotFilePriv(did, type, siteid, doc_buff, docfilemgr, snap_id, trans_id, rdata);
}


bool
AosXmlDoc::deleteDoc(
		const u64 &docid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	// Currently we simply mark it as being deleted.
	// In the future, we need to reuse the space allocated for
	// the doc, and if possible, reuse the docid.
	
	if (docid == 0)
	{
		rdata->setError() << "To delete a doc but the Docid is null";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosDocType::E type = AosDocType::getDocType(docid);
	u64 did = getOwnDocid(docid);

	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, did, false);
/*
	if (snap_id) {
		p_doc = docfilemgr->readDoc(rdata, snap_id, did, false);
	}
	else {
		p_doc = docfilemgr->readDoc(rdata, did, false);
	}
*/
	if(!p_doc)
	{
		OmnScreen << "XmlDoc: doc not exist." 
				<< "; docid: " << did
				<< "; type:" << type
				<< "; loc_did: " << docid
				<< "; trans_id: " << trans_id.toString()
				<< endl;
		return false;
	}

	AosDfmDocDataletPtr doc = (AosDfmDocDatalet *)p_doc.getPtr();

	OmnScreen << "XmlDoc: delete doc:"
			<< "; docid: " << did
			<< "; loc_did: " << docid
			<< "; trans_id: " << trans_id.toString()
			<< endl;

	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);

	//bool rslt = docfilemgr->deleteDoc(rdata, trans_id, doc);
	bool rslt = docfilemgr->deleteDoc(rdata, snap_id, doc, trans_ids);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosXmlDoc::isDocDeleted(
		const u64 &docid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid != AOS_INVDID, true);

	// Convert the docid
	u64 did = getOwnDocid(docid);

	AosDfmDocPtr  p_doc = docfilemgr->readDoc(rdata, snap_id, did, false);
	//AosDfmDocPtr  p_doc = docfilemgr->readDoc(rdata, did, false);
	if(!p_doc)	return true;
	return false;
}


AosXmlTagPtr
AosXmlDoc::readFromFile(
		const u64 &docid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	if (docid == AOS_INVDID)
	{
		rdata->setError() << "To read a doc but the docid is NULL";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	u64 did = getOwnDocid(docid);
	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, did);
	//AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, did);
	if(!p_doc) return 0;
	
	AosBuffPtr body_buff = p_doc->getBodyBuff();
	aos_assert_r(body_buff, 0);
	
	AosXmlParser parser;
	AosXmlTagPtr xml_doc = parser.parse(body_buff->data(), body_buff->dataLen(),
		"" AosMemoryCheckerFileLine);
	aos_assert_r(xml_doc, 0);
	return xml_doc;
}


OmnString
AosXmlDoc::readHeader(
		const u64 &docid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid != AOS_INVDID, "Invalid Docid");
	
	u64 did = getOwnDocid(docid);
	
	//AosDfmDocPtr  p_doc = smDfmDoc->clone(did);
	//bool rslt = docfilemgr->readDoc(rdata, p_doc, false);
	//aos_assert_rr(rslt, rdata, "");
	
	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, did, false);	
//	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, did, false);	
	if(!p_doc)	return "";
	AosDfmDocDataletPtr doc = (AosDfmDocDatalet *)p_doc.getPtr();
	
	// Ketty 2013/01/15
	OmnNotImplementedYet;
	//read body information from header	
	OmnString str;
	//str << "Docid: " << docid
	//	<< "Version : " << header_buff->getU32(eInvVersion)
	//	<< ". Siteid: " << header_buff->getU32(AOS_INVSID)
	//	<< ". Docid: " << header_buff->getU64(0)
	//	<< ". Seqno: " << header_buff->getU32(eAosInvFseqno)
	//	<< ". Offset: " << header_buff->getU64(eAosInvFoffset)
	//	<< ". Docsize: " << header_buff->getU64(eAosInvDocSize); 
    return str;
}


bool
AosXmlDoc::saveBinaryDoc(
		const u64 &docid,
		const AosBuffPtr &docBuff,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	u64 did = getOwnDocid(docid);
	u32 siteid = rdata->getSiteid();
	AosDocType::E type = AosDocType::eNormalDoc;

	return saveToSnapshotFilePriv(did, type, siteid, docBuff, docfilemgr, snap_id, trans_id, rdata);
	//return saveToFilePriv(did, type, siteid, docBuff, docfilemgr, trans_id, rdata);
}


AosBuffPtr
AosXmlDoc::readBinaryDoc(
		const u64 &docid, 
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosRundataPtr &rdata)
{
	if (docid == AOS_INVDID)
	{
		rdata->setError() << "To read a doc but the docid is NULL";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	u64 did = getOwnDocid(docid);

	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, did);
	//AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, did);
	if(!p_doc) 	return 0;
	
	AosDfmDocDataletPtr doc = (AosDfmDocDatalet *)p_doc.getPtr();

	AosBuffPtr body_buff = doc->getBodyBuff();
	aos_assert_r(body_buff, 0);
	return body_buff;
}


bool
AosXmlDoc::saveToFilePriv(
		const u64 loc_did,
		const AosDocType::E type,
		const u32 siteid,
		const AosBuffPtr &doc_buff, 
		const AosDocFileMgrObjPtr &docfilemgr,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	if (!siteid)
	{
		rdata->setError() <<"Siteid is empty!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, loc_did, false);
	if(!p_doc)
	{
		// means It's a new doc.
		p_doc = OmnNew AosDfmDocDatalet(loc_did, loc_did, siteid);
	}
	
	aos_assert_r(p_doc, false);
	p_doc->setBodyBuff(doc_buff);
	p_doc->setNeedCompress( isCompress(doc_buff->dataLen()) );
	
	return docfilemgr->saveDoc(rdata, trans_id, p_doc);
}


bool
AosXmlDoc::saveToSnapshotFilePriv(
		const u64 loc_did,
		const AosDocType::E type,
		const u32 siteid,
		const AosBuffPtr &doc_buff, 
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosTransId &trans_id,
		const AosRundataPtr &rdata)
{
	//aos_assert_r(snap_id, false);
	if (!siteid)
	{
		rdata->setError() <<"Siteid is empty!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, loc_did, false);
	if(!p_doc)
	{
		// means It's a new doc.
		p_doc = OmnNew AosDfmDocDatalet(loc_did, loc_did, siteid);
	}
	
	aos_assert_r(p_doc, false);
	p_doc->setBodyBuff(doc_buff);
	p_doc->setNeedCompress( isCompress(doc_buff->dataLen()) );
	
	vector<AosTransId> trans_ids;
	trans_ids.push_back(trans_id);
	return docfilemgr->saveDoc(rdata, snap_id, p_doc, trans_ids);
}


// Chen Ding, 2015/05/28
// Moved to .h
// u64
// AosXmlDoc::getOwnDocid(const u64 &docid)
// {
// 	u64 did = docid & sgDocLocationMask;
// 
// 	return did;
// }                                       


bool
AosXmlDoc::isCompress(const u64 &docsize)
{
	if (smNeedCompress && docsize >= smCompressSize) return true;
	return false;
}


AosBuffPtr
AosXmlDoc::readFromFileTester(
		const u64 &docid,
		const AosDocFileMgrObjPtr &docfilemgr,
		const u64 &snap_id,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	// This function is used by torturer. 
	aos_assert_r(docid, 0);
	u64 did = getOwnDocid(docid);
	AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, snap_id, did);
	//AosDfmDocPtr p_doc = docfilemgr->readDoc(rdata, did);
	if(!p_doc) return 0;
	
	AosDfmDocDataletPtr doc = (AosDfmDocDatalet *)p_doc.getPtr();
	AosBuffPtr body_buff = doc->getBodyBuff();
	aos_assert_r(body_buff, 0);
	return body_buff;
}


