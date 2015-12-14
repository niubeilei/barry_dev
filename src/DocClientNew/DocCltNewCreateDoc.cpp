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
// 07/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClientNew/DocClientNew.h"

/*#include "API/AosApi.h"
#include "CompressUtil/Compress.h"
#include "DocClientNew/DocProc.h"
#include "DocClientNew/DocidMgr.h"
#include "DocClientNew/DocTransHandler.h"
#include "DocServer/DocSvr.h"
// #include "DocTrans/CreateDocTrans.h"
// #include "DocTrans/CreateDocSafeTrans.h"
#include "ErrorMgr/ErrmsgId.h"
#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "IdGen/IdGenMgr.h"
#include "Rundata/RdataUtil.h"
#include "SeLogClient/SeLogClient.h"
#include "SEUtil/SysLogName.h"
#include "SEUtil/FieldName.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/ObjidSvr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEUtilServer/CloudidSvr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RemoteBkCltObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SmartDoc/SMDMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Thread/LockMonitor.h"
// #include "TransClient/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/SeXmlUtil.h"
#include "SEUtil/IILName.h"
#include "SEInterfaces/TaskObj.h"
#include "Util/Opr.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "StorageEngine/SengineDocInfoCSV.h"
#include "StorageEngine/SengineDocInfoFixed.h"
#include "StatUtil/StatIdIDGen.h"
#include "StatUtil/StatDefineDoc.h"
#include "StatUtil/Statistic.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatTrans/BatchSaveStatDocsTrans.h"
static AosDocMgrObjPtr sgDocMgr;
*/

#include "JimoAPI/JimoDocEngine.h"
#include "SEInterfaces/DataRecordObj.h"


bool
AosDocClientNew::createDoc1(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &cmd, 
		const OmnString &target_cid,
		const AosXmlTagPtr &newdoc,
		const bool resolveObjid,
		const AosDocSvrCbObjPtr &caller, 
		void *userdata,
		const bool keepDocid) 
{
	return Jimo::jimoCreateDoc1(rdata.getPtr(), cmd, target_cid, newdoc, resolveObjid, caller, userdata, keepDocid);
}


AosXmlTagPtr
AosDocClientNew::createMissingContainer(
		const AosRundataPtr &rdata,
		const OmnString &container_objid, 
		const bool ispublic) 
{
	OmnNotImplementedYet; 
	return 0;             
/*
	// This function creates the missing container 'container_objid'.
	// AOSMONITORLOG_ENTER(rdata);
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, NULL);

	// AOSMONITORLOG_LINE(rdata);
	aos_assert_r(container_objid.length() > 0, 0);

	OmnString prefix, ctnr_cid;
	OmnString ctnr_objid = container_objid;
	AosObjid::decomposeObjid(ctnr_objid, prefix, ctnr_cid);

	u64 userid  = rdata->getUserid();
	OmnString caller_cid = AosCloudidSvr::getCloudid(userid, rdata);

	// If the container is not public but the container does not 
	// contain the caller's cloud id or the cloud id is not the
	// caller's cid, append the caller's cloud id to it.
	OmnString parent_ctnr_objid;
	OmnString stype;
	bool rslt = true;
	bool is_public = ispublic;
	if (!is_public)
	{
		// It is a private container. The 'ctnr_cid' must not be empty.
		// AOSMONITORLOG_LINE(rdata);
		if (ctnr_cid.length() == 0)
		{
			rdata->setError() << "Container Cloudid is empty: " << container_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			// AOSMONITORLOG_FINISH(rdata);
			return 0;
		}
		
		AosXmlTagPtr userdoc;
		if (userid) userdoc = getDocByDocid(userid, rdata);
		if (userdoc)
		{
			// AOSMONITORLOG_LINE(rdata);
			parent_ctnr_objid = userdoc->getAttrStr(AOSTAG_CTNR_HOME);
			if (prefix == parent_ctnr_objid)
			{
				// This means that 'ctnr_objid' is the parent container. 
				// Need to set the parent container to AOSCTNR_CID
				// AOSMONITORLOG_LINE(rdata);
				parent_ctnr_objid = AOSCTNR_CID;
			}
		}

		if (parent_ctnr_objid == "")
		{
			// This means 'userid' is 0 or it is not a valid userid. 
			// AOSMONITORLOG_LINE(rdata);
			parent_ctnr_objid = AosObjid::compose(AOSOBJIDPRE_LOSTaFOUND, ctnr_cid);
		}

		// Since it is a private doc, its parent container should be either the user's
		// home container (if we can find the user doc) or the container cid's 
		// Lost and Found container.
		AosXmlTagPtr pctnr_doc = getDocByObjid(parent_ctnr_objid, rdata);
		if (!pctnr_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< parent_ctnr_objid << enderr;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		
		if (!mIsRepairing)
		{
			rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddMember1(pctnr_doc, rdata);
			if (!rslt)
			{
	 			// Access Denied
				// AOSMONITORLOG_FINISH(rdata);
	 			return 0;
			}
		}
		
		stype = AOSSTYPE_AUTO;
	}
	else
	{
		// AOSMONITORLOG_LINE(rdata);

		// Since it is a public container, we will not change the container's objid.
		if (!mIsRepairing)
		{
			rslt = AosSecurityMgrObj::getSecurityMgr()->checkAddPubObjid(ctnr_objid, rdata);
			if (!rslt)
			{
				// AOSMONITORLOG_FINISH(rdata);
				return 0;
			}
		}

		parent_ctnr_objid = AosObjid::composeLostFoundCtnrObjid(siteid);

		if (!mObjMgrObj) mObjMgrObj = AosObjMgrObj::getObjMgr();
		aos_assert_r(mObjMgrObj, 0);
		AosXmlTagPtr parent_doc = mObjMgrObj->createLostFoundCtnr(rdata);
		if (!parent_doc)
		{
			AosSetErrorU(rdata, "eContainerNotFound") 
				<< parent_ctnr_objid << enderr;
			return 0;
		}

		stype = AOSSTYPE_LOSTaFOUND;
	}

	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	AosXmlTagPtr doc;
	OmnString sobjids = "";
	if(cmd)
	{
		sobjids = cmd->getAttrStr(AOSTAG_SDOCCTNR);
		cmd->removeAttr(AOSTAG_SDOCCTNR);
	}

	if(sobjids != "")
	{
		// AOSMONITORLOG_LINE(rdata);
		rdata->setArg1(AOSARG_CTNR_OBJID, ctnr_objid);
		rdata->setArg1(AOSARG_PARENT_CTNR_OBJID, parent_ctnr_objid);
		rdata->setArg1(AOSARG_CTNR_SUBTYPE, stype);

		OmnStrParser1 parser(sobjids, ",");
		OmnString sdocid;
		AosSmartDocObjPtr smtobj = AosSmartDocObj::getSmartDocObj();
		aos_assert_r(smtobj, 0);

		while ((sdocid = parser.nextWord()) != "")
		{
			smtobj->runSmartdocs(sdocid, rdata);
			doc = rdata->getRetrievedDoc();
		}

		// Check whether the container was created by these smart docs.
		doc = getDocByObjid(ctnr_objid, rdata);
	}
	
	if (!doc)
	{
		// AOSMONITORLOG_LINE(rdata);
		OmnString docstr = "<container ";
		docstr << AOSTAG_OBJID << "=\"" << ctnr_objid<< "\" "
			<< AOSTAG_SITEID << "=\"" << siteid << "\" "
			<< AOSTAG_OTYPE << "=\"" << AOSOTYPE_CONTAINER << "\" "
			<< AOSTAG_PARENTC << "=\"" << parent_ctnr_objid << "\" "
			<< AOSTAG_CREATOR << "=\"" << caller_cid << "\" "
			<< AOSTAG_STYPE << "=\"" << stype 
			<< "\"/>";
		doc = createDocSafe1(rdata, docstr, "", "",
			true, false, false, false, false, false);
		if (!doc)
		{
			AosSetErrorU(rdata, "failed_creating_doc") 
				<< ":" << docstr << enderr;
			return 0;
		}

		OmnString objid = doc->getAttrStr(AOSTAG_OBJID);
		AosXmlTagPtr doc_tt = getDocByObjid(objid, rdata);
		aos_assert_r(doc_tt, 0);
		doc_tt = 0;
	}
		
	if (!doc)
	{
		OmnAlarm << "Failed to create missing container: " 
				<< ctnr_objid<< enderr;
		// AOSMONITORLOG_FINISH(rdata);
		return 0;
	}
		
	rdata->setOk();
	// AOSMONITORLOG_FINISH(rdata);
	return doc;
	*/
}


AosXmlTagPtr
AosDocClientNew::cloneDoc(
		const AosRundataPtr &rdata,
		const OmnString &cloudid,
		const OmnString &fromobjid, 
		const OmnString &toobjid, 
		const bool is_public)
{
	return Jimo::jimoCloneDoc(rdata.getPtr(), cloudid, fromobjid, toobjid, is_public);
}


AosXmlTagPtr 
AosDocClientNew::createDocSafe1(
		const AosRundataPtr &rdata,
		const OmnString &docstr,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid, 
		const bool reserved, 
		const bool cid_required, 
		const bool check_ctnr) 
{
	return Jimo::jimoCreateDocSafe1(rdata.getPtr(), docstr, cloudid, objid_base, is_public, checkCreation, keepDocid, reserved, cid_required, check_ctnr); 
}


AosXmlTagPtr
AosDocClientNew::createDocSafe3(
		const AosRundataPtr &rdata_ptr,
		const AosXmlTagPtr &newdoc,
		const OmnString &cloudid,
		const OmnString &objid_base,
		const bool is_public,
		const bool checkCreation,
		const bool keepDocid, 
		const bool reserved, 
		const bool cid_required, 
		const bool check_ctnr, 
		const bool saveDocFlag)
{
	return Jimo::jimoCreateDocSafe3(rdata_ptr.getPtr(), newdoc, cloudid, objid_base, is_public, checkCreation, keepDocid, reserved, cid_required, check_ctnr, saveDocFlag);
}


AosXmlTagPtr
AosDocClientNew::createDocByTemplate1(
		const AosRundataPtr &rdata,
		const OmnString &cid,
		const OmnString &objid,
		const OmnString &template_objid)
{
	return Jimo::jimoCreateDocByTemplate1(rdata.getPtr(), cid, objid, template_objid);
}


AosXmlTagPtr
AosDocClientNew::createRootCtnr(
		const OmnString &docstr, 
		const AosRundataPtr &rdata_ptr)
{
	return Jimo::jimoCreateRootCtnr(rdata_ptr.getPtr(), docstr);
}


bool
AosDocClientNew::createTempDoc(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;         
	/*
	// It saves the temporary doc into a file and creates a new objid
	// for the doc. The new objid is:
	if (AosDocClientNew::getSelf()->isRepairing())
	{
		return true;
	}

	aos_assert_r(doc, false);

	OmnString otype = doc->getAttrStr(AOSTAG_OTYPE);
	if (otype == AOSOTYPE_LOG || otype == AOSOTYPE_BINARYDOC)
	{
		return true;
	}

	// u64 docid = 0;

	// Ketty 2014/07/04
	return true;
	aos_assert_r(mSeLogClientObj, false);
	// Chen Ding, 2013/01/06
	// docid = mSeLogClientObj->addLogWithResp(
	// 		AOSCTNR_TEMPOBJ, AOSSYSLOGNAME_TEMPOBJ, doc->toString(), rdata);
	bool rslt = mSeLogClientObj->addLog(rdata, AOSCTNR_TEMPOBJ, 
		AOSSYSLOGNAME_TEMPOBJ, doc->toString());
	if (!rslt)
	{
		OmnAlarm << "Failed creating temp object: " << rdata->getErrmsg() << enderr;
		return false;
	}

	// Chen Ding, 2013/01/06
	// Need to rework on it!!!!!!!!!!!!!!!!
	// AosSetError(rdata, "create_tmpdoc") << ": " << docid;
	return true;
	*/
}


AosXmlTagPtr 
AosDocClientNew::createBinaryDoc(
		const OmnString &docstr,
		const bool is_public, 
		const AosBuffPtr &buff,
		const int &vid,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateBinaryDoc(rdata.getPtr(), docstr, is_public, buff, vid);
}


bool 
AosDocClientNew::appendBinaryDoc(
		const OmnString &objid, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDocClientNew::createJQLTable(	
		const AosRundataPtr &rdata,
		const OmnString &table_def)
{
	return Jimo::jimoCreateJQLTable(rdata.getPtr(), table_def);
}

bool
AosDocClientNew::createDocByJQL( 
		const AosRundataPtr &rdata,
		const OmnString &container_objid,
		const OmnString &objid,
		const OmnString &doc)
{
	return Jimo::jimoCreateDocByJQL(rdata.getPtr(), container_objid, objid, doc);
}


/*
 * Chen Ding, 2014/11/09
 * This function is no longer needed
AosXmlTagPtr 
AosDocClientNew::createBinaryDocByStat(
		const OmnString &docstr,
		const AosBuffPtr &buff,
		const int &vid,
		const u64 &normal_snap_id,
		const u64 &binary_snap_id,
		const AosRundataPtr &rdata)
{
	// doc format 
	// <doc zky_otype = AOSOTYPE_BINARYDOC>
	//		<![BDATA[buff data]]>
	// </doc>
	aos_assert_r(buff && buff->dataLen() > 0, 0);
	aos_assert_r(docstr != "", 0);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(docstr, "" AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);

	if (doc->getAttrStr(AOSTAG_OTYPE, "") != AOSOTYPE_BINARYDOC)
	{
		OmnAlarm << "zky_otype mismatch:" << doc->getAttrStr(AOSTAG_OTYPE, "") 
			<< ":" << AOSOTYPE_BINARYDOC << enderr;
		doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	}

	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
	doc->setTextBinary(nodename, buff);

	//OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	//u64 docid = AosDocidMgr::getSelf()->nextDocid(vid, objid, rdata);
	//doc->setAttr(AOSTAG_DOCID, docid);
	//doc->setAttr(AOSTAG_OBJID, objid);
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);	
	aos_assert_r(docid, 0);

	aos_assert_r(doc->getAttrStr(AOSTAG_OBJID) != "", 0);
	aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(docid)) == vid, 0);
	doc->setAttr(AOSTAG_SITEID, rdata->getSiteid());

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosCreateDocTrans(docid, doc, false, false, snap_id);
	bool rslt = addReq(rdata, trans);
	aos_assert_r(rslt, 0);
	aos_assert_r(rdata->isOk(), 0);

	return doc;
}

*/

bool
AosDocClientNew::insertInto(
		const AosXmlTagPtr &data,
		const AosRundataPtr &rdata)
{

	//		<datafield zky_name="key_field1"><![CATA[xxxx]]></datafield>
	//		<datafield zky_name="key_field2"><![CATA[xxxx]]></datafield>
	//		<datafield zky_name="key_field3"><![CATA[xxxx]]></datafield>
	//</data>
	//
	//table describe
	//<sqltable zky_def_index="true" zky_use_schema="_zt4g_schema_t1_paser" zky_database="db">
	//	<columns>
	//		<column name="key_field1" datatype="double" size="8" offset="0"></column>
	//	    <column name="key_field2" datatype="string" size="20" offset="8"></column>
	//	    <column name="key_field3" datatype="string" size="20" offset="28"></column>
	//		<column name="vf_field1" datatype="string" size="10" type="virtual">
	//			<keys>
	//				<key><![CDATA[`key_field1`]]></key>
	//			</keys>
	//			<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1" datatype="string">
	//				<keys>
	//					<key><![CDATA[`key_field1`]]></key>
	//				</keys>
	//				<values>
	//					<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//				</values>
	//			</map>
	//		</column>
	//	</columns>
	//	<indexes>
	//		<index zky_name="key_field1" zky_iilname="_zt44_idx_t1_key_field1" idx_name="_zt4g_idxmgr_idx_t1_key_field1"/>
	//	</indexes>
	//	<cmp_indexes>
	//		<cmp_index zky_iilname="_zt44_idx_t1_key_field1_key_field2_key_field3">
	//			<key_field1 zky_name="key_field1"/>
	//			<key_field2 zky_name="key_field2"/>
	//			<key_field3 zky_name="key_field3"/>
	//		</cmp_index>
	//	</cmp_indexes>
	//	<maps>
	//		<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1">
	//			<keys>
	//				<key><![CDATA[`key_field1`]]></key>
	//			</keys>
	//			<values>
	//				<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//			</values>
	//		</map>
	//	</maps>
	//	<statistic stat_doc_objid="_zt4g_statistics_t1_db"/>
	//<sqltable>
	//
/*	AosRundata* rdata_raw = rdata.getPtr();
	OmnString table_name = data->getAttrStr("zky_name", "");
	aos_assert_r(table_name != "", false);
	AosXmlTagPtr table_doc = AosGetDocByObjid(table_name, rdata);
	aos_assert_r(table_doc, false);

	AosDataRecordObjPtr record = createDataRecord(table_doc, data, rdata_raw);
	aos_assert_r(record, false);

	//insert group doc
	bool rslt = insertDoc(record, rdata_raw);
	aos_assert_r(rslt, false);

	//insert index
	AosXmlTagPtr index_tags = table_doc->getFirstChild("indexes");
	if (index_tags)
	{
		rslt = insertIndex(index_tags, record, rdata_raw);
		aos_assert_r(rslt, false);
	}

	//insert cmp index
	AosXmlTagPtr cmp_index_tags = table_doc->getFirstChild("cmp_indexes");
	if(cmp_index_tags)
	{
		rslt = insertCmpIndex(cmp_index_tags, record, rdata_raw);
		aos_assert_r(rslt, false);
	}

	//insert map
	AosXmlTagPtr maps_tag = table_doc->getFirstChild("maps");	
	if (maps_tag)
	{
		rslt = insertMap(maps_tag, record, rdata_raw);
	}

	//insert statistic
	//<statistic zky_stat_name="db_t1_stat_uw_st5" zky_objid="_zt4g_statistics_t1_db">
	//	<statistic_defs>
	//		<statistic zky_stat_conf_objid="_zt4k_uw_st5"/>
	//	</statistic_defs>
	//	<internal_statistics>
	//		<statistic zky_stat_identify_key="key_field3" zky_stat_conf_objid="db_t1_stat_uw_st5_internal_0"/>
	//	</internal_statistics>
	//</statistic>
	AosXmlTagPtr statistic_tag = table_doc->getFirstChild("statistic");
	if (statistic_tag)
	{
		OmnString db_name = table_doc->getAttrStr("zky_database", "");
		aos_assert_r(db_name != "", false);
		OmnString stat_objid = statistic_tag->getAttrStr("stat_doc_objid", "");
		aos_assert_r(stat_objid != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(stat_objid, rdata);
		aos_assert_r(stat_doc, false);
		AosXmlTagPtr norm_stat_tags = stat_doc->getFirstChild("statistic_defs");
		if (norm_stat_tags)
		{
			rslt = insertNormalStat(db_name, stat_doc, record, rdata_raw);
			aos_assert_r(rslt, false);
		}
		AosXmlTagPtr internal_stat_tags = stat_doc->getFirstChild("internal_statistics");
		if (internal_stat_tags)
		{
			rslt = insertInternalStat(internal_stat_tags, record, rdata_raw);
			aos_assert_r(rslt, false);
		}
	}
	return true;
	*/
	return Jimo::jimoInsertInto(rdata.getPtr(), data);
}

AosDataRecordObjPtr
AosDocClientNew::createDataRecord(
		const AosXmlTagPtr &table_doc,
		const AosXmlTagPtr &data,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;   
	return 0;
/*
	//dataschema describe
	//<dataschema zky_dataschema_type="static" zky_name="t1_paser">
	//	<datarecord type="csv" zky_field_delimiter="," zky_name="t1_schm" zky_row_delimiter="LF" zky_text_qualifier="NULL">
	//		<datafields>
	//			<datafield type="double" zky_length="8" zky_name="key_field1"/>
	//			<datafield type="str" zky_datatooshortplc="cstr" zky_length="20" zky_name="key_field2"/>
	//			<datafield type="str" zky_datatooshortplc="cstr" zky_length="20" zky_name="key_field3"/> 
	//		</datafields>
	//	</datarecord>
	//</dataschema>
	OmnString schema_name = table_doc->getAttrStr("zky_use_schema", "");
	aos_assert_r(schema_name != "", 0);
	AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_name, rdata_raw);
	aos_assert_r(schema_doc, 0);
	AosXmlTagPtr record_tag = schema_doc->getFirstChild(true);
	aos_assert_r(record_tag, 0);
	u64 record_docid = record_tag->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(record_docid, 0);


	AosXmlTagPtr clone_record_tag = record_tag->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(clone_record_tag, 0);
	vector<AosXmlTagPtr> virtual_field_tags;// = AosDataFieldObj::getVirtualField(table_doc, rdata_raw);
	AosXmlTagPtr datafieldsNode = clone_record_tag->getFirstChild("datafields");
	aos_assert_r(datafieldsNode, 0);

	set<OmnString> fieldnames;
	for (size_t i=0; i<virtual_field_tags.size(); i++)
	{
		OmnString fieldName = virtual_field_tags[i]->getAttrStr("zky_name", "");
		aos_assert_r(fieldName != "", 0);
		if (fieldnames.count(fieldName) == 0)
		{
			datafieldsNode->addNode(virtual_field_tags[i]);
			fieldnames.insert(fieldName);
		}
	}

	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(clone_record_tag, 0, rdata_raw);
	aos_assert_r(record, 0);
	AosBuffPtr buff = OmnNew AosBuff(1024*1024 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	record->setMemory(buff->data(), buff->buffLen());

	int i=0;
	OmnString field_name;
	OmnString field_value;
	AosXmlTagPtr field_tag = data->getFirstChild(true);
	while(field_tag)
	{
		field_name = field_tag->getAttrStr("zky_name", "");
		int idx = record->getFieldIdx(field_name, rdata_raw);
		aos_assert_r(idx == i, 0);
		field_value = field_tag->getNodeText();
		AosValueRslt value(field_value);
		bool outofmem = false;
		record->setFieldValue(idx, value, outofmem, rdata_raw);
		if (value.isNull())
		{
			AosDataFieldObj* datafield = record->getFieldByIdx1(idx);
			aos_assert_r(datafield, 0);
			AosStrValueInfo info = datafield->getFieldInfo();
			if (info.notnull)
			{
				return 0;
			}
		}

		i++;
		field_tag = data->getNextChild();
	}
	record->setDocid(AosGetNextDocid(record_docid, rdata_raw));
	return record;
	*/
}

bool
AosDocClientNew::insertDoc(
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;
	return false;        
	/*
	int rcd_len = record->getRecordLen();
	char * doc = record->getData(rdata_raw);
	u64 docid = record->getDocid();
	aos_assert_r(docid != 0, false);
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	buff->setU64(docid);
	buff->setInt(rcd_len);
	buff->setBuff(doc, rcd_len);
	buff->reset();

	u32 vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
	u64 snapshot_id = 0;
	snapshot_id = createSnapshot(rdata_raw, vid, snapshot_id,  AosDocType::eGroupedDoc, 0);
	docid = buff->getU64(0);
	aos_assert_r(docid, false);
	AosSengineDocInfoPtr doc_info;
	AosDataRecordType::E type = record->getType();
	if (type == AosDataRecordType::eCSV)
	{
		doc_info = OmnNew AosSengineDocInfoCSV(AosGroupDocOpr::eBatchInsert, vid, snapshot_id, 1);
	}
	else if (type == AosDataRecordType::eFixedBinary)
	{
		doc_info = OmnNew AosSengineDocInfoFixed(AosGroupDocOpr::eBatchInsert, vid, snapshot_id, 1);
	}
	else
	{
		OmnNotImplementedYet;
		return false;
	}
	int record_len = AosGetDataRecordLenByDocid(rdata_raw->getSiteid(), docid, rdata_raw);
	doc_info->createMemory(1024*1024*4, record_len);
	aos_assert_r(doc_info, false);

	int record_size = buff->getInt(0);                                                                                 
	aos_assert_r(record_size, false);
	int64_t offset = buff->getCrtIdx();
	bool rslt = doc_info->addDoc(buff->data() + offset, record_size, docid, rdata_raw);
	aos_assert_r(rslt, false);
	doc_info->sendRequestPublic(rdata_raw);
	return true;
	*/
}

bool
AosDocClientNew::insertIndex(
		const AosXmlTagPtr &index_tags,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;
	return false;        
	/*
	aos_assert_r(index_tags, false);
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	OmnString iil_name, field_name, type;
	int idx = -1;
	AosXmlTagPtr index_tag = index_tags->getFirstChild(true);
	while(index_tag)
	{
		type = index_tag->getAttrStr("zky_type", "");
		if (type == "cmp")
		{
			index_tag = index_tags->getNextChild();
			continue;
		}
		field_name = index_tag->getAttrStr("zky_name", "");
		aos_assert_r(field_name != "", false);
		idx = record->getFieldIdx(field_name, rdata_raw);
		aos_assert_r(idx != -1, false);

		AosValueRslt value;
		bool outofmem = false;
		record->getFieldValue(idx, value, outofmem, rdata_raw);
		//if (value.isNull())
		//{
		//	AosDataFieldObj* datafield = record->getFieldByIdx1(idx);
		//	aos_assert_r(datafield, false);
		//	AosStrValueInfo info = datafield->getFieldInfo();
		//	if (info.notnull)
		//	{
		//		continue;
		//	}
		//}

		iil_name = index_tag->getAttrStr("zky_iilname", "");
		aos_assert_r(iil_name != "", false);
		bool rslt = iil_client->addStrValueDoc(iil_name, value.getValueStr1(), record->getDocid(), false, true, rdata_raw);
		aos_assert_r(rslt, false);
		index_tag = index_tags->getNextChild();
	}
	return true;
	*/
}

bool
AosDocClientNew::insertCmpIndex(
		const AosXmlTagPtr &cmp_indexs_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;
	return false;        
	/*
	//	<cmp_indexes>
	//		<cmp_index zky_iilname="_zt44_idx_t1_key_field1_key_field2_key_field3">
	//			<key_field1 zky_name="key_field1"/>
	//			<key_field2 zky_name="key_field2"/>
	//			<key_field3 zky_name="key_field3"/>
	//		</cmp_index>
	//	</cmp_indexes>
	//[key0x01key, docid]
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	int idx = -1;
	u64 docid = record->getDocid();
	OmnString field_name;
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	AosXmlTagPtr cmp_index_tag = cmp_indexs_tag->getFirstChild(true);
	while(cmp_index_tag)
	{
		OmnString iil_name = cmp_index_tag->getAttrStr("zky_iilname");
		aos_assert_r(iil_name != "", false);
		OmnString key_str;
		int num = cmp_index_tag->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr field_tag = cmp_index_tag->getChild(i);
			aos_assert_r(field_tag, false);
			field_name = field_tag->getAttrStr("zky_name", "");
			aos_assert_r(field_name != "", false);
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			key_str << key_rslt.getValueStr1();
			if (i != num - 1)
			{
				key_str << key_sep;
			}
		}
		bool rslt = iil_client->addStrValueDoc(iil_name, key_str, docid, false, true, rdata_raw);
		aos_assert_r(rslt, false);
		cmp_index_tag = cmp_indexs_tag->getNextChild();
	}
	return true;
}

bool
AosDocClientNew::insertMap(
		const AosXmlTagPtr &maps_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	//<map zky_type="iilmap" zky_name="map1" zky_iilname="_zt44_map_t1_map1">
	//	<keys>
	//		<key><![CDATA[`key_field1`]]></key>
	//	</keys>
	//	<values>
	//		<value datatype="str" max_len="18" type="field"><![CDATA[`zky_docid`]]></value>
	//	</values>
	//</map>

	// For aggregation functions: "sum", "count", "distinct count", "max", "min"
	// 	the data field type must be numerical. Use the following 
	// 	format for IILs:
	// 	[key0x01key01..., value]	
	//
	// If there is no aggregation function, use the following format:
	// 	[key0x01key01...0x02value, docid]
	// "value" is converted to string.
	bool rslt = false;
	OmnString iil_name;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	OmnString value_sep = "0x02";
	AosConvertAsciiBinary(value_sep);
	AosXmlTagPtr map_tag = maps_tag->getFirstChild(true);
	while(map_tag)
	{
		OmnString key_name, value_name;
		iil_name = map_tag->getAttrStr("zky_iilname", "");
		aos_assert_r(iil_name != "", false);

		//keys
		OmnString key;
		AosXmlTagPtr keys_tag = map_tag->getFirstChild("keys");
		aos_assert_r(keys_tag, false);
		int num = keys_tag->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr key_tag = keys_tag->getChild(i);
			aos_assert_r(key_tag, false);

			key_name = key_tag->getNodeText();
			key_name << ";";
			OmnString err;
			AosExprObjPtr expr = AosParseExpr(key_name, err, rdata_raw);
			AosValueRslt key_rslt;
			rslt = expr->getValue(rdata_raw, record.getPtr(), key_rslt);
			aos_assert_r(rslt, false);

			key << key_rslt.getValueStr1();
			if (i != num -1)
			{
				key << key_sep;
			}
		}

		//values
		AosXmlTagPtr values_tag = map_tag->getFirstChild("values");
		aos_assert_r(values_tag, false);
		AosXmlTagPtr value_tag = values_tag->getFirstChild(true);
		aos_assert_r(value_tag, false);
		value_name = value_tag->getNodeText();
		value_name << ";";

		OmnString argtype = value_tag->getAttrStr("agrtype", "");
		if (argtype != "")
		{
			AosValueRslt value_rslt;
			OmnNotImplementedYet;
			rslt = iil_client->addStrValueDoc(iil_name, key, value_rslt.getU64Value(0), true, true, rdata_raw);
		}
		else
		{
			AosValueRslt value_rslt;
			OmnString err;
			AosExprObjPtr expr = AosParseExpr(value_name, err, rdata_raw);
			aos_assert_r(expr, false);
			rslt = expr->getValue(rdata_raw, record.getPtr(), value_rslt);
			aos_assert_r(rslt, false);
			if (value_name == "`zky_docid`;")
			{
				rslt = iil_client->addStrValueDoc(iil_name, key, value_rslt.getU64Value(0), true, true, rdata_raw);
			}
			else
			{
				key << value_sep << value_rslt.getValueStr1();
				rslt = iil_client->addStrValueDoc(iil_name, key, 1, true, true, rdata_raw);
			}
		}
		aos_assert_r(rslt, false);
		map_tag = maps_tag->getNextChild();
	}
	return true;
	*/
}

bool
AosDocClientNew::insertInternalStat(
		const AosXmlTagPtr &stat_tags,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;
	return false;        
	/*
	//<statistic zky_stat_identify_key="key_field3" zky_stat_name="db_t1_stat_uw_st5">
	//	<key_fields>
	//		<field field_name="key_field3"></field>
	//	</key_fields>
	//	<stat_key_conn/>
	//	<vector2ds>
	//		<vt2d>
	//			<vt2d_conn time_bucket_weight="1" start_time_slot="14610"/>
	//			<vt2d_info vt2d_name="vt2d_0" has_valid_flag="true">
	//				<measures>
	//					<measure zky_name="sum0x28key_field10x29" field_name="key_field1" agr_type="sum" zky_data_type="double"></measure>
	//				</measures>
	//			</vt2d_info>
	//		</vt2d>
	//	</vector2ds>
	//	<stat_cubes>
	//		<cube cube_id="0">
	//			<stat_key_conn meta_fileid="107"/>
	//			<vector2ds>
	//				<vt2d>
	//					<vt2d_conn time_bucket_weight="1" start_time_slot="14610" meta_fileid="108"/>
	//					<vt2d_info vt2d_name="vt2d_0" has_valid_flag="true">
	//						<measures>
	//							<measure zky_name="sum0x28key_field10x29" field_name="key_field1" agr_type="sum" zky_data_type="double"/>
	//						</measures>
	//					</vt2d_info>
	//				</vt2d>
	//			</vector2ds>
	//		</cube>
	//	</stat_cubes>
	//</statistic>

	AosXmlTagPtr stat_tag = stat_tags->getFirstChild(true);	
	OmnString conf_objid;
	while(stat_tag)
	{
		conf_objid = stat_tag->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(conf_objid != "", false);
		OmnString idfy_key = stat_tag->getAttrStr("zky_stat_identify_key", "");
		aos_assert_r(idfy_key != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(conf_objid, rdata_raw);
		aos_assert_r(stat_doc, false);
		//proc stat
		stat_tag = stat_tags->getNextChild();
	}
	return true;
	*/
}

bool
AosDocClientNew::insertNormalStat(
		const OmnString &db_name,
		const AosXmlTagPtr &mstat_tag,
		const AosDataRecordObjPtr &record,
		AosRundata* rdata_raw)
{
	OmnNotImplementedYet;
	return false;        
	/*
	//<statistic 
	//	zky_stat_identify_key="ZGFYHZ_FFFS_ZGFYHZ_QYJHBH_ZGFYHZ_ZT" 
	//	zky_stat_name="huabao_hb_table_ZGFYHZ_stat_hb_stat_ZGFYHZ"
	//	zky_table_name="hb_table_ZGFYHZ">
	//	<key_fields>
	//		<field field_name="ZGFYHZ_FFFS"></field>
	//		<field field_name="ZGFYHZ_QYJHBH"></field>
	//		<field field_name="ZGFYHZ_ZT"></field>
	//	</key_fields>
	//	<measures>
	//		<measure field_name="ZGFYHZ_SFJE" agr_type="sum" zky_name="sum0x28ZGFYHZ_SFJE0x29" zky_data_type="int64"/>
	//	</measures>
	//	<time_field time_format ="yyyymmdd" grpby_time_unit="_day" time_field_name="ZGFYHZ_JZRQ"/>
	//	<stat_key_conn/>
	//	<vt2d_conn time_bucket_weight="1000" start_time_slot="14610"/>
	//</statistic>
	int idx = -1;
	OmnString key_sep = "0x01";
	AosConvertAsciiBinary(key_sep);
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);

	AosXmlTagPtr stat_tag_itl = mstat_tag->getFirstChild("internal_statistics");
	AosXmlTagPtr stat_tags = mstat_tag->getFirstChild("statistic_defs");
	
	int num = stat_tags->getNumSubtags();
	OmnString conf_objid;
	for (int n=0; n<num; n++)
	{
		AosXmlTagPtr stat_tag = stat_tags->getChild(n);
		conf_objid = stat_tag->getAttrStr("zky_stat_conf_objid", "");
		aos_assert_r(conf_objid != "", false);
		AosXmlTagPtr stat_doc = AosGetDocByObjid(conf_objid, rdata_raw);
		aos_assert_r(stat_doc, false);
		OmnString table_name = stat_doc->getAttrStr("zky_table_name", "");
		aos_assert_r(table_name != "", false);
		OmnString stat_name = stat_doc->getAttrStr("zky_stat_name", "");
		aos_assert_r(stat_name != "", false);

		OmnString stat_key = stat_doc->getAttrStr("zky_stat_identify_key", "");
		aos_assert_r(stat_key != "", false);
		u32 hash_key = AosGetHashKey(stat_key);
		int cube_num = AosGetNumCubes();
		aos_assert_r(cube_num > 0, -1);
		u32 cube_id =  hash_key % cube_num;

		//proc stat
		OmnString field_name;
		vector<OmnString> key_fields;
		AosXmlTagPtr key_tags = stat_doc->getFirstChild("key_fields");
		OmnString key_str;
		int num = key_tags->getNumSubtags();
		for (int i=0; i<num; i++)
		{
			AosXmlTagPtr key_tag = key_tags->getChild(i);
			aos_assert_r(key_tag, false);
			field_name = key_tag->getAttrStr("field_name", "");
			key_fields.push_back(field_name);
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			key_str << key_rslt.getValueStr1();
			if (i != num -1)
			{
				key_str << key_sep;
			}
		}

		//<time_field time_format ="yyyymmdd" grpby_time_unit="_day" time_field_name="ZGFYHZ_JZRQ"/>
		AosValueRslt time_value;
		OmnString  time_unit_str;
		AosXmlTagPtr time_field_tag = stat_doc->getFirstChild("time_field");
		AosStatTimeUnit::E grpby_time_unit;
		if (time_field_tag)
		{
			time_unit_str = time_field_tag->getAttrStr("grpby_time_unit", "");
			grpby_time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str );
			aos_assert_r(grpby_time_unit != AosStatTimeUnit::eInvalid, false);

			OmnString time_format = time_field_tag->getAttrStr("time_format", "");
			OmnString time_field = time_field_tag->getAttrStr("time_field_name", "");
			if (time_field != "") 
			{
				idx = record->getFieldIdx(time_field, rdata_raw);
				aos_assert_r(idx != -1, false);
				record->getFieldValue(idx, time_value, false, rdata_raw);
			}
		}

		//iil_name ...?
		//_zt4k
		//db name [db]
		//table name [t1]
		//stat name [xxx]
		//_internal_0__$group
		OmnString iil_name = "_zt4k_";
		iil_name << stat_name << "_internal_" << n << "__" << cube_id ;
		u64 sdocid = 0;
		iil_client->getDocid(iil_name, key_str, sdocid, rdata_raw);
		if (sdocid == 0)
		{
			OmnString stat_name = stat_doc->getAttrStr("zky_stat_name", "");
			aos_assert_r(stat_name != "", false);
			AosStatIdIDGen* idGen = OmnNew AosStatIdIDGen(stat_name, 1);
			sdocid = idGen->nextDocid(rdata_raw, 0);
			aos_assert_r(sdocid != 0, false);
			iil_client->addStrValueDoc(iil_name, key_str, sdocid, true, true, rdata_raw);
		}

		// Create indexes for key fields
		//iilname ...?
		//_zt44
		//db name [db]
		//table name [t1]
		//stat name [xxxx]
		//_internal_0_key_
		//field_name [key_field1]
		for (u32 i=0; i<key_fields.size(); i++)
		{
			OmnString field_name = key_fields[i];
			iil_name = "_zt44_";
			iil_name << stat_name << "_internal_" << n << "_key_" << field_name;
			idx = record->getFieldIdx(field_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			AosValueRslt key_rslt;
			record->getFieldValue(idx, key_rslt, false, rdata_raw);
			iil_client->addStrValueDoc(iil_name, key_rslt.getValueStr1(), sdocid, true, true, rdata_raw);
		}

		//<measure field_name="ZGFYHZ_SFJE" agr_type="sum" zky_name="sum0x28ZGFYHZ_SFJE0x29" zky_data_type="int64"/>
		OmnString measure_name;
		
	//	OmnString time_unit_str;
		int vt2d_idx;
		AosVt2dInfo vt2d_info;


		vector<AosValueRslt> measures;
		AosXmlTagPtr measure_tags = stat_doc->getFirstChild("measures");
		AosXmlTagPtr measure_tag = measure_tags->getFirstChild(true);
		while (measure_tag)
		{
			measure_name = measure_tag->getAttrStr("field_name", "");
			AosValueRslt measure_rslt;
			idx = record->getFieldIdx(measure_name, rdata_raw);
			aos_assert_r(idx != -1, false);
			record->getFieldValue(idx, measure_rslt, false, rdata_raw);
			measures.push_back(measure_rslt);
			measure_tag = measure_tags->getNextChild();
		}
		//
		//craete input_data
		//
		AosBuffPtr input_data = OmnNew AosBuff(10 AosMemoryCheckerFileLine);
		input_data->appendU64(sdocid);
		int time_value_pos = input_data->getCrtIdx();
		input_data->appendU64(time_value.getU64Value(0));
		int measures_value_pos = input_data->getCrtIdx();
		for(size_t i = 0; i < measures.size();i++)
		{
			input_data->appendInt64(measures[i].getInt64Value(rdata_raw));
		}
		u32 keyPos = input_data->getCrtIdx();
		input_data->addBuff(key_str.data(),key_str.length());
		u32 newFlagPos = input_data->getCrtIdx();
		OmnString isnew("0");
		input_data->addBuff(isnew.data(),isnew.length());
		u64 record_len = input_data->dataLen();
//
//		a new record string value is in thr following format
//		|sdocid|time_value|value1..Valuen|key_str|isnew
//
		AosStatModifyInfo mStatMdfInfo;
		mStatMdfInfo.mSdocidPos = 0;
		mStatMdfInfo.mKeyPos = keyPos;
		mStatMdfInfo.mIsNewFlagPos = newFlagPos;
		mStatMdfInfo.mRecordLen = record_len;

		AosXmlTagPtr stat_doc_itl = stat_tag_itl->getFirstChild("statistic");
		aos_assert_r(stat_doc_itl,false);
		OmnString itl_conf_objid = stat_doc_itl->getAttrStr("zky_stat_conf_objid", "");
		AosXmlTagPtr itl_stat_doc = AosGetDocByObjid(itl_conf_objid, rdata_raw);
		aos_assert_r(itl_stat_doc,false);
		measure_tags = stat_doc->getFirstChild("measures");
		measure_tag = measure_tags->getFirstChild(true);
		while(measure_tag)
		{
			measure_name = measure_tag->getAttrStr("zky_name", "");
			aos_assert_r(measure_name!="",0);
			AosXmlTagPtr time_field_tag = stat_doc->getFirstChild("time_field","");
			
			if(time_field_tag)
			{
				time_unit_str = time_field_tag->getAttrStr("grpby_time_unit","");
				grpby_time_unit = AosStatTimeUnit::getTimeUnit(time_unit_str);
			}
			else
			{
				time_value_pos = -1;
				grpby_time_unit = AosStatTimeUnit::eInvalid;
			}
			AosStatisticPtr mStat = new AosStatistic();
			mStat->config(rdata_raw,itl_stat_doc);
			vt2d_idx = mStat->getVt2dIdxByMeasureName(measure_name,grpby_time_unit);
			aos_assert_r(vt2d_idx != 1,false);
			bool rslt = mStat->getVt2dInfo(vt2d_idx,vt2d_info);
			aos_assert_r(rslt,false);
			mStatMdfInfo.addMeasure(rdata_raw,measure_name,measures_value_pos,8*measures.size(),time_value_pos,vt2d_idx,vt2d_info);
			measure_tag = measure_tags->getNextChild();
		}
		AosXmlTagPtr stat_cubes = itl_stat_doc->getFirstChild("stat_cubes", "");
		AosXmlTagPtr cube_conf = stat_cubes->getFirstChild("cube");

		bool svr_death;
		AosBuffPtr resp;
		AosTransPtr trans = OmnNew AosBatchSaveStatDocsTrans(cube_id,cube_conf,input_data,mStatMdfInfo);
		AosSendTrans(rdata_raw,trans,resp,svr_death);
		if(svr_death)
		{
			OmnScreen << "batch save stat docs error! svr_death!" << svr_death;
			return false;
		}
		aos_assert_r(resp,0);
		bool rslt = resp->getU8(0);
		aos_assert_r(rslt,false);
	//	rslt = AosAddVector2D(cube_id,rdata_raw,cube_conf,input_data,mStatMdfInfo);
	//	aos_assert_r(rslt, false);
		stat_tag = stat_tags->getNextChild();
	}
	return true;
	*/
}
