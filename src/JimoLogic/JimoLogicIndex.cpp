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
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicIndex.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JQLStatement/JqlStatement.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcIf.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataProcIndex.h"
#include "AosConf/CompFun.h"

#include <boost/make_shared.hpp>
using namespace AosConf;
using boost::make_shared;
using boost::shared_ptr;


AosJLIndex::AosJLIndex(
		const AosRundataPtr &rdata,
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &def,
		const string &tablename,
		map<OmnString, AosXmlTagPtr> &table_fileds,
		JQLTypes::OpType op)
:AosJimoLogic(1)
{
	aos_assert(def);
	boost::shared_ptr<DataRecord> recordObj = inputds->getSchema()->getRecord();
	OmnString recordName = recordObj->getAttribute("zky_name");
	aos_assert(recordName != "");
	vector<boost::shared_ptr<DataField> > &v = recordObj->getFields();

	// 1. output dataset
	OmnString indexObjid = def->getAttrStr("idx_name");
	aos_assert(indexObjid != "");
	boost::shared_ptr<DataSet> datasetObj = boost::make_shared<DataSet>();
	datasetObj->setAttribute("zky_name", indexObjid);
	mOutputds.push_back(datasetObj);

	// 2. maptask dataproc (a pair)
	AosXmlTagPtr iildoc = AosJqlStatement::getDocByObjid(rdata, indexObjid);
	OmnString index_type = iildoc->getAttrStr("zky_type", "");

	//OmnString iilstr = "<index zky_iilname=\"_zt44_idx_t1_k1\" zky_name=\"idx_t1_k1\" zky_tablename=\"t1\" zky_objid=\"_zt4g_idxmgr_idx_t1_k1\" zky_pctrs=\"zky__zt_indexctnr\" zky_otype=\"zky_unknown\" zky_siteid=\"100\" zky_crtor=\"50003\" zky_ctime=\"08-11-2014 16:43:04\" zky_mtime=\"08-11-2014 16:43:04\" zky_ctmepo__d=\"1407746584\" zky_mtmepo__d=\"1407746584\" zky_docid=\"5084\"><columns><column zky_name=\"k1\" /></columns></index>";
	//AosXmlTagPtr iildoc = AosXmlParser::parse(iilstr AosMemoryCheckerArgs);
	aos_assert(iildoc);
	AosXmlTagPtr columnsNode = iildoc->getFirstChild("columns");
	aos_assert(columnsNode);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");

	OmnString composeFieldName = "";
	if (index_type == "")
	{
		boost::shared_ptr<DataProcCompose> keyDataprocComposeObj = boost::make_shared<DataProcCompose>();
		boost::shared_ptr<DataProcCompose> valueDataprocComposeObj = boost::make_shared<DataProcCompose>();

		composeFieldName = indexObjid;
		//OmnString composeFieldName = indexObjid;
		OmnString tmpname = "";
		int vv = 0;
		while (columnNode)
		{
			OmnString fieldName = columnNode->getAttrStr("zky_name");
			aos_assert(fieldName != "");
			//composeFieldName << "_" << fieldName;

			AosXmlTagPtr t_filed = table_fileds[fieldName];
			//aos_assert(t_filed);
			OmnString nfname;
			OmnString type;
			if (t_filed) {
				type = t_filed->getAttrStr("datatype", "");
			}
			nfname = AosDataType::getFieldName(fieldName, type);
			nfname = AosDataType::getFieldName(fieldName, type);
			aos_assert(nfname != "");

			OmnString tmpname = "";
			tmpname << recordName << "." << nfname;
			if (vv != 0)                                             
					keyDataprocComposeObj->setInput(tmpname, "0x01");    
			else                                                     
					keyDataprocComposeObj->setInput(tmpname);            
			vv++;                                                    
			//keyDataprocComposeObj->setInput(tmpname);

			columnNode = columnsNode->getNextChild("column");
		}
		tmpname = "";
		tmpname << recordName << composeFieldName << "__new_idx.key";
		keyDataprocComposeObj->setOutput(tmpname);

		tmpname = "";
		if (op == JQLTypes::eDrop)
		{
			tmpname << recordName << ".zky_docid";
		}
		else
		{
			tmpname << recordName << "_new.zky_docid";
		}
		valueDataprocComposeObj->setInput(tmpname);
		tmpname = "";
		tmpname << recordName << composeFieldName << "__new_idx.value";
		valueDataprocComposeObj->setOutput(tmpname);
		
		AosXmlTagPtr condNode = iildoc->getFirstChild("cond");	
		if (condNode)
		{
			OmnString condText = condNode->getNodeText();
			boost::shared_ptr<DataProcIf> procIfObj = boost::make_shared<DataProcIf>();
			procIfObj->setCondsText(condText);
			procIfObj->setDataProc(keyDataprocComposeObj);
			procIfObj->setDataProc(valueDataprocComposeObj);
			mMapDataProcs.push_back(procIfObj);
		}
		else
		{
			mMapDataProcs.push_back(keyDataprocComposeObj);
			mMapDataProcs.push_back(valueDataprocComposeObj);	
		}
	}
	else if (index_type == "carnum") {
		boost::shared_ptr<DataProcIndex> indexdataproc = boost::make_shared<DataProcIndex>();
		indexdataproc->setAttribute("type", "carnum");

		AosXmlTagPtr col1 = columnsNode->getFirstChild(true);
		AosXmlTagPtr col2 = columnsNode->getNextChild();
		OmnString col_name = col1->getAttrStr(AOSTAG_NAME);
		OmnString col2_name = col2->getAttrStr(AOSTAG_NAME);


		OmnString fieldName = columnNode->getAttrStr("zky_name");
		aos_assert(fieldName != "");

		OmnString tmpname1 = "";
		tmpname1 << recordName << "." << col_name;
		OmnString tmpname2 = "";
		tmpname2 << recordName << "_" << fieldName<< "_" << col2_name << "__new_idx.key";
		indexdataproc->setKey(tmpname1, tmpname2);

		indexdataproc->setAttribute("zky_sep", "0x01");

		AosXmlTagPtr t_filed = table_fileds[col2_name];
		aos_assert(t_filed);
		OmnString type = t_filed->getAttrStr("datatype", "");
		OmnString nfname = AosDataType::getFieldName(col2_name, type);
		aos_assert(nfname != "");
		tmpname1 = "";
		tmpname1 << recordName << "." << nfname;
		indexdataproc->setAttribute("zky_time_field", tmpname1);

		tmpname1 = "";
		tmpname1 << recordName << "_new.zky_docid";
		tmpname2 = "";
		tmpname2 << recordName << "_" << fieldName << "_" << col2_name << "__new_idx.value";
		
		indexdataproc->setValue(tmpname1, tmpname2);
			
		mMapDataProcs.push_back(indexdataproc);
	}
	else if (index_type == "wordparser")
	{
		boost::shared_ptr<DataProcIndex> indexdataproc = boost::make_shared<DataProcIndex>();
		indexdataproc->setAttribute("type", "wordparser");
		AosXmlTagPtr col = columnsNode->getFirstChild(true);
		aos_assert(col);
		OmnString col_name = col->getAttrStr(AOSTAG_NAME);

		OmnString tmpname1 = "";
		tmpname1 << recordName << "." << col_name;
		OmnString tmpname2 = "";
		tmpname2 << recordName << "_" << col_name << "__new_idx.key";
		indexdataproc->setKey(tmpname1, tmpname2);

		tmpname1 = "";
		tmpname1 << recordName << "_new.zky_docid";
		tmpname2 = "";
		tmpname2 << recordName << "_" << col_name << "__new_idx.value";
		
		indexdataproc->setValue(tmpname1, tmpname2);

		indexdataproc->setAttribute("group_type","physical_group");
			
		mMapDataProcs.push_back(indexdataproc);
	}

	// 3. reduceTask dataproc
	if (index_type == "wordparser")
	{
		boost::shared_ptr<ReduceTaskDataProc> procObj = boost::make_shared<ReduceTaskDataProc>("createhitiil");
		boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
		boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();

		int keyLen = 0;
		int valueLen = sizeof(u64);
		AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
		columnNode = columnsNode->getFirstChild("column");
		string fieldName = columnNode->getAttrStr("zky_name");

		for (size_t i=0; i<v.size(); i++)
		{
			if (v[i]->getAttribute("zky_name") == fieldName)
			{
				int len = atoi((v[i]->getAttribute("zky_length")).c_str());
				if (len <= 0) len = 50;
				keyLen += (len + 1);
				break;
			}
		}
		if (keyLen <=0) keyLen = 50;
		keyLen++;
		// felicia, 2014/11/12, for too long key
		if (keyLen > 200) keyLen = 200;

		keyFieldObj->setAttribute("zky_name", "key");
		keyFieldObj->setAttribute("type", "str");
		keyFieldObj->setAttribute("zky_offset", 0); // offset is 0
		keyFieldObj->setAttribute("zky_length", keyLen);
		keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");
		valueFieldObj->setAttribute("zky_name", "value");
		valueFieldObj->setAttribute("type", "bin_u64");
		valueFieldObj->setAttribute("zky_offset", keyLen);
		valueFieldObj->setAttribute("zky_length", valueLen);

		OmnString tmpDatasetName = indexObjid;
		procObj->setAttribute("datasetname", tmpDatasetName);
		
		//procObj->setAttribute("group_type", "physical_group");
		procObj->setAttribute("run_mode", "run_child");
		procObj->setAttribute("zky_tablename", tablename);
		procObj->setAttribute("zky_attrname", fieldName);
		procObj->setAttribute("zky_compose_iilname", "true");
		procObj->setCollectorAttr("group_type", "physical_group");
		procObj->setCollectorAttr("zky_name", tmpDatasetName);
		procObj->setCollectorType("iil");

		procObj->setAssemblerAttr("type", "strinc");
		procObj->setAssemblerAttr("zky_asmtype", "hitadd");
		procObj->setAssemblerAttr("zky_inctype", "set");

		boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", keyLen + valueLen);
		fun->setCmpField("str", 0);
		fun->setCmpField("u64", keyLen);
		fun->setAggrField("u64", 50, "set");

		procObj->setCompFun(fun);

		boost::shared_ptr<DataRecordCtnr> tmpRecordCtnrObj = boost::make_shared<DataRecordCtnr>();
		OmnString tmpname = "";
		tmpname << recordName << "_" << fieldName << "__new_idx";
		tmpRecordCtnrObj->setAttribute("zky_name", tmpname);
		boost::shared_ptr<DataRecord> tmpRecordObj = boost::make_shared<DataRecord>();
		tmpRecordObj->setAttribute("zky_name", tmpname);
		tmpRecordObj->setAttribute("type", "fixbin");
		tmpRecordObj->setAttribute("zky_length", keyLen + valueLen);

		tmpRecordObj->setField(keyFieldObj);
		tmpRecordObj->setField(valueFieldObj);
		tmpRecordCtnrObj->setRecord(tmpRecordObj);
		procObj->setRecord(tmpRecordCtnrObj);
		mReduceDataProcs.push_back(procObj);	
	}
	else 
	{
		boost::shared_ptr<ReduceTaskDataProc> procObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
		boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
		boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();

		int keyLen = 0;
		int valueLen = sizeof(u64);
		OmnString tmpFieldsName = "";
		OmnString fieldType = "";
		OmnString iilname = iildoc->getAttrStr("zky_iilname");
		aos_assert(iilname != "");
		columnNode = columnsNode->getFirstChild("column");
		while (columnNode)
		{
			string fieldName = columnNode->getAttrStr("zky_name");
			for (size_t i=0; i<v.size(); i++)
			{
				if (v[i]->getAttribute("zky_name") == fieldName)
				{
					int len = atoi((v[i]->getAttribute("zky_length")).c_str());
					if (len <= 0) len = 50;
					keyLen += (len + 1);

					fieldType = v[i]->getAttribute("type");
					break;
				}
			}
			tmpFieldsName << "_" << fieldName;	
			columnNode = columnsNode->getNextChild("column");
		}
		if (keyLen <=0) keyLen = 50;
		keyLen++;

		keyFieldObj->setAttribute("zky_name", "key");
		keyFieldObj->setAttribute("type", "str");
		keyFieldObj->setAttribute("zky_offset", 0); // offset is 0
		keyFieldObj->setAttribute("zky_length", keyLen);
		keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");
		valueFieldObj->setAttribute("zky_name", "value");
		valueFieldObj->setAttribute("type", "bin_u64");
		valueFieldObj->setAttribute("zky_offset", keyLen);
		valueFieldObj->setAttribute("zky_length", valueLen);

		OmnString tmpDatasetName = indexObjid;
		procObj->setAttribute("datasetname", tmpDatasetName);

		procObj->setAttribute("zky_iilname", iilname);

		OmnString asmType = "stradd";
		if (fieldType == "u64") asmType = "u64add";
		if (op == JQLTypes::eDrop) asmType="strdel";
		procObj->setAssemblerAttr("type", asmType);
		procObj->setAttribute(AOSTAG_OPERATOR, asmType);
		procObj->setCollectorType("iil");
		procObj->setCollectorAttr("zky_name", tmpDatasetName);

		boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", keyLen + valueLen);
		if (fieldType == "u64") 
		{
			fun->setCmpField("u64", 0);
			fun->setCmpField("u64", 8);
		}
		else 
		{
			fun->setCmpField("str", 0);
			fun->setCmpField("u64", keyLen);
		}
		procObj->setCompFun(fun);

		boost::shared_ptr<DataRecordCtnr> tmpRecordCtnrObj = boost::make_shared<DataRecordCtnr>();
		OmnString tmpname = "";

		//tmpname << recordName << tmpFieldsName << "__new_idx";
		tmpname << recordName << composeFieldName << "__new_idx";

		tmpRecordCtnrObj->setAttribute("zky_name", tmpname);
		boost::shared_ptr<DataRecord> tmpRecordObj = boost::make_shared<DataRecord>();
		tmpRecordObj->setAttribute("zky_name", tmpname);
		tmpRecordObj->setAttribute("type", "fixbin");
		tmpRecordObj->setAttribute("zky_length", keyLen + valueLen);

		tmpRecordObj->setField(keyFieldObj);
		tmpRecordObj->setField(valueFieldObj);
		tmpRecordCtnrObj->setRecord(tmpRecordObj);
		procObj->setRecord(tmpRecordCtnrObj);
		mReduceDataProcs.push_back(procObj);	
	}
}


AosJLIndex::AosJLIndex(int version)
:AosJimoLogic(version)
{
}


AosJLIndex::~AosJLIndex()
{
}
