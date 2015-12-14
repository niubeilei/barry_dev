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
//
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtLoadData.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "JQLStatement/JqlStmtJob.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLExpr/ExprString.h"
#include "AosConf/ReduceTaskDataProc.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcIf.h"
#include "AosConf/DataProcConvertdoc.h"
#include "AosConf/DataEngineScanSingle.h"
#include "AosConf/DataRecordCtnr.h"
#include "JimoProg/JimoProgLoadData.h"

#include <boost/make_shared.hpp>
using boost::shared_ptr;

AosJqlStmtLoadData::AosJqlStmtLoadData()
:
mIsTable(false),
mIsHbase(false),
mOffSet(0),
mMaxThread(eMaxThreads),
mRawKeyList(0),
mRecordLen(-1),
mMaxTask(1),
mFieldSeqno(0),
mErrmsg("")
{
	mRawKeyList = 0;
	mIsHbase = false;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtLoadData::AosJqlStmtLoadData(const OmnString &errmsg)
:
mIsTable(false),
mIsHbase(false),
mOffSet(0),
mMaxThread(eMaxThreads),
mRawKeyList(0),
mRecordLen(-1),
mMaxTask(1),
mFieldSeqno(0),
mErrmsg(errmsg)
{
	mRawKeyList = 0;
	mIsHbase = false;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtLoadData::~AosJqlStmtLoadData()
{
	if (mRawKeyList)
	{
		delete mRawKeyList;
	}
}


bool
AosJqlStmtLoadData::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	checkOutputIsTableOrDataset(rdata);
	if (mOp == JQLTypes::eCreate) return createLoadData(rdata);
	if (mOp == JQLTypes::eShow) return showLoadDatas(rdata);
	if (mOp == JQLTypes::eDrop) return dropLoadData(rdata);
	if (mOp == JQLTypes::eDescribe) return describeLoadData(rdata);
	if (mOp == JQLTypes::eRun) return runLoadData(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL LoadData", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool
AosJqlStmtLoadData::createLoadData(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg = "";
	AosXmlTagPtr job_doc = getDoc(rdata, JQLTypes::eJobDoc, mLoadDataName);
	if (job_doc)
	{
		msg << "LoadData '" << mLoadDataName << "' already exists";
		rdata->setJqlMsg(msg);
		return true;
	}

	//generate the xml data firstly
	job_doc = getJobConf(rdata);
	if (!job_doc) return false;

	job_doc->setAttr("zky_job_name", mLoadDataName);   
	job_doc->setAttr(AOSTAG_PUBLIC_DOC, "true");   
	job_doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");  
	job_doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_JOB_CONTAINER);   
	job_doc->setAttr(AOSTAG_CONTAINER_OBJID, "task_ctnr");   

	string container_objid = job_doc->getAttrStr(AOSTAG_HPCONTAINER);
	//OmnString objid = getObjidByName(rdata, JQLTypes::eJobDoc, mLoadDataName);
OmnCout << "===================== LoadData XML ==================" << endl;
OmnCout << job_doc->toString() << endl;
OmnCout << "===================== LoadData XML ==================" << endl;

	if ( !createDoc(rdata, JQLTypes::eJobDoc, mLoadDataName, job_doc) )
	{
		msg << "Failed to create LoadData doc.";
		rdata->setJqlMsg(msg);
		return false;
	}

	//runLoadData(rdata);
	
	msg << "LoadData '" << mLoadDataName << "' created successfully."; 
	//Set resp to mJQLMsg
	//Modified by MaYazong 2014-1-4
	rdata->setJqlMsg(msg);
	//setPrintData(rdata, msg);

	return true;
}


bool
AosJqlStmtLoadData::showLoadDatas(const AosRundataPtr &rdata)
{
	return true;
}


bool
AosJqlStmtLoadData::runLoadData(const AosRundataPtr &rdata)
{
	OmnString objid = getObjid(mLoadDataName);
	if ( !startJob(rdata, objid) )
	{
		OmnCout << "Failed to run LoadData ." << endl;
		return false;
	}
	OmnCout << "LoadData " << mLoadDataName << " run successfully." << endl; 
	return true;
}


bool 
AosJqlStmtLoadData::dropLoadData(const AosRundataPtr &rdata)
{
	return createLoadData(rdata);
}


bool
AosJqlStmtLoadData::describeLoadData(const AosRundataPtr &rdata)
{
	return true;
}


void
AosJqlStmtLoadData::checkOutputIsTableOrDataset(const AosRundataPtr &rdata)
{
}


OmnString
AosJqlStmtLoadData::getObjid(const OmnString name)
{
	OmnString objid = "";

	objid << AOSZTG_LOAD_DATA_IIL << "_" << name;
	return objid;
}


AosJqlStatement *
AosJqlStmtLoadData::clone()
{
	return OmnNew AosJqlStmtLoadData(*this);
}


void 
AosJqlStmtLoadData::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtLoadData::setLoadDataName(OmnString name)
{
	mLoadDataName = name;
}


void
AosJqlStmtLoadData::setToDataSet(OmnString name)
{
	mToDataSet = name;
}


void
AosJqlStmtLoadData::setFromDataSet(OmnString name)
{
	mFromDataSet = name;
}


AosXmlTagPtr
AosJqlStmtLoadData::getJobConf(const AosRundataPtr &rdata)
{
	boost::shared_ptr<Job> jobObj = boost::make_shared<Job>();	
	if (mConfParms)
	{
		return getConf(rdata, jobObj, mConfParms);
	}

	// create by young: 2014/08/22
	AosXmlTagPtr tableDoc = AosJqlStmtTable::getTable(mToDataSet, rdata);
	if (!tableDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	//AosXmlTagPtr martixNode = tableDoc->getFirstChild("matrix");
	if (mVersionNum == 2 || mOp == JQLTypes::eDrop)
	{
		AosXmlTagPtr datasetDoc = getDoc(rdata, JQLTypes::eDatasetDoc, mFromDataSet);
		if (!datasetDoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr inputDatasetsConf = AosJqlStmtJob::createInputDatasetConf(rdata, datasetDoc);
		if (!inputDatasetsConf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		//AosXmlTagPtr dsNode = inputDatasetsConf->getFirstChild("dataset");
		//if (!dsNode)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return 0;
		//}

		// run loadData2 
		list<string> list;
		//AosJPLoadData jpLoadDataObj(dsNode, tableDoc, list);
//		AosJPLoadData jpLoadDataObj(rdata, inputDatasetsConf, tableDoc, list, mOp);
//		OmnString ldConf = jpLoadDataObj.createConfig()->toString();
//
//		return AosXmlParser::parse(ldConf AosMemoryCheckerArgs);
	}
	
	// 1. input
	AosXmlTagPtr inputDatasetConf = getInputDatasetConf(rdata);
	if (!inputDatasetConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	
	// 2. output
	AosXmlTagPtr outputDatasetsConf = getOutputDatasetsConf(rdata);
	if (!outputDatasetsConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	// 3. maptask
	AosXmlTagPtr inputSchemaNode = inputDatasetConf->getFirstChild("dataschema");
	if (!inputSchemaNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	AosXmlTagPtr inputRecordNode = inputSchemaNode->getFirstChild("datarecord");
	if (!inputRecordNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	mRecordType = inputRecordNode->getAttrStr("type");
	if (mRecordType == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	mRecordName = inputRecordNode->getAttrStr("zky_name");
	if (mRecordName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	mRecordLen = inputRecordNode->getAttrInt("zky_length", 0);
	AosXmlTagPtr mapTaskConf = getMapTaskConf(rdata, inputRecordNode);
	if (!mapTaskConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	// 4. reducetask
	AosXmlTagPtr reduceTaskConf = getReduceTaskConf(rdata, inputRecordNode);
	if (!reduceTaskConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	string taskname = mLoadDataName;
	taskname += "_task";
	boost::shared_ptr<JobTask> jobTaskObj = boost::make_shared<JobTask>();
	jobTaskObj->setAttribute("zky_name", taskname);
	jobTaskObj->setAttribute("zky_otype", "task");
	jobTaskObj->setAttribute("zky_max_num_thread", mMaxThread); 

	jobTaskObj->setInputDataset(inputDatasetConf->toString());
	jobTaskObj->setOutputDataset(outputDatasetsConf->toString());
	jobTaskObj->setMapTask(mapTaskConf->toString());
	jobTaskObj->setReduceTask(reduceTaskConf->toString());

	jobObj->setJobTask(jobTaskObj);
	jobObj->setAttribute("zky_otype", "job");
	jobObj->setAttribute("zky_pctrs", "jobctnr");
	jobObj->setAttribute("zky_job_version", "1");

	OmnString jobConf = jobObj->getConfig();

	return AosXmlParser::parse(jobConf AosMemoryCheckerArgs);
}


AosXmlTagPtr 
AosJqlStmtLoadData::getInputDatasetConf(const AosRundataPtr &rdata)
{
	AosXmlTagPtr datasetDoc = getDoc(rdata, JQLTypes::eDatasetDoc, mFromDataSet);
	if (!datasetDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr inputDatasetsConf = AosJqlStmtJob::createInputDatasetConf(rdata, datasetDoc);
	if (!inputDatasetsConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	AosXmlTagPtr schemaNode = inputDatasetsConf->getFirstChild("dataschema");
	AosXmlTagPtr datarecordNode = schemaNode->getFirstChild("datarecord");
	AosXmlTagPtr datafieldsNode = datarecordNode->getFirstChild("datafields");
	vector<AosXmlTagPtr> virtualFieldDataFieldV = getVirtualFieldInputDataFieldConf(rdata);
	set<OmnString> fieldnames;
	for (size_t i=0; i<virtualFieldDataFieldV.size(); i++)
	{
		OmnString fieldName = virtualFieldDataFieldV[i]->getAttrStr("zky_name", "");
		if (fieldName == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		if (fieldnames.count(fieldName) == 0)
		{
			datafieldsNode->addNode(virtualFieldDataFieldV[i]);
			fieldnames.insert(fieldName);
		}
	}

	AosXmlTagPtr fieldNode = datafieldsNode->getFirstChild("datafield");
	while (fieldNode)
	{
		OmnString fieldName = fieldNode->getAttrStr("zky_name");
		if (fieldName == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		mInputDataFieldConfMap[fieldName] = fieldNode->toString();
		
		fieldNode = datafieldsNode->getNextChild();
	}
		
	return inputDatasetsConf;
}


vector<AosXmlTagPtr>
AosJqlStmtLoadData::getVirtualFieldInputDataFieldConf(const AosRundataPtr &rdata)
{
	// This function does:
	//
	// This function assumes 'table doc' is in the following format:
	// 	<table ...>
	// 		...
	// 		<maps ...>			(optional)
	// 			<map .../>
	// 			<map .../>
	// 			...
	// 		</maps>
	// 	</table>
	vector<AosXmlTagPtr> fieldConfV;
	AosXmlTagPtr tableDoc = AosJqlStmtTable::getTable(mToDataSet, rdata);
	if (!tableDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}

	// 1. create map case (virtual field)
	AosXmlTagPtr mapsNode = tableDoc->getFirstChild("maps");
	if (!mapsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	} 
	AosXmlTagPtr mapNode = mapsNode->getFirstChild("map");
	while (mapNode) 
	{
		vector<AosXmlTagPtr> confV = getMapDatafieldConf(rdata, mapNode);
		fieldConfV.insert(fieldConfV.begin(), confV.begin(), confV.end());
		mapNode = mapsNode->getNextChild("map");
	}

	// 2. create index case (when use virtual field)
	AosXmlTagPtr columnsNode = tableDoc->getFirstChild("columns");
	if (!columnsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		OmnString type = columnNode->getAttrStr("type");
		if (type == "virtual")
		{
			vector<AosXmlTagPtr> confV = getVirtualFieldConf(rdata, columnNode);
			fieldConfV.insert(fieldConfV.begin(), confV.begin(), confV.end());
		}
		columnNode = columnsNode->getNextChild("column");
	}
	
	return fieldConfV;
}

vector<AosXmlTagPtr> 
AosJqlStmtLoadData::getVirtualFieldConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// def's format just like this:
	// <column>
	// 	<keys>
	// 		<key>...</key>
	// 		<key>...</key>
	// 		...
	// 	</keys>
	// 	<map>
	// 		<keys>
	// 	 		<key>...</key>
	// 			<key>...</key>
	// 			...
	// 		</keys>
	// 		<values>
	// 			<value></value>
	// 		</values>
	// 	</map>
	// </column>
	
	vector<AosXmlTagPtr> fieldConfV;
	AosXmlTagPtr keysNode = def->getFirstChild("keys");
	AosXmlTagPtr mapNode = def->getFirstChild("map");
	if (!mapNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	if (!keysNode && valuesNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}

	// 1. constants key datafield config
	//    get key's length, key's name
	u32 keyLen = 0;
	OmnString keyName = "cps";
	OmnString keyfieldsConf = "";
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");
	while (keyNode)
	{
		// tmpname nust be a fieldname
		OmnString fieldname = keyNode->getNodeText();
		keyName << fieldname;
		keyfieldsConf << "<field>" << fieldname << "</field>";

		AosXmlTagPtr tmpconf= getDoc(rdata, JQLTypes::eDataFieldDoc, fieldname);
		if (!tmpconf)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return fieldConfV;
		}
		keyLen += tmpconf->getAttrU32("zky_length", 0);

		keyNode = keysNode->getNextChild("key");
	}


	// 2. constants value datafield config
	// 	  get value's name(iilname), value's length(max length)
	OmnString valueName = mapNode->getAttrStr("zky_iilname");
	OmnString need_split = "true";
	u32 valueLen = 0;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if  (valueNode)
	{
		valueLen = valueNode->getAttrU32("max_len", 0);
		if (!valueLen > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return fieldConfV;
		}
		OmnString agr_type = valueNode->getAttrStr("agrtype");	
		if (agr_type != "")
		{
			need_split = "false";
		}
		else
		{
			agr_type = "invalid";
		}
		mVirtualFieldOprType.push_back(agr_type);
	}


	// 3. push key datafield and value datafield into fieldConfV
	OmnString keyFieldStr, valueFieldStr;
	keyFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" zky_opr=\"concat\" zky_name=\"" << keyName << "\">" << keyfieldsConf << "</datafield>";

	OmnString virtualfield_name = def->getAttrStr("name");
	valueFieldStr << "<datafield type=\"iilmap\" zky_name=\"" << virtualfield_name << "\" zky_iilname=\"" << valueName << "\" zky_value_from_field=\"" << keyName << "\" zky_ignore_serialize=\"true\" zky_needswap=\"false\" zky_needsplit=\"" << need_split << "\" zky_sep=\"0x02\"></datafield>";
	OmnString tmpstr = "";
	tmpstr << keyName << "_" << valueName;
	OmnString mapName = mapNode->getAttrStr("zky_name");
	mVirtualFieldNames.push_back(make_pair(tmpstr, mapName));
	mVirtualFieldIONames.push_back(make_pair(keyName, valueName));
	mVirtualFieldIOLens.push_back(make_pair(keyLen, valueLen));

	AosXmlTagPtr keyFieldConf = AosXmlParser::parse(keyFieldStr AosMemoryCheckerArgs);
	AosXmlTagPtr valueFieldConf = AosXmlParser::parse(valueFieldStr AosMemoryCheckerArgs);
	if (!keyFieldConf && valueFieldConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	fieldConfV.push_back(keyFieldConf);
	fieldConfV.push_back(valueFieldConf);

	return fieldConfV;
}


vector<AosXmlTagPtr> 
AosJqlStmtLoadData::getMapDatafieldConf(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &mapNode)
{
	// This function will constans two datafield config with map config, 
	// one is key datafieldConf, and another is value datafieldConf.
	// The following processes one map tag. It assumes the following:
	// 	<map zky_map_name="xxx">
	// 		<keys .../>
	// 			<key ...>field_name</key>
	// 			...
	// 			<key .../>
	// 		</keys>
	// 		<values .../>
	// 			<value .../>		There should be only one value subtag
	// 		</values> 
	// 	</map>
	
	vector<AosXmlTagPtr> fieldConfV;

	AosXmlTagPtr keysNode = mapNode->getFirstChild("keys");
	AosXmlTagPtr valuesNode = mapNode->getFirstChild("values");
	if (!keysNode && valuesNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	//OmnString map_name = mapNode->getAttrStr("zky_map_name");

	u32 keylen, valuelen;
	keylen = valuelen = 0;
	OmnString keyFieldsConf, valueFieldsConf, keyName;
	keyName = "cps";
	AosXmlTagPtr keyNode = keysNode->getFirstChild("key");

	// Chen Ding, 2014/07/31
	bool isEmptyKey = false;	
	if (!keyNode)
	{
		// This is the case:
		// CREATE MAP map-name ON tablename
		// KEYS ()
		// ...
		isEmptyKey = true;
		keyName = "__zkyatvf_";
		keyName << mFieldSeqno++;
		keylen = 15; 
	}
	else
	{
		while (keyNode)
		{
			// 1. For each key field, it will create the following config:
			// 	  <field>field_name</field>
			// 2. Retrieve the field's length, and add it to keylen
			// 3. Construct 'keyName' by concatenating all key field names
			//    together. This will be used as the name of the virtual field.
			OmnString field_name = keyNode->getNodeText();
			if (field_name == "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return fieldConfV;
			}

			keyFieldsConf << "<field>" << field_name << "</field>";
			keyName << "_" << field_name;

			AosXmlTagPtr tmpFieldConf = getDoc(rdata, JQLTypes::eDataFieldDoc, field_name);
			if (!tmpFieldConf)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return fieldConfV;
			}
			keylen += tmpFieldConf->getAttrU32("zky_length", 0);
			keyNode = keysNode->getNextChild("key");
		}
		keyName << "_" << mFieldSeqno++;
	}

	OmnString valueType;
	AosXmlTagPtr valueNode = valuesNode->getFirstChild("value");
	if (valueNode)
	{
		// It assumes 'valuesNode' in the following format:
		// 	<values ...>
		// 		<value agrtype="max" type="expr" ...>
		// 			<expr>xxx</expr>
		// 		</value>
		// 	</values>
		// Young: 2014/07/29
		OmnString type = valueNode->getAttrStr("agrtype");	

		// Chen Ding, 2014/07/31
		if (isEmptyKey)
		{
			if (type != "sum" || type != "max" || type != "min" || type != "count")
			{
				OmnAlarm << "Empty key must use aggregation functions: "
					<< type << enderr;
				return fieldConfV;
			}
		}

		if (type == "") type = "invalid";
		mVirtualFieldOprType.push_back(type);
		valueType = valueNode->getAttrStr("type");

		OmnString node_text = valueNode->getNodeText();
		OmnString data_type = mapNode->getAttrStr("datatype");
		if (data_type == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return fieldConfV;
		}
		if (valueType == "expr")
		{
			// This is an expression. It needs to:
			// 1. The expression should go to the corresponding data proc.
			// 2. Need to determine field length.
			valueFieldsConf << "<expr datatype=\"" << data_type << "\">" << node_text << "</expr>";
		}
		else
		{
			// This is a single field. It retrieves the field's definition.
			// From the field definition, it can retrieve the field length.
			valueFieldsConf << "<field>" << node_text << "</field>";
		}
	}

	OmnString data_type = valueNode->getAttrStr("datatype");
	valuelen = getValueLength(data_type);
	if (valuelen <= 0) valuelen = 20;

	// Construct the key field config
	OmnString keyFieldStr, valueFieldStr;

	// Chen Ding, 2014/07/31
	if (isEmptyKey)
	{
		keyFieldStr << "<datafield type=\"str\" "
			<< AOSTAG_ISCONST << "=\"true\" "
			<< AOSTAG_VALUE << "=\"__null_entry__\" "
			<< "zky_name=\"" << keyName << "\"/>";
	}
	else
	{
		keyFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" "
			<< "zky_opr=\"concat\" "
			<< "zky_name=\"" << keyName << "\">" 
			<< keyFieldsConf << "</datafield>";
	}

	OmnString valueFieldName = getNextValueFieldName();
	if (valueType == "expr")
	{
		valueFieldStr << "<datafield "
			<< "type=\"expr\" "
			<< "zky_otype=\"datafield\" "
			<< "zky_name=\"" << valueFieldName << "\">" 
			<< valueFieldsConf << "</datafield>";
	}
	else
	{
		// It is just a single field, no expressions.
		valueFieldStr << "<datafield type=\"math\" zky_sep=\"0x01\" "
			<< "zky_opr=\"concat\" "
			<< "zky_name=\"" << valueFieldName << "\">" 
			<< valueFieldsConf << "</datafield>";
	}

	OmnString tmpstr = "";
	tmpstr << keyName << "_" << valueFieldName;
	OmnString mapname = mapNode->getAttrStr("zky_name");
	mVirtualFieldNames.push_back(make_pair(tmpstr, mapname));
	mVirtualFieldIONames.push_back(make_pair(keyName, valueFieldName));
	mVirtualFieldIOLens.push_back(make_pair(keylen, valuelen));

	AosXmlTagPtr keyFieldConf = AosXmlParser::parse(keyFieldStr AosMemoryCheckerArgs);
	AosXmlTagPtr valueFieldConf = AosXmlParser::parse(valueFieldStr AosMemoryCheckerArgs);
	if (!keyFieldConf && valueFieldConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return fieldConfV;
	}
	fieldConfV.push_back(keyFieldConf);
	fieldConfV.push_back(valueFieldConf);

	return fieldConfV;
}


AosXmlTagPtr
AosJqlStmtLoadData::getOutputDatasetsConf(const AosRundataPtr &rdata)
{
	OmnString outputDatasetsConf = "<output_datasets>";
	outputDatasetsConf << "<dataset zky_name=\"" << mToDataSet << "\"/>";
	AosXmlTagPtr tableDoc = AosJqlStmtTable::getTable(mToDataSet, rdata);
	if (!tableDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	AosXmlTagPtr indexesNode = tableDoc->getFirstChild("indexes");
	if (indexesNode)
	{
		AosXmlTagPtr indexNode = indexesNode->getFirstChild(true);
		while (indexNode)
		{
			OmnString indexObjid = indexNode->getAttrStr("idx_name");
			if (indexObjid == "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			mIndexObjids.push_back(indexObjid);	
			boost::shared_ptr<DataSet> datasetObj = boost::make_shared<DataSet>();
			datasetObj->setAttribute("zky_name", indexObjid);
			OmnString conf = datasetObj->getConfig();
			outputDatasetsConf << conf;
			OmnScreen << "outputDataset:\n" << conf << endl;

			indexNode = indexesNode->getNextChild();
		};
	}

	// Young, 2014/07/17
	AosXmlTagPtr mapsNode = tableDoc->getFirstChild("maps");
	if (!mapsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr mapNode = mapsNode->getFirstChild("map");
	if (mapNode)
	{
		OmnString type = mapNode->getAttrStr("zky_type");
		OmnString name = mapNode->getAttrStr("zky_name");
		if (type == "" || name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		OmnString tmpName = "";
		tmpName << mToDataSet << "_" << type << "_" << name;
		outputDatasetsConf << "<dataset zky_name=\"" << tmpName << "\"/>";

		mapNode = mapsNode->getNextChild();
	}
	outputDatasetsConf << "</output_datasets>";
	return AosXmlParser::parse(outputDatasetsConf AosMemoryCheckerArgs);
}


AosXmlTagPtr 
AosJqlStmtLoadData::getMapTaskConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &recordNode)
{
	// 'recordNode' format:
	// 	<record ...>
	// 		<datafields>
	// 			<datafield .../>
	// 			<datafield .../>
	// 			...
	// 		</datafields>
	// 	</record>
	//
	// 'recordNode' is retrieved from the input dataset. 
	
	OmnString recordName = recordNode->getAttrStr("zky_name");
	if (recordName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");

	AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
	boost::shared_ptr<DataProcConvertdoc> convertDocObj = boost::make_shared<DataProcConvertdoc>();
	
	vector<boost::shared_ptr<DataProc> > dataProcObjV;
	// 1. convert doc dataproc
	if (mOptionFields.empty())
	{
		while (fieldNode)
		{
			OmnString fieldName = fieldNode->getAttrStr("zky_name");
			if ( !mOptionFields.empty() && mOptionFieldsMap.count(fieldName) == 0) 
			{
				fieldNode = fieldsNode->getNextChild();
				continue; 
			}

			if ( !AosDataFieldObj::isVirtualField(fieldNode) )
			{
				boost::shared_ptr<DataFieldStr> fieldObj = boost::make_shared<DataFieldStr>();
				OmnString inputName, outputName;
				inputName << recordName << "." << fieldName;
				outputName << recordName << "_new." << fieldName;
				fieldObj->setAttribute("zky_input_field_name", inputName);
				fieldObj->setAttribute("zky_output_field_name", outputName);
				convertDocObj->setField(fieldObj);
			}
			fieldNode = fieldsNode->getNextChild();
		}
	}
	else
	{
		for (size_t i=0; i<mOptionFields.size(); i++)
		{
			OmnString fieldstr = mInputDataFieldConfMap[mOptionFields[i]];
			if (fieldstr == "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			fieldNode = AosXmlParser::parse(fieldstr AosMemoryCheckerArgs);
			if (!fieldNode)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			OmnString fieldName = fieldNode->getAttrStr("zky_name");

			boost::shared_ptr<DataFieldStr> fieldObj = boost::make_shared<DataFieldStr>();
			OmnString inputName, outputName;
			inputName << recordName << "." << fieldName;
			outputName << recordName << "_new." << fieldName;
			fieldObj->setAttribute("zky_input_field_name", inputName);
			fieldObj->setAttribute("zky_output_field_name", outputName);
			convertDocObj->setField(fieldObj);
		}
	}
	dataProcObjV.push_back(convertDocObj);

	// 2. index dataprocs 
	for (size_t i=0; i<mIndexObjids.size(); i++)
	{
		boost::shared_ptr<DataProcCompose> keyDataprocComposeObj = boost::make_shared<DataProcCompose>();
		boost::shared_ptr<DataProcCompose> valueDataprocComposeObj = boost::make_shared<DataProcCompose>();
		AosXmlTagPtr iildoc = getDocByObjid(rdata, mIndexObjids[i]);
		if (!iildoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr columnsNode = iildoc->getFirstChild("columns");
		if (!columnsNode)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");

		OmnString composeFieldName = "";
		OmnString tmpname = "";
		int vv = 0;
		while (columnNode)
		{
			OmnString fieldName = columnNode->getAttrStr("zky_name");
			if (fieldName == "")
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			composeFieldName << "_" << fieldName; 

			OmnString tmpname = "";
			tmpname << recordName << "." << fieldName;

			columnNode = columnsNode->getNextChild();
			if (vv != 0) 
				keyDataprocComposeObj->setInput(tmpname, "0x01");
			else
				keyDataprocComposeObj->setInput(tmpname);
			vv++;
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

		dataProcObjV.push_back(keyDataprocComposeObj);
		dataProcObjV.push_back(valueDataprocComposeObj);
	}

	// 3. Generate virtual field dataprocs
	AosXmlTagPtr tableDoc = AosJqlStmtTable::getTable(mToDataSet, rdata);
	if (!tableDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}
	AosXmlTagPtr mapsNode = tableDoc->getFirstChild("maps");
	if (!mapsNode)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr mapNode = mapsNode->getFirstChild("map");
	u32 counts = 0;
	while (mapNode)
	{
		// Modify by Young: 2014/08/22
		boost::shared_ptr<DataProcCompose> keyDataProcObj = boost::make_shared<DataProcCompose>();
		boost::shared_ptr<DataProcCompose> valueDataProcObj = boost::make_shared<DataProcCompose>();

		OmnString keyInputFieldName, keyInputFieldName2, keyOutputFieldName, valueInputFieldName, valueOutputFieldName;
		keyInputFieldName = keyInputFieldName2 = keyOutputFieldName = valueInputFieldName = valueOutputFieldName = recordName;

		keyInputFieldName << "." << mVirtualFieldIONames[counts].first;
		keyInputFieldName2 << "." << mVirtualFieldIONames[counts].second;
		keyOutputFieldName << "_" << mVirtualFieldNames[counts].first << "__new_idx.key";
		valueInputFieldName << "_new.zky_docid";
		valueOutputFieldName << "_" << mVirtualFieldNames[counts].first << "__new_idx.value";

		keyDataProcObj->setInput(keyInputFieldName);
		keyDataProcObj->setOutput(keyOutputFieldName);
		valueDataProcObj->setOutput(valueOutputFieldName);
		if (mVirtualFieldOprType[counts] != "invalid")
		{
			valueDataProcObj->setInput(keyInputFieldName2);
		}
		else
		{
			keyDataProcObj->setInput(keyInputFieldName2, "0x02");
			valueDataProcObj->setInput(valueInputFieldName);
		}

		AosXmlTagPtr condNode = mapNode->getFirstChild("cond");
		if (condNode)
		{
			boost::shared_ptr<DataProcIf> dataprocIfObj = boost::make_shared<DataProcIf>();
			OmnString condText = condNode->getNodeText();
			dataprocIfObj->setCondsText(condText);
			//felicia, 2014/11/19
			//dataprocIfObj->setKeyDataproc(keyDataProcObj);
			//dataprocIfObj->setValueDataproc(valueDataProcObj);
			dataprocIfObj->setDataProc(keyDataProcObj);
			dataprocIfObj->setDataProc(valueDataProcObj);
			dataProcObjV.push_back(dataprocIfObj);
		}
		else
		{
			dataProcObjV.push_back(keyDataProcObj);
			dataProcObjV.push_back(valueDataProcObj);
		}
		
		counts++;
		mapNode = mapsNode->getNextChild();
	}

	boost::shared_ptr<MapTask> mapTaskObj = boost::make_shared<MapTask>();
	boost::shared_ptr<DataEngineScanSingle> engineObj = boost::make_shared<DataEngineScanSingle>();
	for (size_t i=0; i<dataProcObjV.size(); i++) engineObj->setDataProc(dataProcObjV[i]);
	mapTaskObj->setDataEngine(engineObj);
	OmnString mapTaskStr = mapTaskObj->getConfig();
	AosXmlTagPtr mapTaskConf = AosXmlParser::parse(mapTaskStr AosMemoryCheckerArgs);
	if (!mapTaskConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	return mapTaskConf;
}


AosXmlTagPtr
AosJqlStmtLoadData::getReduceTaskConf(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &inputRecordConf)
{
	// 1. normal doc
	OmnString asmType = "docnorm"; 
	OmnString dataProcType = "importdoc_fixed";
	if (mRecordType == "csv") 
	{
		asmType = "doccsv";
		dataProcType = "importdoc_csv";
	}
	boost::shared_ptr<ReduceTask> reduceTaskObj = boost::make_shared<ReduceTask>();
	boost::shared_ptr<ReduceTaskDataProc> importProcObj = boost::make_shared<ReduceTaskDataProc>(dataProcType);
	boost::shared_ptr<DataRecordCtnr> recordCtnrObj = boost::make_shared<DataRecordCtnr>();	
	boost::shared_ptr<DataRecord> recordObj = boost::make_shared<DataRecord>();	
	boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();

	AosXmlTagPtr inputDataFieldsConf = inputRecordConf->getFirstChild("datafields");
	if (!inputDataFieldsConf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	AosXmlTagPtr fieldNode = inputDataFieldsConf->getFirstChild("datafield");

	u32 curFieldLen = 0;
	u32 curOffset = 0;
	if (mOptionFields.empty())
	{
		fieldNode = inputDataFieldsConf->getFirstChild("datafield");
		while (fieldNode)
		{
			if ( !AosDataFieldObj::isVirtualField(fieldNode) )
			{
				boost::shared_ptr<DataFieldStr> fieldObj = boost::make_shared<DataFieldStr>(fieldNode);
				recordObj->setField(fieldObj);
			}
			fieldNode = inputDataFieldsConf->getNextChild();
		}
	}
	else
	{
		for (size_t i=0; i<mOptionFields.size(); i++)
		{
			OmnString nodestr = "";
			if (mInputDataFieldConfMap.count(mOptionFields[i]))
			{
				nodestr = mInputDataFieldConfMap[mOptionFields[i]];
				AosXmlTagPtr node = AosXmlParser::parse(nodestr AosMemoryCheckerArgs);
				if (!node)
				{
					AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
					OmnAlarm << __func__ << enderr;
					return 0;
				}

				OmnString type = node->getAttrStr("type");
				if (type == "iilmap")
				{
					nodestr = "";
					nodestr << "<datafield type=\"str\" zky_length=\"20\" zky_name=\"" << mOptionFields[i] << "\" zky_otype=\"datafield\"></datafield>";
				}
			}
			else
			{
				nodestr << "<datafield type=\"str\" zky_length=\"20\" zky_name=\"" << mOptionFields[i] << "\" zky_otype=\"datafield\"></datafield>";
			}

			fieldNode = AosXmlParser::parse(nodestr AosMemoryCheckerArgs);
			if (!fieldNode)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			boost::shared_ptr<DataFieldStr> fieldObj = boost::make_shared<DataFieldStr>(fieldNode);
			fieldObj->setAttribute("zky_offset", curOffset);
			curFieldLen = fieldNode->getAttrU32("zky_length", 0);
			curOffset += curFieldLen;

			recordObj->setField(fieldObj);
		}
	}

	docidFieldObj->setAttribute("zky_name", "zky_docid");
	docidFieldObj->setAttribute("zky_otype", "datafield");
	docidFieldObj->setAttribute("type", "docid");
	recordObj->setField(docidFieldObj);

	OmnString record_name = "";
	record_name << mRecordName << "_new";
	recordCtnrObj->setRecord(recordObj);
	recordCtnrObj->setAttribute("zky_name", record_name);

	recordObj->setAttribute("zky_name", record_name);
	recordObj->setAttribute("type", mRecordType);
	
	// set record attributes
	vector<pair<OmnString, OmnString> > attrs = inputRecordConf->getAttributes();
	for (size_t i=0; i<attrs.size(); i++)
	{
		recordObj->setAttribute(attrs[i].first, attrs[i].second);
	}


	if (mRecordType == "fixbin")
	{
		//recordObj->setAttribute("zky_length", mRecordLen);
		recordObj->setAttribute("zky_length", curOffset);
		if (curOffset == 0) recordObj->setAttribute("zky_length", mRecordLen);
	}

	importProcObj->setRecord(recordCtnrObj);
	importProcObj->setAttribute("datasetname", mToDataSet);
	importProcObj->setAttribute("type", dataProcType);
	importProcObj->setAttribute("zky_otype", "dataproc");
	importProcObj->setCollectorType("doc");
	importProcObj->setCollectorAttr("zky_name", mToDataSet);
	importProcObj->setAssemblerType(asmType);
	reduceTaskObj->setDataProc(importProcObj);

	// 2.
	int fieldLen = 0;
	while (!mIndexObjids.empty())
	{
		AosXmlTagPtr iildoc = getDocByObjid(rdata, mIndexObjids[0]);
		if (!iildoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		mIndexObjids.pop_front();

		OmnString iilname = iildoc->getAttrStr("zky_iilname");
		if (iilname == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		AosXmlTagPtr columnsNode = iildoc->getFirstChild("columns");
		if (!columnsNode)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		OmnString fieldName = "";
		OmnString fieldType = "";
		AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
		while (columnNode)
		{
			OmnString name = columnNode->getAttrStr("zky_name");
			fieldName << name << "_";
			fieldNode = AosXmlParser::parse(mInputDataFieldConfMap.find(name)->second AosMemoryCheckerArgs);
			if (!fieldNode)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return 0;
			}
			fieldType = fieldNode->getAttrStr("type");
			fieldLen += fieldNode->getAttrInt("zky_length", 50);

			columnNode = columnsNode->getNextChild("column");	
		}

		if (fieldName == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
		keyFieldObj->setAttribute("zky_name", "key");
		keyFieldObj->setAttribute("type", fieldType);

		if (fieldType == "iilmap") 
		{
			keyFieldObj->setAttribute("type", "str");
		}
		else if (fieldType == "math")
		{
			keyFieldObj->setAttribute("type", "str");
		}

		keyFieldObj->setAttribute("zky_length", fieldLen);
		keyFieldObj->setAttribute("zky_offset", 0); // offset is 0
		keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");

		boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();
		valueFieldObj->setAttribute("zky_name", "value");
		valueFieldObj->setAttribute("type", "bin_u64");
		valueFieldObj->setAttribute("zky_length", "8");
		valueFieldObj->setAttribute("zky_offset", fieldLen);

		boost::shared_ptr<ReduceTaskDataProc> procObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
		//OmnString tmpDatasetName = getObjidByName(rdata, JQLTypes::eIndexDoc, fieldName);
		//procObj->setAttribute("datasetname", tmpDatasetName);
		procObj->setAttribute("datasetname", iilname);

		procObj->setAttribute("zky_iilname", iilname);
		asmType = "stradd";
		if (fieldType == "u64") asmType = "u64add";
		procObj->setAssemblerAttr("type", asmType);
		procObj->setCollectorType("iil");
		//procObj->setCollectorAttr("zky_name", tmpDatasetName);
		procObj->setCollectorAttr("zky_name", iilname);

		boost::shared_ptr<DataRecordCtnr> tmpRecordCtnrObj = boost::make_shared<DataRecordCtnr>();	
		OmnString tmpname = "";
		tmpname << mRecordName << "_" << fieldName << "_new_idx";
		tmpRecordCtnrObj->setAttribute("zky_name", tmpname);
		boost::shared_ptr<DataRecord> tmpRecordObj = boost::make_shared<DataRecord>();	
		tmpRecordObj->setAttribute("zky_name", tmpname);
		tmpRecordObj->setAttribute("type", "fixbin");
		tmpRecordObj->setAttribute("zky_length", fieldLen + 8);

		tmpRecordObj->setField(keyFieldObj);
		tmpRecordObj->setField(valueFieldObj);
		tmpRecordCtnrObj->setRecord(tmpRecordObj);
		procObj->setRecord(tmpRecordCtnrObj);
		reduceTaskObj->setDataProc(procObj);
	}

	// 3. This dataproc for "CREATE MAP"
	for (size_t i=0; i<mVirtualFieldNames.size(); i++)
	{
		OmnString datasetname = "";
		datasetname << mToDataSet << "_" << mVirtualFieldNames[i].first << "_iil";

		OmnString iilname = "_zt44_map_";
		iilname << mToDataSet << "_" << mVirtualFieldNames[i].second;

		boost::shared_ptr<DataField> keyFieldObj = boost::make_shared<DataField>();
		boost::shared_ptr<DataField> valueFieldObj = boost::make_shared<DataField>();
		keyFieldObj->setAttribute("zky_name", "key");
		keyFieldObj->setAttribute("type", "str");
		keyFieldObj->setAttribute("zky_length", mVirtualFieldIOLens[i].first+50);
		keyFieldObj->setAttribute("zky_offset", "0");
		keyFieldObj->setAttribute("zky_datatooshortplc", "cstr");

		valueFieldObj->setAttribute("zky_name", "value");
		//valueFieldObj->setAttribute("type", "str");
		//valueFieldObj->setAttribute("zky_datatooshortplc", "cstr");
		valueFieldObj->setAttribute("type", "bin_u64");
		//valueFieldObj->setAttribute("zky_length", mVirtualFieldIOLens[i].second);
		//valueFieldObj->setAttribute("zky_offset", mVirtualFieldIOLens[i].first+50);
		valueFieldObj->setAttribute("zky_length", "8");
		valueFieldObj->setAttribute("zky_offset", mVirtualFieldIOLens[i].first+50);

		OmnString recordName = "";
		recordName << mRecordName << "_" << mVirtualFieldNames[i].first << "__new_idx";
		boost::shared_ptr<DataRecordCtnr> recordCntrObj = boost::make_shared<DataRecordCtnr>();
		recordCntrObj->setAttribute("zky_name", recordName);
		boost::shared_ptr<DataRecord> recordObj = boost::make_shared<DataRecord>();
		recordObj->setAttribute("zky_name", recordName);
		recordObj->setAttribute("type", "fixbin");
		//recordObj->setAttribute("zky_length", mVirtualFieldIOLens[i].first + mVirtualFieldIOLens[i].second);
		recordObj->setAttribute("zky_length", mVirtualFieldIOLens[i].first + 8 + 50);
		recordObj->setField(keyFieldObj);
		recordObj->setField(valueFieldObj);
		recordCntrObj->setRecord(recordObj);

		boost::shared_ptr<ReduceTaskDataProc> dataprocObj = boost::make_shared<ReduceTaskDataProc>("iilbatchopr");
		dataprocObj->setAttribute("datasetname", datasetname);
		dataprocObj->setAttribute("zky_iilname", iilname);
		dataprocObj->setCollectorType("iil");
		dataprocObj->setCollectorAttr("zky_name", datasetname);
		dataprocObj->setAssemblerType("stradd");

		// Young: 2014/07/29 
		if (mVirtualFieldOprType[i] != "invalid")
		{
			dataprocObj->setAssemblerType("strinc");
			dataprocObj->setAssemblerAttr("zky_inctype", mVirtualFieldOprType[i]);
		}
		dataprocObj->setRecord(recordCntrObj);

		reduceTaskObj->setDataProc(dataprocObj);
	}
			
	OmnString reduceTaskStr = reduceTaskObj->getConfig();
	return AosXmlParser::parse(reduceTaskStr AosMemoryCheckerArgs);
}


void 
AosJqlStmtLoadData::setIsHbase(bool is_hbase)
{
	mIsHbase = is_hbase;
}


void
AosJqlStmtLoadData::setRawkeyList(AosExprList* rawkey_list)
{
	mRawKeyList = rawkey_list;
}


void 
AosJqlStmtLoadData::setOptionFields(AosExprList *fieldnames)
{
	if (fieldnames)
	{
		for (size_t i=0; i<fieldnames->size(); i++)
		{
			OmnString fieldname = getNameByExprObj( (*fieldnames)[i] );
			mOptionFields.push_back(fieldname);
			mOptionFieldsMap[fieldname] = fieldname;
		}
	}
}


OmnString
AosJqlStmtLoadData::getNextValueFieldName()
{
	OmnString name = "__zkyatvf_";
	name << mFieldSeqno++;
	return name;
}


int
AosJqlStmtLoadData::getValueLength(const OmnString &datatype)
{
	AosDataType::E dt = AosDataType::toEnum(datatype);
	//AosDataType::E dt = AosDataType::toEnum("u64");
	//if (!AosDataType::isValid(dt))
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return -1;
	//}
	return AosDataType::getValueSize(dt);
}
