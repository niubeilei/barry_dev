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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtDataset.h"

#include "JQLStatement/JqlStmtSchema.h"
#include "JQLStatement/JqlStmtDatascanner.h"
#include "JQLStatement/JqlStmtDataConnector.h"
#include "JQLStatement/JqlStmtDataRecord.h"
#include "JQLStatement/JqlStmtDataField.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"

#include "AosConf/DataSet.h"
#include "AosConf/DataRecordMulti.h"
#include "AosConf/DataSchemaMultiRecord.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtDataset::AosJqlStmtDataset()
{
	mName = "";
	mScanner = "";
	mSchema = "";
	mComment = "";
	mOp = JQLTypes::eOpInvalid;
	mErrmsg = "";
}


AosJqlStmtDataset::AosJqlStmtDataset(const OmnString &errmsg)
{
	mName = "";
	mScanner = "";
	mSchema = "";
	mComment = "";
	mOp = JQLTypes::eOpInvalid;
	mErrmsg = errmsg;
}


bool
AosJqlStmtDataset::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
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

	if (mOp == JQLTypes::eCreate) return createDataset(rdata);
	if (mOp == JQLTypes::eShow) return showDatasets(rdata); 
	if (mOp == JQLTypes::eDrop) return dropDataset(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataset(rdata);

	AosSetEntityError(rdata, "JQL_dataset_run_err", "JQL Dataset", "")  
		    << "opr is undefined!" << enderr;                                                
	return false;                                                        
}

bool
AosJqlStmtDataset::createDataset(const AosRundataPtr &rdata)
{
	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDatasetDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (doc)
	{
		msg << "DataSet " << mName << " already exists. "
			  << " Use \"drop\" command to delete firstly.";	
		rdata->setJqlMsg(msg);
		return true;
	}

	doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr(AOSTAG_OBJID, objid);

	if ( !createDoc(rdata, JQLTypes::eDatasetDoc, mName, doc) )
	{
		msg << "Failed to create dataset doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "DataSet " << mName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDataset::convertToXml(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataSet> dset = boost::make_shared<DataSet>();
	if (mConfParms)
	{
		return getConf(rdata, dset, mConfParms);
	}

	dset->setAttribute("zky_name", mName);
	dset->setAttribute("zky_otype", "dataset");
	dset->setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");

	// Young, 2014/10/13
	if (mScanner == "" || mSchema == "")
	{
		return AosXmlParser::parse(dset->getConfig() AosMemoryCheckerArgs);
	}
	
	// have some questions.... (schema)
	AosXmlTagPtr scannerDoc = getDoc(rdata, JQLTypes::eDataScannerDoc, mScanner);
	AosXmlTagPtr schemaDoc = getDoc(rdata, JQLTypes::eSchemaDoc, mSchema);
	if (!scannerDoc || !schemaDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return NULL;
	}

	//boost::shared_ptr<DataScanner> scannerObj = boost::make_shared<DataScanner>(scannerDoc);
	//boost::shared_ptr<DataSchema> schemaObj;

	//AosXmlTagPtr datarecords = schemaDoc->getFirstChild("datarecords");
	//if (!datarecords)
	//{
	//	AosXmlTagPtr node = schemaDoc->getFirstChild();
	//	if (node)
	//	{
	//		OmnString type = schemaDoc->getAttrStr("type");
	//		if (type == "multi")
	//		{
	//			schemaObj = boost::make_shared<DataRecordMulti>(schemaDoc);
	//		}
	//	}
	//	
	//	schemaObj = boost::make_shared<DataSchema>(schemaDoc);
	//}
	//else
	//{
	//	schemaObj = boost::make_shared<DataSchemaMultiRecord>(schemaDoc);
	//}

	//dset->setScanner(scannerObj);
	//dset->setSchema(schemaObj);
	AosXmlTagPtr datarecord = schemaDoc->getFirstChild("datarecord");
	OmnString type = datarecord->getAttrStr("type");
	if (type == "multi")
	{
		AosXmlTagPtr clone_schemaDoc = schemaDoc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(clone_schemaDoc, NULL);
		clone_schemaDoc->removeAllChildren();
		schemaDoc = clone_schemaDoc;
	}
	dset->setScanner(scannerDoc->toString());
	dset->setSchema(schemaDoc->toString());

	return AosXmlParser::parse(dset->getConfig() AosMemoryCheckerArgs); 
}

bool
AosJqlStmtDataset::showDatasets(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> datasets;
	if ( !getDocs(rdata, JQLTypes::eDatasetDoc, datasets) )
	{
		msg << "No Data Set Found. ";
		rdata->setJqlMsg(msg);
		return true;
	}

	int total = datasets.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<datasets.size(); i++)
	{
		AosXmlTagPtr dataset = datasets[i];
		AosXmlTagPtr doc;
		string name;
		string schema;
		string scanner;
		name = dataset->getAttrStr("zky_name", "");
		doc = dataset->getFirstChild("datascanner");
		scanner = doc->getAttrStr("zky_name", "");
		doc = dataset->getFirstChild("dataschema");
		schema = doc->getAttrStr("zky_name", "");
		content << "<record name=\"" << name << "\" schema=\"" << schema << "\" scanner=\"" << scanner <<"\" />";
	}
	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("name");
	map<OmnString, OmnString> alias_name;
	alias_name["name"] = "DataSetName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}

bool	
AosJqlStmtDataset::dropDataset(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDatasetDoc, mName);
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                        
		msg << "Failed to delete the dataset " << mName;
		rdata->setJqlMsg(msg);
		return false;                                        
	}                                                        
	msg << "DataSet " << mName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
	return true;
}

bool
AosJqlStmtDataset::describeDataset(const AosRundataPtr &rdata)
{
	if (mName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 

	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDatasetDoc, mName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display dataset " << mName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;
	fields.push_back("zky_name");
	fields.push_back("type");
	fields.push_back("zky_datascanner_name");
	fields.push_back("zky_dataschema_name");
	alias_name["zky_datarecord_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["zky_datascanner_name"] = "DataScannerName";
	alias_name["zky_length"] = "DataRecordName";

	OmnString content = "<content>";
	content << doc->toString() << "</content>";
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);

	return true;
}

AosJqlStatement *
AosJqlStmtDataset::clone()
{
	return OmnNew AosJqlStmtDataset(*this);
}


void 
AosJqlStmtDataset::setName(OmnString name) {
	mName = name;
}

void 
AosJqlStmtDataset::setScanner(OmnString name) {
	mScanner = name;
}

void 
AosJqlStmtDataset::setSchema(OmnString name) {
	mSchema = name;
}

void 
AosJqlStmtDataset::setComment(OmnString comment) {
	mComment = comment;
}

bool
AosJqlStmtDataset::convertDatasetConf(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &dataset_doc)
{
	// format:
	//<dataset zky_name="vc_792_dataset" zky_otype="dataset">
	//	<datascanner zky_name="vc_792_scanner"/>
	//	<dataschema zky_name="vc_792_schema"/>
	//</dataset>
	
	OmnString msg;
	OmnString aname = "zky_name";
	OmnString avalue= dataset_doc->getAttrStr(aname, "");
	if (avalue == "")
	{
		msg << "Failed to get " << aname;
		rdata->setJqlMsg(msg);
		return false;
	}

	OmnString tagname = "datascanner";
	AosXmlTagPtr scanner_tag = dataset_doc->getFirstChild(tagname);
	if (!scanner_tag)
	{
		msg << "Dataset config error, missing " << tagname << " node.";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr result = createDataScannerConf(rdata, scanner_tag);
	if (!result)
	{
		return false;
	}

	OmnString tagname1 = "dataschema";
	AosXmlTagPtr schema_tag = dataset_doc->getFirstChild(tagname1);
	if (!schema_tag)
	{
		msg << "Dataset config error, missing " << tagname1 << " node.";
		rdata->setJqlMsg(msg);
		return false;
	}

	AosXmlTagPtr result1 = createDataSchemaConf(rdata, schema_tag);
	if (!result1)
	{
		return false;
	}

	dataset_doc->removeNode(tagname, false, false);
	dataset_doc->removeNode(tagname1, false, false);
	dataset_doc->addNode(result);
	dataset_doc->addNode(result1);
	modifyName(dataset_doc, dataset_doc, aname, avalue);	
	return true;
}

AosXmlTagPtr
AosJqlStmtDataset::createDataScannerConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//format:
	//<datascanner zky_name="vc_792_scanner" zky_otype="datasanner" 
	//	zky_jimo_objid="datascanner_parallel_jimodoc_v1" zky_public_doc="true" zky_public_ctnr="true" zky_objid="test_4">
	//	<dataconnector zky_name="vc_792_file"/>
	//</datascanner>
	OmnString msg;
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		msg << "Failed to get " << aname;
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr scanner_doc = getDoc(rdata, JQLTypes::eDataScannerDoc, avalue);
	if (!scanner_doc)
	{
		msg << "Failed to get datascannerdoc '" << avalue << "'";
		rdata->setJqlMsg(msg);
		return 0;
	}

	OmnString tagname = "dataconnector";
	AosXmlTagPtr connector_tag = scanner_doc->getFirstChild(tagname);
	if (!connector_tag)
	{
		msg << "DataScanner config error , missing " << tagname << " node.";
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr result = createConnectorConf(rdata, connector_tag);
	if (!result)
	{
		return 0;
	}
	scanner_doc->removeNode(tagname, false, false);
	scanner_doc->addNode(result);

	modifyName(def, scanner_doc, aname, avalue);	
	return scanner_doc;
}


AosXmlTagPtr
AosJqlStmtDataset::createConnectorConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//<dataconnector zky_name="vc_792_file" zky_otype="dataconnector" 
	//	zky_jimo_objid="dataconnector_file_jimodoc_v1" AOSTAG_PHYSICALID="0"
	//	file_name="/home/prod/vc_792_file.txt" zky_character="UTF-8">
	//	<split zky_jimo_objid="dataspliter_filesize_jimodoc_v1" group_size="1G"></split>
	//</dataconnector>
	OmnString msg;
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		msg << "Failed to get " << aname;
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr connector_doc = getDoc(rdata, JQLTypes::eDataConnectorDoc, avalue);
	if (!connector_doc)
	{
		msg << "Failed to get dataconnectordoc '" << avalue << "'";
		rdata->setJqlMsg(msg);
		return 0;
	}

	modifyName(def, connector_doc, aname, avalue);	
	return connector_doc;
}


AosXmlTagPtr
AosJqlStmtDataset::createDataSchemaConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	//format:
	//<dataschema zky_name="dataschemaname" zky_otyp="dataschema" 
	//	zky_jimo_objid="dataschema_unilength_jimodoc_v1">
	//		<record zky_name="vc_792_rcd"/>
	//</dataschema>	
	OmnString msg;
	OmnString otype = "dataschema";	
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	if (avalue == "")
	{
		msg << "Failed to get " << aname;
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr schema_doc = getDoc(rdata, JQLTypes::eSchemaDoc, avalue);
	if (!schema_doc)
	{
		msg << "Failed to get schema '" << avalue << "' doc";
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr record_tag = schema_doc->getFirstChild("record"); 
	if (!record_tag)
	{
		msg << "DataSchema config error, missing record node.";
		rdata->setJqlMsg(msg);
		return 0;
	}

	OmnString tagname = record_tag->getTagname();

	AosXmlTagPtr result = createDataRecordConf(rdata, record_tag);	
	if (!result)
	{
		return 0;
	}

	schema_doc->removeNode(tagname, false, false);
	schema_doc->addNode(result);
	modifyName(def, schema_doc, aname, avalue);	
	return schema_doc;
}


AosXmlTagPtr
AosJqlStmtDataset::createDataRecordConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format:
	//<datarecord zky_name="vc_792" zky_otype="datarecord" type="fixbin" length="792">
	//	<datafields>
	//		<datafield zky_name="system_type"/>
	//		<datafield zky_name="msisdn"/>
	//		<datafield zky_name="other_party"/>
	//		<datafield zky_name="start_datetime"/>
	//		<datafield zky_name="call_duration"/>
	//		<datafield zky_name="all"/>
	//	</datafields>
	//</datarecord>
	OmnString msg;
	OmnString otype = "datarecord";	
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, "");
	AosXmlTagPtr record_doc = getDoc(rdata, JQLTypes::eDataRecordDoc, avalue);
	if ( !record_doc )
	{
		msg << "Fail to get record '" << avalue << "'doc";
		rdata->setJqlMsg(msg);
		return 0;
	}

	OmnString record_type = record_doc->getAttrStr(AOSTAG_TYPE);
	if (record_type == "")
	{
		msg << "DataRecord config error, missing " << AOSTAG_TYPE << " attribute.";
		rdata->setJqlMsg(msg);
		return 0;
	}

	OmnString tagname = "datafields";
	AosXmlTagPtr fields_tag = record_doc->getFirstChild(tagname);
	if (!fields_tag)
	{
		msg << "DataRecord config error, missing " << tagname << " node.";
		rdata->setJqlMsg(msg);
		return 0;
	}

	int next_length = 0;
	vector<AosXmlTagPtr> result;
	AosXmlTagPtr field_tag = fields_tag->getFirstChild();
	while(field_tag)
	{
		AosXmlTagPtr tag = createDataFieldConf(rdata, field_tag);
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}

		OmnString type = tag->getAttrStr(AOSTAG_TYPE);
		if (record_type == AOSRECORDTYPE_FIXBIN && 
				(type == AOSDATAFIELDTYPE_STR || type == AOSDATAFIELDTYPE_U64))
		{
			int off = tag->getAttrInt(AOSTAG_OFFSET, -1);
			if (off == -1)
			{
				tag ->setAttr(AOSTAG_OFFSET, next_length);
				off = next_length;
			}

			int len = tag->getAttrInt(AOSTAG_LENGTH, 0);
			if (type == AOSDATAFIELDTYPE_U64 && len == 0)
			{
				len = sizeof(u64);
				tag->setAttr(AOSTAG_LENGTH, len);
			}
			next_length = off + len;
		}

		result.push_back(tag);
		field_tag = fields_tag->getNextChild();
	}

	int len = record_doc->getAttrInt(AOSTAG_LENGTH, 0);
	if (len != 0)
	{
		if (!next_length <= len)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
	}
	if (len == 0 && record_type == AOSRECORDTYPE_FIXBIN)
	{
		record_doc->setAttr(AOSTAG_LENGTH, next_length);
	}
	record_doc->removeNode(tagname, false, false);
	fields_tag = record_doc->addNode1(tagname);

	for(u32 i = 0; i < result.size(); i++)
	{
		fields_tag->addNode(result[i]);
	}

	modifyName(def, record_doc, aname, avalue);	
	return record_doc;
}


AosXmlTagPtr
AosJqlStmtDataset::createDataFieldConf(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// format: <datafield zky_name="system_type" zky_otype="datafiled" type="str" zky_length="2" zky_offset="0"/>
	OmnString msg;
	OmnString otype = "datafield";
	OmnString aname = "zky_name";
	OmnString avalue = def->getAttrStr(aname, ""); 
	if (avalue == "")
	{
		msg << "Failed to get " << aname;
		rdata->setJqlMsg(msg);
		return 0;
	}

	AosXmlTagPtr field_doc = getDoc(rdata, JQLTypes::eDataFieldDoc, avalue);
	if (!field_doc)
	{
		msg << "Failed to get " << otype << "doc '" << avalue << "'";
		rdata->setJqlMsg(msg);
		return 0;
	}

	modifyName(def, field_doc, aname, avalue);
	return field_doc;
}


void
AosJqlStmtDataset::modifyName(
		const AosXmlTagPtr &def,
		const AosXmlTagPtr &doc,
		const OmnString &aname,
		const OmnString &avalue)
{
	//doc->removeAttr(aname);
	//doc->removeAttr(AOSTAG_NAME);
	doc->removeAttr(aname, 0, true);
	doc->removeAttr(AOSTAG_NAME, 0, true);
	doc->removeAttr(AOSTAG_OBJID, 0, true);
	doc->removeAttr(AOSTAG_DOCID, 0, true);
	doc->removeAttr(AOSTAG_CT_EPOCH, 0, true);
	doc->removeAttr(AOSTAG_CTIME, 0, true);
	doc->removeAttr(AOSTAG_CREATOR, 0, true);
	doc->removeAttr(AOSTAG_MTIME, 0, true);
	doc->removeAttr(AOSTAG_MT_EPOCH, 0, true);
	doc->removeAttr(AOSTAG_PUB_CONTAINER, 0, true);
	doc->removeAttr(AOSTAG_SITEID, 0, true);

	OmnString name = def->getAttrStr(AOSTAG_NAME, "");
	if (name == "") name = avalue;

	doc->setAttr(AOSTAG_NAME, name);
}
