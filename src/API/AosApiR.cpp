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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiR.h"

#include "alarm_c/alarm.h"
#include "aosDLL/DllMgr.h"
#include "API/AosApiC.h"
#include "API/AosApiG.h"
#include "Alarm/Alarm.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/BitmapTreeObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/JQLParserObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/ValueSelObj.h"
#include "SEInterfaces/IILObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapStorageMgrObj.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "Thread/ThreadPool.h"
#include "TransServer/TransServer.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"
#include <signal.h>


AosIILClientObjPtr g__AosIILClient;
AosDocClientObjPtr g__AosDocClient;

AosXmlTagPtr AosRetrieveDocByObjid(
		const OmnString &objid, 
		const bool force_retrieve,
		const AosRundataPtr &rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	if (force_retrieve)
	{
		g__AosDocClient->purgeDoc(objid, rdata);
	}
	return g__AosDocClient->getDocByObjid(objid, rdata);
}


AosXmlTagPtr AosRetrieveDocByDocid( 
		const u64 &docid, 
		const bool force_retrieve,
		const AosRundataPtr &rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	if (force_retrieve)
	{
		g__AosDocClient->purgeDoc(docid, rdata);
	}
	return g__AosDocClient->getDocByDocid(docid, rdata);
}


AosXmlTagPtr AosRetrieveDocByCloudId( 
		const OmnString &cid,
		const bool force_retrieve,
		AosRundata *rdata)
{
	if (!g__AosDocClient) g__AosDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(g__AosDocClient, 0);
	if (force_retrieve)
	{
		g__AosDocClient->purgeDocByCloudid(cid, rdata);
	}
	return g__AosDocClient->getDocByCloudid(cid, rdata);
}


AosIILObjPtr AosRetrieveIIL(
			const OmnString &iilname,
			const bool read_only,
			const int timer,
			AosRundata *rdata)
{
	// Ketty 2012/05/12
	if (!g__AosIILClient) g__AosIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(g__AosIILClient, 0);
	OmnNotImplementedYet;
	return 0;
}


AosIILObjPtr AosRetrieveIIL(
			const u64 &iilid,
			const bool read_only,
			const int timer,
			AosRundata *rdata)
{
	// Ketty 2012/05/12
	if (!g__AosIILClient) g__AosIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(g__AosIILClient, 0);
	OmnNotImplementedYet;
	return 0;
}


bool AosReturnIIL(const AosIILObjPtr &iil)
{
	// Ketty 2012/05/12
	if (!g__AosIILClient) g__AosIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(g__AosIILClient, false);
	OmnNotImplementedYet;
	return false;
}



OmnString AosRetrieveServerInfo(const AosXmlTagPtr &doc, AosRundata *rdata)
{
	OmnNotImplementedYet;
	return "";
}


/*
bool AosRegisterServerListener(
		const AosModuleId::E module_id, 
		const AosTransProcPtr &listener)
{
	// Ketty 2012/05/17
	if(!sgTransServer)
	{
		//sgTransServer = AosGetTransServer();
	}
	if(sgTransServer)
	{
		return sgTransServer->registerListener(module_id, listener);
	}
	return true;
}
*/

bool AosRemoveKeyedU64Value(
		const u64 &iilid,
		const u64 &key,
		AosRundata *rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);

	u64 docid = 0;
	return iilclient->removeU64ValueDoc(iilid, key, docid, rdata);
}


bool AosRemoveDataRecordBySizeId(
		const u32 siteid, 
		const u64 &sizeid, 
		const bool flag,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->removeDataRecordBySizeId(siteid, sizeid, flag, rdata);
}


AosXmlTagPtr AosRetrieveBinaryDoc(
		const OmnString &objid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docClient = AosDocClientObj::getDocClient();
	aos_assert_r(docClient, 0);

	return docClient->retrieveBinaryDoc(objid, buff, rdata);
}

AosXmlTagPtr AosRetrieveBinaryDoc(
		const u64 &docid, 
		AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docClient = AosDocClientObj::getDocClient();
	aos_assert_r(docClient, 0);

	return docClient->retrieveBinaryDoc(docid, buff, rdata);
}
AosXmlTagPtr AosRetrieveBinaryDoc(
		const OmnString &objid, 
		AosBuffPtr	&buff, 
		u64			&startpos,
		u64			&needlen,
		u64			&readsize,
		const AosRundataPtr &rdata)
{
	// create by xulei.
	OmnNotImplementedYet;
	return 0;
}
		

AosDocFileMgrObjPtr AosRetrieveDocFileMgr(
		AosRundata *rdata,
		const u32   virtual_id,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	//return obj->retrieveDocFileMgr(virtual_id, module_id, proc, rdata);
	return vfsMgr->retrieveDocFileMgr(rdata, virtual_id, config);
}


// Chen Ding, 2013/02/08
/*
void AosReturnBitmap(const AosBitmapObjPtr &bitmap)
{
	AosBitmapObjPtr obj = AosBitmapObj::getObject();
	aos_assert(obj);
	obj->returnBitmap(bitmap);
}
*/

// Chen Ding, 2013/02/14
bool AosResetKeyedValue(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis, 
		const u64 &incValue)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->resetKeyedValue(rdata, iilname, key, value, persis, incValue);
}


// Chen Ding, 2013/03/03
AosBitmapTreeObjPtr AosRetrieveBitmapTree(
		AosRundata *rdata, 
		const u64 &tree_id)
{
	OmnNotImplementedYet;
	return 0;
}


bool AosRunQuery(
		AosRundata *rdata,
		const int startidx, 
		const int psize,
	 	const AosXmlTagPtr &query,
		AosQueryRsltObjPtr &query_rslt)
{
	// This function queries the database. The input is 'query':
	// 	<query>
	// 		<term AOSTAG_QUERY_CONTAINER_OBJID="xxx"
	// 			AOSTAG_QUERY_ATTRNAME="xxx"
	// 			AOSTAG_QUERY_OPR="xxx"
	// 			AOSTAG_QUERY_REVERSE="true/false"
	// 			AOSTAG_QUERY_ORDER="true/false">
	// 			value
	// 		</term>
	// 		...
	// 	</query>
	AosQueryObjPtr q = AosQueryObj::getQueryObj();
	aos_assert_rr(q, rdata, false);
	return q->runQuery(rdata, startidx, psize, query, query_rslt);
}


bool AosRunSmartDoc(
		const AosRundataPtr &rdata, 
		const OmnString &sdoc_objid)
{
	return AosSmartDocObj::procSmartdocsStatic(sdoc_objid, rdata);
}


bool AosRunSmartDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	return AosSmartDocObj::runSmartdocStatic(sdoc, rdata);
}


bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc)
{
	aos_assert_rr(worker_doc, rdata, false);
	aos_assert_rr(jimo_doc, rdata, false);
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);

	if (!jimo)
	{
		AosSetErrorU(rdata, "failed_retrieve_jimo") << jimo_doc->toString() << enderr;
		return false;
	}

	return jimo->run(rdata, worker_doc);
}


bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc) 
{
	AosXmlTagPtr jimo_doc;
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_retrieve_jimo") 
			<< ". Worker: " << worker_doc->toString() << enderr;
		return false;
	}

	return jimo->run(rdata, worker_doc);
}


bool AosRunJimo(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker_doc, 
		const OmnString &jimo_objid) 
{
	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_objid);
	if (!jimo)
	{
		AosSetErrorUser(rdata, "failed_retrieve_jimo") 
			<< "Jimo Objid: " << jimo_objid 
			<< ". Worker: " << worker_doc->toString() << enderr;
		return false;
	}

	return jimo->run(rdata, worker_doc);
}


AosXmlTagPtr AosRunDocSelector(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const OmnString &tagname) 
{
	AosDocSelObjPtr obj = AosDocSelObj::getDocSelector();
	aos_assert_rr(obj, rdata, 0);
	return obj->selectDoc(worker_doc, tagname, rdata);
}


AosXmlTagPtr AosRunDocSelector(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	AosDocSelObjPtr obj = AosDocSelObj::getDocSelector();
	aos_assert_rr(obj, rdata, 0);
	return obj->selectDoc(worker_doc, rdata);
}


bool AosRunAction(
		AosRundata *rdata, 
		const OmnString &action_id,
		const AosXmlTagPtr &worker_doc)
{
	return AosActionObj::runAction(rdata, action_id, worker_doc);
}

bool AosRunAction(
		AosRundata *rdata, 
		const OmnString &act_def)
{
	return AosActionObj::runAction(rdata, act_def);
}

bool AosReadDataFile(
		AosRundata *rdata, 
		AosBuffPtr &buff,
		const int phy_id,
		const OmnString &fname, 
		const int64_t start_pos,
		const int64_t bytes_to_read, 
		AosDiskStat &disk_stat)
{
	return AosNetFileCltObj::readFileToBuffStatic(
			fname, phy_id, start_pos, bytes_to_read, buff, disk_stat, rdata);
}


// Chen Ding, 2013/12/15
AosXmlTagPtr AosGetReferencedDoc(
		AosRundata *rdata, 
		const AosXmlTagPtr &tag)
{
	aos_assert_rr(tag, rdata, 0);
	OmnString objid = tag->getAttrStr(AOSTAG_REFERENCED_OBJID);
	if (objid != "")
	{
		return AosGetDocByObjid(objid, rdata);
	}

	u64 docid = tag->getAttrU64(AOSTAG_REFERENCED_DOCID, 0);
	if (docid != 0)
	{
		return AosGetDocByDocid(docid, rdata);
	}

	OmnString name = tag->getAttrStr(AOSTAG_RUNDATA_DOC_NAME);
	if (name != "")	
	{
		return rdata->getDocByVar(name);
	}

	return 0;
}


AosXmlTagPtr AosResolveReference(
		AosRundata *rdata, 
		const AosXmlTagPtr &tag)
{
	// This function assumes the tag may refer to another doc/tag. 
	// If its AOSTAG_REFERENCE_ID or AOSTAG_REFERENCE_TYPE is empty, 
	// return the tag itself.
	//
	// 1. If AOSTAG_REFERENCE_TYPE is AOSTAG_EXTENSION_DOC, 'extension_doc'
	//    should not be null and it should contain a root-level tag
	//    whose attribute AOSTAG_EMBEDDED_ID matches its 
	//    AOSTAG_REFERENCE_ID. If not found, it is an error. Otherwise, 
	//    it returns the tag.
	// 2. If AOSTAG_REFERENCE_TYPE is AOSTAG_REFERED_OBJID, it refers
	//    to a database doc. It returns that doc.
	// 3. If AOSTAG_REFERENCE_TYPE is AOSTAG_RUNDATA, it refers to 
	//    a rundata doc.
	aos_assert_rr(tag, rdata, 0);

	OmnString type = tag->getAttrStr(AOSTAG_REFERENCE_TYPE);
	if (type == "") type = AOSTAG_REFERENCED_OBJID;

	OmnString id = tag->getAttrStr(AOSTAG_REFERENCED_ID);
	if (id == "" && type == "") return tag;

	if (id == "" && type != "")
	{
		AosSetErrorUser3(rdata, "syntax_error", "aosapir_missing_id") << tag << enderr;
		return 0;
	}

	if (id != "" && type == "")
	{
		AosSetErrorUser3(rdata, "syntax_error", "aosapir_missing_type") << tag << enderr;
		return 0;
	}

	if (type == AOSTAG_EXTENSION_DOC)
	{
		// The referenced is a tag in a doc. 
		AosXmlTagPtr ref_doc = AosGetReferencedDoc(rdata, tag);
		if (!ref_doc)
		{
			AosSetErrorUser3(rdata, "syntax_error", "aosapir_miss_refdoc") << tag << enderr;
			return 0;
		}

		AosXmlTagPtr entry = ref_doc->getFirstChild();
		while (entry)
		{
			if (entry->getAttrStr(AOSTAG_EMBEDDD_ID) == id)
			{
				return entry->clone(AosMemoryCheckerArgsBegin);
			}
			entry = ref_doc->getNextChild();
		}

		AosSetErrorUser3(rdata, "syntax_error", "aosapir_ref_not_found") << tag << enderr;
		return 0;
	}
	
	if (type == AOSTAG_REFERENCED_OBJID)
	{
		AosXmlTagPtr doc = AosGetDocByObjid(id, rdata);
		if (doc) return doc;
		
		AosSetErrorUser3(rdata, "syntax_error", "aosapir_objdoc_notfound") << tag << enderr;
		return 0;
	}

	if (type == AOSTAG_REFERENCED_DOCID)
	{
		u64 docid = AosStr2U64(id);
		AosXmlTagPtr doc = AosGetDocByDocid(docid, rdata);
		if (doc) return doc;
		
		AosSetErrorUser3(rdata, "syntax_error", "aosapir_dociddoc_notfound") << tag << enderr;
		return 0;
	}

	if (type == AOSTAG_RUNDATA_DOC)
	{
		AosXmlTagPtr doc = rdata->getDocByVar(id);
		if (doc) return doc;

		AosSetErrorUser3(rdata, "syntax_error", "aosapir_rdatadoc_notfound") << tag << enderr;
		return 0;
	}

	AosSetErrorUser3(rdata, "syntax_error", "aosapir_unrecog_type") << tag << enderr;
	return 0;
}


bool AosRunProcsSync(
		AosRundata *rdata, 
		const vector<OmnThrdShellProcPtr> runners)
{
	return OmnThreadPool::runProcSync(runners);
}

bool AosRunProcsAsync(
		AosRundata *rdata, 
		const vector<OmnThrdShellProcPtr> runners)
{
	return OmnThreadPool::runProcAsync(runners);
}

bool AosRemoveIndexEntry(
		AosRundata *rdata,
		const OmnString &table_name,
		const OmnString &field_name)
{
	return AosIndexMgrObj::removeEntryStatic(rdata, table_name, field_name);
}


bool AosRemoveStrValueDoc(
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->removeStrValueDoc(iilname, value, docid, rdata);
}

