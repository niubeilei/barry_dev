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
#include "JimoLogic/JLPattern001.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JQLStatement/JqlStatement.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataProcIf.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataField.h"
#include "AosConf/DataFieldStr.h"
#include "AosConf/DataFieldMath.h"
#include "AosConf/DataFieldExpr.h"

#include <boost/make_shared.hpp>
using namespace AosConf;
using boost::make_shared;
using boost::shared_ptr;


AosJLPattern001::AosJLPattern001(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &def,
		map<OmnString, AosXmlTagPtr> &table_fileds)
:AosJimoLogic(1)
{
	// 'def' is in the following format:
	//<matrix zky_name="logicname" zky_type="pattern1" zky_objid="xxx">
	//	<columns>
	// 		<endpoint>...</endpoint>
	// 		<time>...</time>
	//		<checkpoint>...</checkpoint>
	// <columns>
	// <matrix>...</matrix>
	// <results>...</results>
	//</matrix>
	
	boost::shared_ptr<DataRecord> recordObj = inputds->getSchema()->getRecord();
	OmnString recordName = recordObj->getAttribute("zky_name");
	aos_assert(recordName != "");
	vector<boost::shared_ptr<DataField> > &v = recordObj->getFields();

	// 1. output dataset
	OmnString objid = def->getAttrStr("zky_objid");
	aos_assert(objid != "");
	boost::shared_ptr<DataSet> datasetObj = boost::make_shared<DataSet>();
	datasetObj->setAttribute("zky_name", objid);
	mOutputds.push_back(datasetObj);

	// 2. maptask dataproc (a pair)
	boost::shared_ptr<DataProcCompose> keyDataprocComposeObj = boost::make_shared<DataProcCompose>();
	boost::shared_ptr<DataProcCompose> valueDataprocComposeObj = boost::make_shared<DataProcCompose>();

	AosXmlTagPtr columnsNode = def->getFirstChild("columns");
	aos_assert(columnsNode);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild();

	OmnString composeFieldName = "";
	OmnString tmpname = "";
	int idx = 0;
	while (columnNode)
	{
		//OmnString fieldName = columnNode->getAttrStr("zky_name");
		OmnString fieldName = columnNode->getNodeText();
		aos_assert(fieldName != "");
		composeFieldName << "_" << fieldName;

		AosXmlTagPtr t_filed = table_fileds[fieldName];
		aos_assert(t_filed);
		OmnString type = t_filed->getAttrStr("datatype", "");
		OmnString nfname = AosDataType::getFieldName(fieldName, type);
		aos_assert(nfname != "");

		OmnString tmpname = "";
		tmpname << recordName << "." << nfname;
		
		if(idx > 0)
		{
			keyDataprocComposeObj->setInput(tmpname,"0x01");
		}
		else
		{
			keyDataprocComposeObj->setInput(tmpname);
		}

		idx++;
		columnNode = columnsNode->getNextChild();
	}

	OmnString jimoLogicName = def->getAttrStr("zky_name");
	tmpname = "";
	tmpname << jimoLogicName << "__new_idx.key";
	keyDataprocComposeObj->setOutput(tmpname);

	tmpname = "";
	tmpname << recordName << "_new.zky_docid";
	valueDataprocComposeObj->setInput(tmpname);
	tmpname = "";
	tmpname << jimoLogicName << "__new_idx.value";
	valueDataprocComposeObj->setOutput(tmpname);

	mMapDataProcs.push_back(keyDataprocComposeObj);
	mMapDataProcs.push_back(valueDataprocComposeObj);	

	// 3. reduceTask dataproc
	boost::shared_ptr<ReduceTaskDataProc> procObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
	boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();

	int keyLen = 0;
	int valueLen = sizeof(u64);
	OmnString fieldType = "";
	OmnString iilname = def->getAttrStr("zky_iilname");
	aos_assert(iilname != "");
	columnNode = columnsNode->getFirstChild();
	while (columnNode)
	{
		string fieldName = columnNode->getAttrStr("zky_name");
		int len = 0;
		for (size_t i=0; i<v.size(); i++)
		{
			if (v[i]->getAttribute("zky_name") == fieldName)
			{
				int len = atoi((v[i]->getAttribute("zky_length")).c_str());
				if (len <=0) len  = 50;
				fieldType = v[i]->getAttribute("type");
				break;
			}
		}
		if (len <=0) len = 20;
		keyLen += len;
		columnNode = columnsNode->getNextChild();
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

	OmnString tmpDatasetName = objid;
	procObj->setAttribute("datasetname", tmpDatasetName);

	procObj->setAttribute("zky_iilname", iilname);
	OmnString type = def->getAttrStr("zky_type");
	AosXmlTagPtr matrixNode = def->getFirstChild("matrix");
	OmnString matrixNodeText = matrixNode->getNodeText();
	AosXmlTagPtr resultsNode = def->getFirstChild("results");
	OmnString resultsNodeText = resultsNode->getNodeText();

	//felicia, 2014/10/17, for overspeed
	AosXmlTagPtr matrixNode2 = def->getFirstChild("matrix2");
	if(matrixNode2)
	{
		OmnString speed_iilname = matrixNode2->getNodeText();
		procObj->setAttribute("zky_speed_iilname", speed_iilname);
	}


	//procObj->setAttribute("matrix", matrixNodeText);
	//procObj->setAttribute("results", resultsNodeText);
	procObj->setAttribute("zky_map_iilname", matrixNodeText);
	procObj->setAttribute("zky_merger_iilname", resultsNodeText);
	procObj->setAttribute("type", type);

	OmnString asmType = "stradd";
	if (fieldType == "u64") asmType = "u64add";
	procObj->setAssemblerAttr("type", asmType);
	procObj->setCollectorType("iil");
	procObj->setCollectorAttr("zky_name", tmpDatasetName);

	boost::shared_ptr<DataRecordCtnr> tmpRecordCtnrObj = boost::make_shared<DataRecordCtnr>();
	tmpname = "";
	tmpname << jimoLogicName << "__new_idx";
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


AosJLPattern001::AosJLPattern001(int version)
:AosJimoLogic(version)
{
}


AosJLPattern001::~AosJLPattern001()
{
}
