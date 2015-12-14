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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogic.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataRecordCtnr.h"

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

using boost::make_shared;
using boost::shared_ptr;


AosJimoLogic::AosJimoLogic()
:AosJimoLogicObj(1)
{
}


AosJimoLogic::AosJimoLogic(int version)
:AosJimoLogicObj(version)
{
}


AosJimoLogic::~AosJimoLogic()
{
}


AosJimoLogicObjPtr 
AosJimoLogic::createJimoLogic(
		const AosXmlTagPtr &descriptor,
		AosRundata *rdata)
{
	return 0;
}


bool
AosJimoLogic::setOutputDataset(const AosXmlTagPtr &def)
{
	// def format like this:
	//<index zky_iilname=_zt44_idx_t1_k1 zky_name=idx_t1_k1 zky_tablename=t1 zky_objid=_zt4g_idxmgr_idx_t1_k1>
	//	 <columns>
	//	   <column zky_name=k1/>
	//	</columns>
	//</index>
	OmnString objid = def->getAttrStr("zky_objid");
	aos_assert_r(objid != "", false);
	boost::shared_ptr<DataSet> datasetObj = boost::make_shared<DataSet>();
	datasetObj->setAttribute("zky_name", objid);
	mOutputds.push_back(datasetObj);

	return true;
}


bool
AosJimoLogic::setMapTaskDataProc(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &def,
		map<OmnString, AosXmlTagPtr> &table_fileds)
{
	// def format like this:
	//<index zky_iilname=_zt44_idx_t1_k1 zky_name=idx_t1_k1 zky_tablename=t1 zky_objid=_zt4g_idxmgr_idx_t1_k1>
	//	 <columns>
	//	   <column zky_name=k1/>
	//	   <column zky_name=k1/>
	//	   ...
	//	</columns>
	//</index>
	boost::shared_ptr<DataRecord> recordObj = inputds->getSchema()->getRecord();
	OmnString recordName = recordObj->getAttribute("zky_name");
	aos_assert_r(recordName != "", false);

	boost::shared_ptr<DataProcCompose> keyDataprocComposeObj = boost::make_shared<DataProcCompose>();
	boost::shared_ptr<DataProcCompose> valueDataprocComposeObj = boost::make_shared<DataProcCompose>();
	AosXmlTagPtr columnsNode = def->getFirstChild("columns");
	aos_assert_r(columnsNode, false);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");

	OmnString composeFieldName = "";
	OmnString tmpname = "";
	while (columnNode)
	{
		OmnString fieldName = columnNode->getAttrStr("zky_name");
		aos_assert_r(fieldName != "", false);
		composeFieldName << "_" << fieldName;

		AosXmlTagPtr t_filed = table_fileds[fieldName];
		aos_assert_r(t_filed, false);
		OmnString type = t_filed->getAttrStr("datatype", "");
		OmnString nfname = AosDataType::getFieldName(fieldName, type);
		aos_assert_r(nfname != "", false);

		OmnString tmpname = "";
		tmpname << recordName << "." << nfname;
		keyDataprocComposeObj->setInput(tmpname);

		columnNode = columnsNode->getNextChild("column");
	}
	tmpname = "";
	tmpname << recordName << composeFieldName << "__new_idx.key";
	keyDataprocComposeObj->setOutput(tmpname);

	tmpname = "";
	tmpname << recordName << "_new.zky_docid";
	valueDataprocComposeObj->setInput(tmpname);
	tmpname = "";
	tmpname << recordName << composeFieldName << "__new_idx.value";
	valueDataprocComposeObj->setOutput(tmpname);

	mMapDataProcs.push_back(keyDataprocComposeObj);
	mMapDataProcs.push_back(valueDataprocComposeObj);

	return true;
}


bool
AosJimoLogic::setReduceDataProc(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &def)
{
	// def format like this:
	//<index zky_iilname=_zt44_idx_t1_k1 zky_name=idx_t1_k1 zky_tablename=t1 zky_objid=_zt4g_idxmgr_idx_t1_k1>
	//	 <columns>
	//	   <column zky_name=k1/>
	//	   <column zky_name=k1/>
	//	   ...
	//	</columns>
	//</index>
	
	boost::shared_ptr<DataRecord> recordObj = inputds->getSchema()->getRecord();
	OmnString recordName = recordObj->getAttribute("zky_name");
	aos_assert_r(recordName != "", false);
	vector<boost::shared_ptr<DataField> > &v = recordObj->getFields();

	boost::shared_ptr<ReduceTaskDataProc> procObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
	boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();

	int keyLen = 0;
	int valueLen = sizeof(u64);
	OmnString tmpFieldsName = "";
	OmnString fieldType = "";
	OmnString iilname = def->getAttrStr("zky_iilname");
	aos_assert_r(iilname != "", false);
	AosXmlTagPtr columnsNode = def->getFirstChild("columns");
	aos_assert_r(columnsNode, false);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		string fieldName = columnNode->getAttrStr("zky_name");
		for (size_t i=0; i<v.size(); i++)
		{
			if (v[i]->getAttribute("zky_name") == fieldName)
			{
				int len = atoi((v[i]->getAttribute("zky_length")).c_str());
				if (len <= 0) len = 50;
				keyLen += len;

				fieldType = v[i]->getAttribute("type");
				break;
			}
		}
		tmpFieldsName << "_" << fieldName;
		columnNode = columnsNode->getNextChild("column");
	}

	keyFieldObj->setAttribute("zky_name", "key");
	keyFieldObj->setAttribute("type", "str");
	keyFieldObj->setAttribute("zky_offset", 0); // offset is 0
	keyFieldObj->setAttribute("zky_length", keyLen);
	keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");
	valueFieldObj->setAttribute("zky_name", "value");
	valueFieldObj->setAttribute("type", "bin_u64");
	valueFieldObj->setAttribute("zky_offset", keyLen);
	valueFieldObj->setAttribute("zky_length", valueLen);

	OmnString tmpDatasetName = def->getAttrStr("zky_objid");
	procObj->setAttribute("datasetname", tmpDatasetName);

	procObj->setAttribute("zky_iilname", iilname);
	OmnString asmType = "stradd";
	if (fieldType == "u64") asmType = "u64add";
	procObj->setAssemblerAttr("type", asmType);
	procObj->setCollectorType("iil");
	procObj->setCollectorAttr("zky_name", tmpDatasetName);

	boost::shared_ptr<DataRecordCtnr> tmpRecordCtnrObj = boost::make_shared<DataRecordCtnr>();
	OmnString tmpname = "";
	tmpname << recordName << tmpFieldsName << "__new_idx";
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

	return true;
}
