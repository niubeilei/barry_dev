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
#include "JimoLogic/JimoLogicMap.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "JQLStatement/JqlStatement.h"
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

#if  0

AosJLMap::AosJLMap(
		const boost::shared_ptr<DataSet> &inputds, 
		const AosXmlTagPtr &mapNode,
		const OmnString &tableName)
:AosJimoLogic(1)
{
	//  <map zky_map_name="xxx">
	//      <keys .../>
	//          <key ...>field_name</key>
	//          ...
	//          <key .../>
	//      </keys>
	//      <values .../>
	//          <value .../>        There should be only one value subtag
	//      </values> 
	//  </map>
	
	aos_assert(mapNode);
	bool isEmptyKey = false;
	boost::shared_ptr<DataRecord> datarecord = inputds->getSchema()->getRecord();
	OmnString recordName = datarecord->getAttribute("zky_name");
	vector<boost::shared_ptr<DataField> > &v = datarecord->getFields();		
	OmnString mapName = mapNode->getAttrStr("zky_map_name");

	// 1. construct two datafields for inputdataset
	AosXmlTagPtr keysNode = mapNode->getFirstChild("keys");
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	aos_assert(keysNode && valuesNode);
	
	OmnString mapKeyName, mapValueName;
	mapKeyName << mapName << "_key";
	mapValueName << mapName << "_value";
	u32 mapKeyLen, mapValueLen;
	mapKeyLen = mapValueLen = 0;

	// 1.1 keyNode : construct datafieldObj
	boost::shared_ptr<DataFieldMath> mapKeyDataFieldObj = boost::make_shared<DataFieldMath>();
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");	
	if (!keyNode)
	{
		// This is the case:
		// CREATE MAP map-name ON tablename
		// KEYS ()
		// ...
		isEmptyKey = true;
		//mapKeyName = "__zkyatvf_";
		mapKeyLen = 15; 
		mapKeyDataFieldObj->setAttribute("type", "str");
		mapKeyDataFieldObj->setAttribute(AOSTAG_ISCONST, "true");
		mapKeyDataFieldObj->setAttribute(AOSTAG_VALUE, "__null_entry__");
		mapKeyDataFieldObj->setAttribute("zky_name", mapKeyName);

	}
	while (keyNode)
	{
		// 1. For each key field, it will create the following config:
		//    <field>field_name</field>
		// 2. Retrieve the field's length, and add it to keylen
		OmnString field_name = keyNode->getNodeText();
		aos_assert(field_name != "");
		mapKeyDataFieldObj->setAttribute("zky_opr", "concat");
		mapKeyDataFieldObj->setAttribute("zky_name", mapKeyName);
		mapKeyDataFieldObj->setFieldName(field_name);

		for (size_t i=0; i<v.size(); i++)
		{
			if (field_name == v[i]->getAttribute("zky_name"))
			{
				int len = atoi(v[i]->getAttribute("zky_length").data());
				mapKeyLen += len;
				break;
			}
		}
		keyNode = keysNode->getNextChild("key");	
	}

	// 1.2 valueNode : construct datafieldObj
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	aos_assert(valueNode);
	OmnString mapValueAgrtype = valueNode->getAttrStr("agrtype");
	if (mapValueAgrtype == "") mapValueAgrtype = "invalid";
	OmnString mapValueDatatype = valueNode->getAttrStr("datatype");
	OmnString mapValueType = valueNode->getAttrStr("type");
	OmnString mapValueNodeText = valueNode->getNodeText();
	mapValueLen = valueNode->getAttrU32("max_len", 20);
	if (isEmptyKey)
	{
		aos_assert(mapValueAgrtype != "sum" && mapValueAgrtype != "max" && 
				mapValueAgrtype != "min" && mapValueAgrtype != "count");
	}

	boost::shared_ptr<DataFieldExpr> mapValueDataFieldObj = boost::make_shared<DataFieldExpr>();
	mapValueDataFieldObj->setAttribute("zky_name", mapValueName);
	mapValueDataFieldObj->setExpr(mapValueNodeText);

	mInputFields.push_back(mapKeyDataFieldObj);
	mInputFields.push_back(mapValueDataFieldObj);

	// 2. maptask dataprocIf
	AosXmlTagPtr condNode = mapNode->getFirstChild("cond");                                                                 
	aos_assert(condNode);
	OmnString condText = condNode->getNodeText();

	OmnString keyInputFieldName, keyInputFieldName2, keyOutputFieldName, valueInputFieldName, valueOutputFieldName;
	keyInputFieldName = keyInputFieldName2 = keyOutputFieldName = valueInputFieldName = valueOutputFieldName = recordName;

	keyInputFieldName << "." << mapKeyName;
	keyInputFieldName2 << "." << mapValueName;
	keyOutputFieldName << "_" << mapKeyName << "_" << mapValueName  << "__new_idx.key";
	valueInputFieldName << "_new.zky_docid";
	valueOutputFieldName << "_" << mapKeyName << "_" << mapValueName << "__new_idx.value";

	boost::shared_ptr<DataProcIf> dataprocIfObj = boost::make_shared<DataProcIf>();
	dataprocIfObj->setCondsText(condText);
	dataprocIfObj->setKeyInputFieldName(keyInputFieldName);
	dataprocIfObj->setKeyOutputFieldName(keyOutputFieldName);
	dataprocIfObj->setValueOutputFieldName(valueOutputFieldName);
	if (mapValueAgrtype != "invalid")
	{
		dataprocIfObj->setValueInputFieldName(keyInputFieldName2);
	}
	else
	{
		dataprocIfObj->setKeyInputFieldName(keyInputFieldName2);
		dataprocIfObj->setValueInputFieldName(valueInputFieldName);
	}
	mMapDataProcs.push_back(dataprocIfObj);

	// 3. reduceTask dataproc
	OmnString datasetname = "";
	datasetname << tableName << "_" << mapKeyName << "_" << mapValueName << "_iil";
	OmnString iilname = mapNode->getAttrStr("zky_iilname");

	boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
	boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();
	keyFieldObj->setAttribute("zky_name", "key");
	keyFieldObj->setAttribute("type", "str");
	keyFieldObj->setAttribute("zky_length", mapKeyLen+50);
	keyFieldObj->setAttribute("zky_offset", "0");
	keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");

	valueFieldObj->setAttribute("zky_name", "value");
	valueFieldObj->setAttribute("type", "bin_u64");
	valueFieldObj->setAttribute("zky_length", "8");
	valueFieldObj->setAttribute("zky_offset", mapKeyLen+50);

	OmnString tmpRecordName = "";
	tmpRecordName << recordName << "_" << mapKeyName << "_" << mapValueName << "__new_idx";
	boost::shared_ptr<DataRecordCtnr> recordCntrObj = boost::make_shared<DataRecordCtnr>();
	recordCntrObj->setAttribute("zky_name", recordName);
	boost::shared_ptr<DataRecord> recordObj = boost::make_shared<DataRecord>();
	recordObj->setAttribute("zky_name", recordName);
	recordObj->setAttribute("type", "fixbin");
	recordObj->setAttribute("zky_length", mapKeyLen + 8 + 50);
	recordObj->setField(keyFieldObj);
	recordObj->setField(valueFieldObj);
	recordCntrObj->setRecord(recordObj);

	boost::shared_ptr<ReduceTaskDataProc> dataprocObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
	dataprocObj->setAttribute("datasetname", datasetname);
	dataprocObj->setAttribute("zky_iilname", iilname);
	dataprocObj->setCollectorType("iil");
	dataprocObj->setCollectorAttr("zky_name", datasetname);
	dataprocObj->setAssemblerType("stradd");
	dataprocObj->setRecord(recordCntrObj);

	// Young: 2014/07/29 
	if (mapValueAgrtype != "invalid")
	{
		dataprocObj->setAssemblerType("strinc");
		dataprocObj->setAssemblerAttr("zky_inctype", mapValueAgrtype);

		boost::shared_ptr<CompFun> fun = boost::make_shared<CompFun>("custom", mapKeyLen + 8 + 50);
		fun->setCmpField("str", 0);
		fun->setCmpField("u64", mapKeyLen);
		fun->setAggrField("u64", mapKeyLen, mapValueAgrtype);
		dataprocObj->setCompFun(fun);
	}	

	mReduceDataProcs.push_back(dataprocObj);	
}


AosJLMap::AosJLMap(int version)
:AosJimoLogic(version)
{
}


AosJLMap::~AosJLMap()
{
}
#endif
