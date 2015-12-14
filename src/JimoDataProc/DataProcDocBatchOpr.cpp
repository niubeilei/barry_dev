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
// 2014/12/24 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcDocBatchOpr.h"

#include "API/AosApi.h"
#include "DataProc/DataProc.h"
#include "DocTrans/BatchInsertDocTrans.h"
#include "DocTrans/BatchUpdateDocTrans.h"
#include "JQLStatement/JqlStmtSchema.h"
#include "JSON/JSON.h"
#include "Schema/Schema.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "StorageEngine/SengineGroupedDoc.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEModules/SnapshotIdMgr.h"
#include "TaskMgr/Task.h"
#include "Porting/Sleep.h"

#include <boost/make_shared.hpp>
#include "JSON/JSON.h"

#include "JQLStatement/JqlStmtSchema.h"
#include "Schema/Schema.h"
#include "StreamEngine/StreamSnapshot.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcDocBatchOpr_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcDocBatchOpr(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosDataProcDocBatchOpr::AosDataProcDocBatchOpr(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcDocBatchOpr)
{
	mRawInputDocid = NULL;
	mOutputRecord = NULL;
	mRawOutputRecord = NULL;
	mGroupDocMaxSize = AosStorageEngineObj::eMaxDocPerGroup;
	mNumDocidsUsed = 0;
	mBuff = OmnNew AosBuff(sizeof(u64)*mGroupDocMaxSize+100  AosMemoryCheckerArgs);
	memset(mBuff->data(), 0, mBuff->buffLen());
	mBuff->setCrtIdx(sizeof(u64)*mGroupDocMaxSize);
	mGroupId = 0;
	mDocid = 0;
}

AosDataProcDocBatchOpr::AosDataProcDocBatchOpr(const AosDataProcDocBatchOpr &proc)
:
AosStreamDataProc(proc),
mRawInputDocid(0),
mRawOutputRecord(0)
{
	if (proc.mInputDocid)
	{
		mInputDocid = proc.mInputDocid->cloneExpr();
		mRawInputDocid = mInputDocid.getPtr();
	}
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
		mRawOutputRecord = mOutputRecord.getPtr();
	}

	for (size_t i = 0; i < proc.mFields.size(); ++i)
	{
		mFields.push_back((proc.mFields)[i]->cloneExpr());
		mRawFields.push_back(mFields[i].getPtr());
	}
	mFieldSize = mFields.size();

	mSnapMaps = proc.mSnapMaps;
	mTaskDocid = proc.mTaskDocid;
	mName = proc.mName;
	mOpr = proc.mOpr;
	mSchemaName = proc.mSchemaName;
	mSchemaDocid = proc.mSchemaDocid;
	mSem = OmnNew OmnSem(0);
	mTotalReqs = proc.mTotalReqs;

	mGroupDocMaxSize = AosStorageEngineObj::eMaxDocPerGroup;
	mNumDocidsUsed = 0;
	mBuff = OmnNew AosBuff(sizeof(u64)*mGroupDocMaxSize+100  AosMemoryCheckerArgs);
	memset(mBuff->data(), 0, mBuff->buffLen());
	mBuff->setCrtIdx(sizeof(u64)*mGroupDocMaxSize);
	mGroupId = 0;
	mName = proc.mName;
}


AosDataProcDocBatchOpr::~AosDataProcDocBatchOpr()
{
}


bool
AosDataProcDocBatchOpr::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp2"><![CDATA[
	//{
	//  "type": "docbatchopr",
	//  "fields": ["key_field2", "key_field3"],
	//  "format": "schemaName",
	//  "opr": "batch_insert",
	//  "docid": "docid"
	//};
	//
	//for stream config
	////<dataproc zky_name = "dp2"><![CDATA[
	//{
	//  "type": "docbatchopr",
	//  "opr": "batch_insert",
	//  "docid": "docid"
	//};
	//mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	OmnString dp_json = def->getNodeText();
	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);

	mTotalReqs = 0;
	mSchemaDocid = 0;
	mSem = OmnNew OmnSem(0);

	JSONValue json;
	JSONReader reader;
	reader.parse(dp_json, json);
	mJson = json;

	OmnString opr_str = json[AOSTAG_OPERATOR].asString("");
	OmnScreen << "dataproc config json is: " << dp_json << endl;
	mOpr = AosGroupDocOpr::toEnum(opr_str);

	//input docid
	OmnString docid = json["docid"].asString("");
	if (docid == "") return false;
	docid << ";";
	OmnString err;
	mInputDocid = AosParseExpr(docid, err, rdata.getPtr());
	if (!mInputDocid)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		return false;
	}
	mRawInputDocid = mInputDocid.getPtr();

	//input fields
	JSONValue fields = json["fields"];
	AosExprObjPtr expr;
	for(size_t i = 0; i < fields.size(); i++)
	{
		OmnString name = fields[i].asString("");
		if (name != "")
		{
			OmnString expr_str, msg;
			expr_str << name << " ;";
			expr = AosParseExpr(expr_str, msg, rdata.getPtr());
			if (!expr)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< "" << enderr;
				OmnAlarm << expr_str << enderr;
				return false;
			}
			mFields.push_back(expr);
			mRawFields.push_back(expr.getPtr());
		}
	}
	mFieldSize = mFields.size();
	//format
	mSchemaName = json["format"].asString("");
	if (mSchemaName != "")
	{
		AosXmlTagPtr doc;
		if(mIsStreaming)
		{
			doc = AosJqlStatement::getDoc(
				rdata, JQLTypes::eDataRecordDoc, mSchemaName);
		}
		else
		{
			doc = AosJqlStatement::getDoc(
				rdata, JQLTypes::eSchemaDoc, mSchemaName);
		}
		aos_assert_r(doc, false);
		mSchemaDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(mSchemaDocid != 0, false);
		mOutputRecord = createOutputRecord(rdata.getPtr(), NULL);
		mRawOutputRecord = mOutputRecord.getPtr(); 
	}

	return true;
}


bool
AosDataProcDocBatchOpr::flush(
		const int vir_id,
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	if (buff->dataLen() == 0) return true;
	u64 docid;
	docid = mTaskDocid;
	if(mIsStreaming)
	{
		docid = getServiceDocid(); 
	}
	aos_assert_r(docid > 0, false);
	bool rslt = false; 
	switch(mOpr)
	{
		case AosGroupDocOpr::eBatchInsert:
			{
				//print by levi
				//u64 tStart = OmnGetTimestamp();

				AosDataProcObjPtr thisptr(this, false);
				aos_assert_r(mGroupId != 0, false);
				AosTransPtr trans = OmnNew AosBatchInsertDocTrans(thisptr, vir_id, mGroupId, mSnapMaps[vir_id], docid, buff);
				aos_assert_r(trans, false);
				rslt = AosSendTransAsyncResp(rdata, trans);
				mTotalReqs++;

				//u64 tEnd = OmnGetTimestamp();
				//OmnScreen << "DocBatchOpr batchinsert take : " << tEnd - tStart << endl;
				break;
			}
		case AosGroupDocOpr::eBatchDelete:
			{
				OmnNotImplementedYet;
				//rslt = AosStorageEngineMgr::getSelf()->batchDeleteDoc(buff, mTaskDocid, rdata);
				break;
			}
		case AosGroupDocOpr::eUpdate:
			{
				OmnNotImplementedYet;
				AosDataProcObjPtr thisptr(this, false);
				aos_assert_r(mGroupId != 0, false);
				AosTransPtr trans = OmnNew AosBatchUpdateDocTrans(thisptr, vir_id, mGroupId, mSnapMaps[vir_id], docid, buff);
				aos_assert_r(trans, false);
				rslt = AosSendTransAsyncResp(rdata, trans);
				mTotalReqs++;

				//OmnNotImplementedYet;
				//rslt = AosStorageEngineMgr::getSelf()->updateDoc(buff, mSnapMaps, mTaskDocid, rdata);
				break;
			}
		default:
			OmnAlarm << "missing operator" << enderr;
			break;
	}
	aos_assert_r(rslt, false);
	return true;
}


AosDataProcStatus::E
AosDataProcDocBatchOpr::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	//return AosDataProcStatus::eContinue;
	mProcDataCount++;
	bool rslt = false;
	AosDataRecordObj* input_record = input_records[0];

	//set docid
	rslt = mRawInputDocid->getValue(rdata_raw, input_record, mValue);
	if (!rslt) return AosDataProcStatus::eError;

	if (mSchemaDocid == 0)
	{
		mSchemaDocid = input_record->getSchemaDocid();
		aos_assert_r(mSchemaDocid > 0, AosDataProcStatus::eContinue);
	}

	//for debugging
	//input_record->dumpData(mDebug,"DocBatchOpr input is: ");
	DumpRecord(input_record, "DocBatchOpr input is: ");

	mDocid = mValue.getU64();
	aos_assert_r(mDocid != 0, AosDataProcStatus::eError);
	input_record->setDocid(mDocid);
	switch(mOpr)
	{
		case AosGroupDocOpr::eBatchInsert:
			rslt = batchInsertDoc(rdata_raw, input_record);
			break;
		case AosGroupDocOpr::eBatchDelete:	
			OmnNotImplementedYet;
			rslt = batchDeleteDoc(rdata_raw, input_record);
			break;
		case AosGroupDocOpr::eUpdate:
			OmnNotImplementedYet;
			rslt = batchUpdateDoc(rdata_raw, input_record);
			break;
		default:
			OmnAlarm << "missing operator" << enderr;
			break;
	}
	aos_assert_r(rslt, AosDataProcStatus::eError);

	mOutputCount++;
	return AosDataProcStatus::eContinue;
}


bool
AosDataProcDocBatchOpr::batchInsertDoc(
		AosRundata* rdata_raw,
		AosDataRecordObj* input_record)
{
	bool rslt;
	char *doc = NULL;
	int rcd_len = 0;

	if (mRawFields.empty())
	{
		rcd_len = input_record->getRecordLen();
		doc = input_record->getData(rdata_raw);
	}
	else 
	{
		bool outofmem = false;
		mRawOutputRecord->clear();
		aos_assert_r(mFieldSize > 0, false);
		for (u32 i = 0; i < mFieldSize; i++)
		{
			rslt = mRawFields[i]->getValue(rdata_raw, input_record, mValue);
			aos_assert_r(rslt, false);

			rslt = mRawOutputRecord->setFieldValue(i, mValue, outofmem, rdata_raw);
			aos_assert_r(rslt, false);

		}
		mRawOutputRecord->flush();
		doc = mRawOutputRecord->getData(rdata_raw);
		rcd_len = mRawOutputRecord->getRecordLen(); 
	}

//OmnScreen << "This : " << (void *)this << ", Docid is " << mDocid << ", Count is " << mNumDocidsUsed << endl;
	mNumDocidsUsed++;
	rslt = AosSengineGroupedDoc::AosAssembleGroupDoc(rdata_raw, mDocid, mSchemaDocid, rcd_len, doc, mBuff);
	aos_assert_r(rslt, false);
	
	if (mNumDocidsUsed == mGroupDocMaxSize)
	{
		int vir_id = AosGetCubeId(mDocid);	
		u64 local_id = AosSengineGroupedDoc::getLocalId(mDocid);
		rslt = AosSengineGroupedDoc::getGroupid(0, local_id, mGroupDocMaxSize, mGroupId); 
		aos_assert_r(rslt, false); 
		bool rslt = flush(vir_id, mBuff, rdata_raw);
		aos_assert_r(rslt, false);
		memset(mBuff->data(), 0, mBuff->buffLen());
		mNumDocidsUsed = 0;
		mBuff->clear();
		mBuff->setCrtIdx(sizeof(u64)*mGroupDocMaxSize);
	}
	return true;
}


AosDataRecordObjPtr
AosDataProcDocBatchOpr::createOutputRecord(
		AosRundata* rdata_raw,
		AosDataRecordObj* input_record)
{
	aos_assert_r(mSchemaName != "", NULL);

	//get schema doc
	AosXmlTagPtr record, doc;
	if(!mIsStreaming)
		doc = AosJqlStatement::getDoc(rdata_raw, JQLTypes::eSchemaDoc, mSchemaName); 
	else
		doc = AosJqlStatement::getDoc(rdata_raw, JQLTypes::eDataRecordDoc, mSchemaName); 
	if (!doc)
	{
		OmnString error;
		error << "Failed to display schema " << mSchemaName << " : not found.";
		rdata_raw->setJqlMsg(error);

		OmnCout << "Failed to display schema " << mSchemaName<< " : not found." << endl;
		return NULL;
	}

	AosXmlTagPtr rcd_conf;
	OmnString name;
	if(!mIsStreaming)
	{
		rcd_conf = doc->getFirstChild("datarecord");
		if (!rcd_conf)
		{
			AosSetEntityError(rdata_raw, "schemaunilen_missing_record", "Schema", mSchemaName) 
				<< doc->toString();
			return NULL;
		}
		name = rcd_conf->getAttrStr("zky_datarecord_name");
	}
	else
	{
		rcd_conf = doc->getFirstChild();
		if (!rcd_conf)
		{
			AosSetEntityError(rdata_raw, "schemaunilen_missing_record", "Schema", mSchemaName) 
				<< doc->toString();
			return NULL;
		}
		name = rcd_conf->getAttrStr("zky_datarecord_name");
	}
	
	if (!rcd_conf)
	{
		AosSetEntityError(rdata_raw, "schemaunilen_missing_record", "Schema", mSchemaName) 
			<< doc->toString();
		return NULL;
	}

	if (name != "")
	{
		record = AosGetDocByKey(rdata_raw, "", "zky_datarecord_name", name);

	}
	else
	{
		record = rcd_conf;      
		
		//if(isStreaming(mJson))
		if(mIsStreaming)
		{
			AosXmlTagPtr recordXml = record;
			AosXmlTagPtr dataFieldsXml = recordXml->getFirstChild("datafields");
			AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();

			OmnString str_type, type;
			while (dataFieldXml)
			{
				str_type = dataFieldXml->getAttrStr("data_type");
				type = dataFieldXml->getAttrStr("type");
				/*
				if(type == "expr")
				{
					dataFieldXml->setAttr("type", str_type);
				}
				*/
				
				dataFieldXml = dataFieldsXml->getNextChild();
			}
		}
	}
	aos_assert_r(record, NULL);

	OmnString record_str = record->toString();

	OmnString recordCtnr = "<datarecord zky_name=\"";
	recordCtnr << mName << "_output\" type=\"ctnr\">" 
				<< record_str 
				<< "</datarecord>";
	AosXmlTagPtr recordCtnr_xml = AosStr2Xml(rdata_raw, recordCtnr AosMemoryCheckerArgs);

	//create datarecord
	AosDataRecordObjPtr output_record = AosDataRecordObj::createDataRecordStatic(recordCtnr_xml, 
			0, rdata_raw AosMemoryCheckerArgs);
	aos_assert_r(output_record, NULL);
	return output_record;
}


bool
AosDataProcDocBatchOpr::batchDeleteDoc(
		AosRundata* rdata_raw,
		AosDataRecordObj* input_record)

{
	OmnNotImplementedYet;
	/*
	u64 docid = input_record->getDocid();
	aos_assert_r(docid != 0, false);
	mBuff->setU64(docid);
	mDataLen += sizeof(u64);
	aos_assert_r(mDataLen == mBuff->dataLen(), false);
	*/
	return true;
}


AosJimoPtr 
AosDataProcDocBatchOpr::cloneJimo() const
{
	return  OmnNew AosDataProcDocBatchOpr(*this);
}


AosDataProcObjPtr
AosDataProcDocBatchOpr::cloneProc() 
{
	//return this;
	return OmnNew AosDataProcDocBatchOpr(*this);
}


bool
AosDataProcDocBatchOpr::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	return true;
}


bool
AosDataProcDocBatchOpr::createSnapshots(const AosRundataPtr &rdata)
{
	vector<AosTaskDataObjPtr> snapshots;
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	aos_assert_r(task, false);
	AosTaskDataObjPtr snapshot;
	vector<u32> virtualids = AosGetTotalCubeIds();
	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		snapshot_id = doc_client->createSnapshot(rdata, virtualids[i], snapshot_id,  AosDocType::eGroupedDoc, mTaskDocid);
		snapshot = AosTaskDataObj::createTaskDataDocSnapShot(virtualids[i], snapshot_id, AosDocType::eGroupedDoc, mTaskDocid, false);
		aos_assert_r(snapshot, false);
		mSnapMaps[virtualids[i]] = snapshot_id;
		snapshots.push_back(snapshot);
	}
	return task->updateTaskSnapShots(snapshots, rdata);
}

bool
AosDataProcDocBatchOpr::createSnapshotsStreaming(const AosRundataPtr &rdata)
{
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);

	vector<u32> virtualids = AosGetTotalCubeIds();
	u64 snapshot_id = AosSnapshotIdMgr::getSelf()->createSnapshotId(rdata);
	u64 snap_id;
	for (u32 i = 0; i < virtualids.size(); i++)
	{
		snap_id = doc_client->createSnapshot(rdata, virtualids[i], snapshot_id,  AosDocType::eGroupedDoc, getServiceDocid());
		mSnapMaps[virtualids[i]] = snapshot_id;
	}
	return true;
}


bool
AosDataProcDocBatchOpr::start(const AosRundataPtr &rdata)
{
	showDataProcInfo(__FILE__, __LINE__, "start", "DataProcDocBatchOpr");
	if (mIsStreaming)
	{
		return createSnapshotsStreaming(rdata);
	}
	else
	{
		return createSnapshots(rdata);
	}
	return true;
}


void
AosDataProcDocBatchOpr::callback(const bool svr_death)
{
	mSem->post();
}


bool
AosDataProcDocBatchOpr::finish(const AosRundataPtr &rdata)
{
	switch(mOpr)
	{
	case AosGroupDocOpr::eBatchInsert:
		if (mBuff)
		{
			int vir_id = AosGetCubeId(mDocid);	
			u64 local_id = AosSengineGroupedDoc::getLocalId(mDocid);
			bool rslt = AosSengineGroupedDoc::getGroupid(0, local_id, mGroupDocMaxSize, mGroupId); 
			aos_assert_r(rslt, false); 
			rslt = flush(vir_id, mBuff, rdata);
			aos_assert_r(rslt, false);
		}
		mBuff = OmnNew AosBuff(sizeof(u64)*mGroupDocMaxSize+100  AosMemoryCheckerArgs);
		memset(mBuff->data(), 0, mBuff->buffLen());
		mBuff->clear();
		mBuff->setCrtIdx(sizeof(u64)*mGroupDocMaxSize);
		break;


	//barry 2015/11/24
	case AosGroupDocOpr::eUpdate:
		{
			map<int, AosBuffPtr>::iterator itr;
			int vir_id;
			u64 local_id;
			bool rslt;
			for (itr = mBuffMap.begin(); itr != mBuffMap.end(); ++itr)
			{
				if (itr->second)
				{
					vir_id = itr->first;
					local_id = AosSengineGroupedDoc::getLocalId(mDocid);
					rslt = AosSengineGroupedDoc::getGroupid(0, local_id, mGroupDocMaxSize, mGroupId);
					aos_assert_r(rslt, false);
					rslt = flush(vir_id, itr->second, rdata);
					aos_assert_r(rslt, false);
					itr->second->clear();
				}
			}
			break;
		}

	default:
		OmnAlarm << "missing operator" << enderr;
		break;
	}

	for (int i = 0; i < mTotalReqs; i++)
	{
		mSem->wait();
	}
	mTotalReqs = 0;
	mNumDocidsUsed = 0;
	mGroupId = 0;
	return true;
}


bool
AosDataProcDocBatchOpr::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcDocBatchOpr " << "(" << mName << ")" << "finished:" 
		<< ", procDataCount:" << procDataCount
		<< ", OutputCount:" << procOutputCount << endl;
	for (u32 i=0; i<procs.size(); i++)
	{
		bool rslt = procs[i]->finish(rdata);
		aos_assert_r(rslt, false);
	}

	showDataProcInfo(__FILE__, __LINE__, procDataCount, procOutputCount);
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcDocBatchOpr");
	return true;
}


bool 
AosDataProcDocBatchOpr::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	JSONReader reader;
	JSONValue jsonObj;
	try
	{
		reader.parse(jsonstr, jsonObj);
	}
	catch (std::exception &e)
	{
		OmnScreen << "JSONException: " << e.what() << "\n" << jsonstr << endl;
		return false;
	}

	mJson = jsonObj;
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, obj_name);
	string cdata = jsonstr;
	OmnString dpconf = "";	
	dpconf 
		<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
		<< 	AOSTAG_OBJID << "=\"" << objid << "\">"
		<< 		"<dataproc zky_name=\"" << obj_name << "\" jimo_objid=\"dataprocdocbatchopr_jimodoc_v0\">"
		<< 			"<![CDATA[" << cdata << "]]>"
		<< 		"</dataproc>"
		<< "</jimodataproc>";
	//return AosCreateDoc(dpconf, true, rdata);
	prog->saveLogicDoc(rdata, objid, dpconf);
	return true;
}


//barry 2015/11/24
bool
AosDataProcDocBatchOpr::batchUpdateDoc(
		AosRundata *rdata_raw,
		AosDataRecordObj* input_record)
{
	bool rslt;
	char *doc = NULL;
	int rcd_len = 0;

	if (mRawFields.empty())
	{
		rcd_len = input_record->getRecordLen();
		doc = input_record->getData(rdata_raw);
	}
	else 
	{
		bool outofmem = false;
		mRawOutputRecord->clear();
		aos_assert_r(mFieldSize > 0, false);
		for (u32 i = 0; i < mFieldSize; i++)
		{
			rslt = mRawFields[i]->getValue(rdata_raw, input_record, mValue);
			aos_assert_r(rslt, false);

			rslt = mRawOutputRecord->setFieldValue(i, mValue, outofmem, rdata_raw);
			aos_assert_r(rslt, false);
		}
		mRawOutputRecord->flush();
		doc = mRawOutputRecord->getData(rdata_raw);
		rcd_len = mRawOutputRecord->getRecordLen(); 
	}
	
	AosBuffPtr buff = NULL;
	int vir_id = AosGetCubeId(mDocid);
	map<int, AosBuffPtr>::iterator itr;
	itr = mBuffMap.find(vir_id);
	if (itr != mBuffMap.end())
	{
		buff = itr->second;
	}
	else
	{
		buff = OmnNew AosBuff(100  AosMemoryCheckerArgs);
		mBuffMap[vir_id] = buff;
	}
	buff->setU64(mDocid);
	buff->setU64(mSchemaDocid);
	buff->setU32(rcd_len);
	buff->setBuff(doc, rcd_len);
	if (buff->dataLen() >= eMaxBuffSize)
	{
		u64 local_id = AosSengineGroupedDoc::getLocalId(mDocid);
		rslt = AosSengineGroupedDoc::getGroupid(0, local_id, mGroupDocMaxSize, mGroupId); 
		aos_assert_r(rslt, false); 
		bool rslt = flush(vir_id, buff, rdata_raw);
		aos_assert_r(rslt, false);
		buff->clear();
	}
	return true;
}
