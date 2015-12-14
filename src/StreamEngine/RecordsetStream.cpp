////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/RecordsetStream.h"

#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"  
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "TaskMgr/Task.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "StreamEngine/Service.h"
#include "StreamEngine/StreamDataProc.h"
#include "JimoDataProc/JimoDataProc.h"
#include <boost/make_shared.hpp> 

static map<OmnString, RecordsetList> sgRecordsetMap;
static bool sgDebug = false;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosRecordsetStream_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosRecordsetStream(version);
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


//
//constructor/destructors/clone
//
AosRecordsetStream::AosRecordsetStream(int idx)
:
AosGenericObj(idx)
{
}

AosRecordsetStream::AosRecordsetStream(const AosRecordsetStream &stream)
:
AosGenericObj(0)
{
}

AosRecordsetStream::~AosRecordsetStream()
{
}

AosJimoPtr
AosRecordsetStream::clone()                          
{
	return this;
}

AosJimoPtr
AosRecordsetStream::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &)
{
	return NULL;
}

AosJimoPtr
AosRecordsetStream::cloneJimo() const
{
	return OmnNew AosRecordsetStream(*this);
}


//////////////////////////////////////////////////////////////////
//     JQL methods
//////////////////////////////////////////////////////////////////

//
//create recordsetstream ds_t1 {
//		"type"  : "recordsetstream",
//			"fields" : {
//					 "k1" : 	{ "type" : "str" ,  "cstr" : "cstr",  "offset" : "0"  , "maxlen" : "10" },
//		 			 "k2" : 	{ "type" : "str" ,  "cstr" : "cstr",  "offset" : "0"  , "maxlen" : "10" },
//	 			 	 "k3" : 	{ "type" : "str" ,  "cstr" : "cstr",  "offset" : "0"  , "maxlen" : "10" },
//					 "d1" : 	{ "type" : "str" ,  "cstr" : "cstr",  "offset" : "0"  , "maxlen" : "10" },
//					 "v1" : 	{ "type" : "str" ,  "cstr" : "cstr",  "offset" : "0"  , "maxlen" : "10" },
//				}
//  };
//							 			 			 			 		     		      
bool 
AosRecordsetStream::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//save above json into a doc
	//AosJimoScheduler scheduler;
	try
	{
		JSONReader reader;
		JSONValue jsonObj;
		reader.parse(jsonstr, jsonObj);

		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, obj_name);
		OmnString dp_str = "";
		dp_str 
			<< "<recordsetstream " << AOSTAG_CTNR_PUBLIC << "=\"true\" "
			<<      AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\">"
			<<          "<![CDATA[" << jsonstr << "]]>"
			<< "</recordsetstream>";

		return AosCreateDoc(dp_str, true, rdata);
	}
	catch (...)
	{
		OmnScreen << "JSONException... " << endl;
	}

	return true;
}

bool 
AosRecordsetStream::showByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	map<OmnString, RecordsetList>::iterator recItr;
	OmnString recName, msg;
	RecordsetList recList;
	if(objName == "all" || objName == "ALL")
	{
		for(recItr = sgRecordsetMap.begin(); recItr != sgRecordsetMap.end(); recItr++)
		{
			recName = recItr->first;
			recList = sgRecordsetMap[recName];
			//out put 
			msg << "\n"
				<< "      " << recName << " :" << recList.size() << " recordset" <<"\n";
		}
		rdata->setJqlMsg(msg);
	}
	else
	{
		//display content of recordset named by objName
		map<OmnString, RecordsetList>::iterator itr;
		itr = sgRecordsetMap.find(objName);
		AosRecordset* rs;
		AosDataRecordObj *rcd;
		AosDataRecord* dataRcd;
		OmnString fType, fLength, fName, str, fValue;
		if(itr != sgRecordsetMap.end())
		{
			RecordsetList rdList = sgRecordsetMap[objName];
			for(u32 i=0; i<rdList.size(); i++)
			{
				rs = dynamic_cast<AosRecordset *>(rdList[i].getPtr());
				rs->resetReadIdx();
				for(int j=0; j<rs->size(); j++)
				{
					rs->nextRecord(rdata, rcd);
					aos_assert_r(rcd, false);
					
					dataRcd = dynamic_cast<AosDataRecord *>(rcd);
					vector<AosDataFieldObjPtr> fields = dataRcd->getFields();

					OmnString rdName = dataRcd->getRecordName();
					
					if(j == 0)
					{
						str << "\n" 
							<< rdName << " : " << "\n"
							<< "      {" << "\n";
					}
					str << "            ";
					for(u32 k=0; k<fields.size(); k++)
					{
						AosDataField* field = dynamic_cast<AosDataField *>(fields[k].getPtr());
						aos_assert_r(field, false);
						fName = field->getName();
						fValue = dataRcd->getFieldValue(rdata, fName);
						str << fName << ": " << fValue << ", ";
					}
					str << "\n"; 
				}
				str << "      }";
				rdata->setJqlMsg(str);
			}
		}
	}

	return true;
}

//
//run recordsetstream ds_t1 {
//		"data": [
//					[  "k1_1",  "k2_2",  "k3_10",   "19320",   "234" ],
//					[  "k1_1",  "k2_2",  "k3_10",   "19320",   "234" ],
//					[  "k1_1",  "k2_2",  "k3_10",   "19320",   "234" ],
//					[  "k1_1",  "k2_2",  "k3_10",   "19320",   "234" ],
//					[  "k1_1",  "k2_2",  "k3_10",   "19320",   "234" ],
//				]
// };
//
bool 
AosRecordsetStream::runByJql(
		AosRundata *rdata,
		const OmnString &objName, 
		const OmnString &jsonstr)
{
	//load recordset schema information from the doc created
	JSONReader reader;

	JSONValue json;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);
	FastWriter writer;                                                                                                                                                  
	//get running parameters
	JSONValue jsonData;
	try {
		//save running parameters into a map
		jsonData = json["data"];
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
	OmnString svr_name = json["service_name"].asString();
	
	//get the service job configuration
	OmnString svr_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objName);
	AosXmlTagPtr doc = AosGetDocByObjid(svr_objid, rdata);
	aos_assert_r(doc, false);

	//config the service
	JSONValue jsonConf;
	OmnString confStr = doc->getNodeText();  
	rslt = reader.parse(confStr, jsonConf);
	aos_assert_r(rslt, false);
	
	//JSONValue fields_json = jsonConf["fields"];
	//aos_assert_r(!fields_json.empty(), false);

	OmnString schema_str = jsonConf["schema"].asString();

	//create a data record
	AosDataRecordObjPtr recordContainer;
	AosRecordsetObjPtr rsPtr;
	vector<OmnString> fields;
	rslt = defineRecord(rdata, schema_str, objName, fields, recordContainer);
	//rsPtr = AosRecordsetObj::createRecordsetStatic(rdata, recordContainer);    
	OmnString str = "<recordset />";
	AosXmlParser parser;
	AosXmlTagPtr rcdstXml = parser.parse(str, "" AosMemoryCheckerArgs); 
	rsPtr = AosRecordsetObj::createRecordsetStatic(rdata, rcdstXml);    
	rsPtr->holdRecord(rdata, recordContainer);

	//save data into recordset buff
	AosBuffDataPtr metaData = NULL;
	JSONValue jsonRecord;
	//Value::Members members;
	OmnString name;
	AosValueRslt valst;
	AosDataFieldObj * data_field;
	OmnString field_type;
	OmnString data = "";

	for (u32 i=0; i<jsonData.size(); i++)
	{
		recordContainer->clear();
		//set values to the record
		//add data fields to dr
		JSONValue field;
		jsonRecord = jsonData[i];
		//members = jsonRecord.getMemberNames();                         
		for (u32 j = 0; j < fields.size(); j++)
		{
			name = fields[j];
			data_field = recordContainer->getDataField(rdata, name);
			field_type = data_field->getTypeName();

			if(field_type == "expr")
				continue;
			if(j > 0 && mDelimiter=="CRLF")
				data << ",";
			data << OmnString(jsonRecord[name].asString());
		}
		data << "\r\n";

		//rslt = rsPtr->appendRecord(rdata, recordContainer, metaData);
		//aos_assert_r(rslt, false);
	}
	//register the recordset to StreamDataProc's recordset map
	/*
	for (u32 i=0; i<jsonData.size(); i++)
	{
		if(i>0)
			data << "\r\n";
		data << jsonData[i].toStyledString();
	}
	*/
	AosRundataPtr rdataPtr = rdata;

	///////////////////////////////////////////////////////////////////
	//
	//			new way to set recordset
	//
	///////////////////////////////////////////////////////////////////
	const bool flag = true;
	char *Data = data.getBuffer();
	const i64 len = data.length();
	AosBuffPtr buff = OmnNew AosBuff(Data, len, len, flag AosMemoryCheckerArgs);
	AosBuffDataPtr buffdata = OmnNew AosBuffData();
	buffdata->setBuff(buff);

	rsPtr->reset();
	rsPtr->setData(rdata, buffdata->getBuff());
	//rsPtr->setMetadata(rdata, buffdata->getMetaData());

	int64_t offset = buffdata->getCrtIdx();
	int64_t data_len = buffdata->getDataLen();
	aos_assert_r(offset >=0, false);

	int record_len = 0;
	int status = 0;
	while(offset < data_len)
	{
		recordContainer->determineRecordLen(&data[offset], data_len - offset, record_len, status);
		const char * cData = data.data();
		OmnString dataRecord = "";
		for(int i= 0; i<record_len; i++)
		{
			dataRecord << cData[offset+i];
		}
		OmnScreen << "data_record : " << dataRecord << endl;

		if(record_len == -1)
		{
			OmnAlarm << "never come here" << enderr;
			break;
		}

		if(status == -2)
		{
			OmnScreen << "invalid record" << endl;

			//offset += record_len;
			//continue;
		}

		AosMetaDataPtr metaData = buffdata->getMetadata();
		rsPtr->appendRecord(rdata, 0, offset, record_len, metaData);

		//print by levi
		//char *curData = rsPtr->getData();
		//OmnScreen << "recordsetstream curData : " << curData << endl;
		offset += record_len;

		aos_assert_r(offset <= data_len, rdata);
	}
	
	//use stream data to distribute external data
	AosStreamDataPtr sData = AosStreamData::getStreamData(rdata, objName, svr_name);
	sData->feedStreamData(rdataPtr, rsPtr);

	return true;
}

/////////////////////////////////////////////////////////////////////
//    Data methods
/////////////////////////////////////////////////////////////////////

//
//generate a data field and insert into the data record
//
bool
AosRecordsetStream::addDataField(
								 AosConf::DataRecordFixbin &dr,
								 const OmnString &name,
								 const OmnString &type,
								 const OmnString &shortplc,
								 const u32 offset,
								 const u32 len)
{
	boost::shared_ptr<AosConf::DataField> df = boost::make_shared<AosConf::DataField>();

	df = boost::make_shared<AosConf::DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);

	if (shortplc != "")
		df->setAttribute("zky_datatooshortplc", shortplc);
	df->setAttribute("zky_offset", offset);
	df->setAttribute("zky_length", len);

	dr.setField(df);
	return true;
}


//
//generate xml string for statkey index record
//
bool
AosRecordsetStream::defineRecord(AosRundata *rdata,
								 const OmnString &schemaName, 
								 const OmnString &rcdName,
								 vector<OmnString> &fields,
								 AosDataRecordObjPtr &rcdContainer)
{
	//OmnString str = "";
	//OmnString rcdName = "";

	//set data record properties
	aos_assert_r(schemaName != "", false);
	AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, schemaName);
	aos_assert_r(doc, false);
	
	const AosTaskObjPtr task;
	OmnString name;
	AosDataRecordObjPtr rcd = AosDataRecordObj::createDataRecordStatic(doc, task, rdata AosMemoryCheckerArgs);
	AosXmlTagPtr rcdXml = rcd->getRecordDoc();

	//get mDelimiter "CRLF"
	mDelimiter = rcdXml->getAttrStr("zky_row_delimiter");

	// add field name to vector
	AosXmlTagPtr dataFieldsXml = doc->getFirstChild("datafields");
	aos_assert_r(dataFieldsXml, false);
	AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();
	aos_assert_r(dataFieldXml, false);

	while (dataFieldXml)
	{
		name = dataFieldXml->getAttrStr("zky_name");
		fields.push_back(name);      
		dataFieldXml = dataFieldsXml->getNextChild();
	}   
	//create reocrd
	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(rcdXml, 0, rdata AosMemoryCheckerArgs);
	
	rcdContainer = record;
	/*OmnString strContainer = "";
	strContainer << "<datarecord type=\"ctnr\" zky_name=\"" << rcdName << "\">"; 
	strContainer << rcdXml->toString(); 
	strContainer << "</datarecord>"; 

	AosXmlParser parser;
	AosXmlTagPtr xmlContainer = parser.parse(strContainer.data(), "" AosMemoryCheckerArgs); 
	rcdContainer = AosDataRecordObj::createDataRecordStatic(xmlContainer, task, rdata AosMemoryCheckerArgs); 

	   OmnString type_str = "";
	   AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(rcdName, type);

	//add data fields to dr
	JSONValue fields;
	Value::Members members = fields.getMemberNames();                         
	JSONValue field;
	std::string name;
	AosDataFieldType::E field_type;

	mDocidField = "";
	for (u32 i = 0; i<fields.size(); i++)
	{
		name = members[i];
		field = fields[name];
		field_type = convertToDataFieldType(rdata, field["type"].asString());
		output->setField(name, field_type, OmnString(field["maxlen"].asString()).toInt64(0));
	}

	//create a data record
	AosTaskObjPtr task = NULL;
	output->init(task, rdata);
	rcd = output->getRecord();
	*/
	//create docid provider
	/*if (mDocidField != "")
	{
		mDocidProvider = OmnNew AosDocidProvider(rdata, task, record_type_key, rcd->getRecordDoc(), doc_size);
	}
	*/
	return true;
}

//////////////////////////////////////////////////////////////////
//     help methods
//////////////////////////////////////////////////////////////////
bool
AosRecordsetStream::isDebug()
{
	return sgDebug;
}

bool
AosRecordsetStream::addDebugData(
		OmnString rsName,
		AosRecordsetObjPtr rsPtr)
{
	aos_assert_r(rsPtr, false);

	//if debugging mode, append the recordset to the global 
	//recordset map
	vector<AosRecordsetObjPtr> *rsList;
	vector<AosRecordsetObjPtr> emptyList;
	map<OmnString, RecordsetList>::iterator rsItr;
	if (sgDebug)
	{
		rsItr = sgRecordsetMap.find(rsName);
		if (rsItr != sgRecordsetMap.end())
		{
			rsList = &(rsItr->second);
		}
		else
		{
			sgRecordsetMap[rsName] = emptyList;
			rsList = &(sgRecordsetMap[rsName]);
		}

		rsList->push_back(rsPtr);
	}

	return true;
}

bool
AosRecordsetStream::feedByJql(
		AosRundata* rdata,
		const OmnString &objName,
		const OmnString &jsonstr)
{
	//load recordset schema information from the doc created
	JSONReader reader;

	JSONValue json;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);
	FastWriter writer;                                                                                                                                                  
	//get running parameters
	JSONValue jsonData;
	try {
		//save running parameters into a map
		jsonData = json["data"];
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
	OmnString svr_name = json["service_name"].asString();

	
	//get the service job configuration
	OmnString svr_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objName);
	AosXmlTagPtr doc = AosGetDocByObjid(svr_objid, rdata);
	aos_assert_r(doc, false);

	//config the service
	JSONValue jsonConf;
	OmnString confStr = doc->getNodeText();  
	rslt = reader.parse(confStr, jsonConf);
	aos_assert_r(rslt, false);
	
	//JSONValue fields_json = jsonConf["fields"];
	//aos_assert_r(!fields_json.empty(), false);

	OmnString schema_str = jsonConf["schema"].asString();

	//create a data record
	AosDataRecordObjPtr recordContainer;
	AosRecordsetObjPtr rsPtr;
	vector<OmnString> fields;
	rslt = defineRecord(rdata, schema_str, objName, fields, recordContainer);
	//rsPtr = AosRecordsetObj::createRecordsetStatic(rdata, recordContainer);    
	OmnString str = "<recordset />";
	AosXmlParser parser;
	AosXmlTagPtr rcdstXml = parser.parse(str, "" AosMemoryCheckerArgs); 
	rsPtr = AosRecordsetObj::createRecordsetStatic(rdata, rcdstXml);    
	rsPtr->holdRecord(rdata, recordContainer);

	//save data into recordset buff
	AosBuffDataPtr metaData = NULL;
	JSONValue jsonRecord;
	//Value::Members members;
	OmnString name;
	AosValueRslt valst;
	AosDataFieldObj * data_field;
	OmnString field_type;
	OmnString data = "";

	for (u32 i=0; i<jsonData.size(); i++)
	{
		recordContainer->clear();
		//set values to the record
		//add data fields to dr
		JSONValue field;
		jsonRecord = jsonData[i];
		//members = jsonRecord.getMemberNames();                         
		for (u32 j = 0; j < fields.size(); j++)
		{
			name = fields[j];
			data_field = recordContainer->getDataField(rdata, name);
			field_type = data_field->getTypeName();

			if(field_type == "expr")
				continue;
			if(j > 0 && mDelimiter=="CRLF")
				data << ",";
			data << OmnString(jsonRecord[name].asString());
		}
		data << "\r\n";

		//rslt = rsPtr->appendRecord(rdata, recordContainer, metaData);
		//aos_assert_r(rslt, false);
	}
	//register the recordset to StreamDataProc's recordset map
	/*
	for (u32 i=0; i<jsonData.size(); i++)
	{
		if(i>0)
			data << "\r\n";
		data << jsonData[i].toStyledString();
	}
	*/
	AosRundataPtr rdataPtr = rdata;

	///////////////////////////////////////////////////////////////////
	//
	//			new way to set recordset
	//
	///////////////////////////////////////////////////////////////////
	const bool flag = true;
	char *Data = data.getBuffer();
	const i64 len = data.length();
	AosBuffPtr buff = OmnNew AosBuff(Data, len, len, flag AosMemoryCheckerArgs);
	AosBuffDataPtr buffdata = OmnNew AosBuffData();
	buffdata->setBuff(buff);

	rsPtr->reset();
	rsPtr->setData(rdata, buffdata->getBuff());
	//rsPtr->setMetadata(rdata, buffdata->getMetaData());

	int64_t offset = buffdata->getCrtIdx();
	int64_t data_len = buffdata->getDataLen();
	aos_assert_r(offset >=0, false);

	int record_len = 0;
	int status = 0;
	while(offset < data_len)
	{
		recordContainer->determineRecordLen(&data[offset], data_len - offset, record_len, status);
		const char * cData = data.data();
		OmnString dataRecord = "";
		for(int i= 0; i<record_len; i++)
		{
			dataRecord << cData[offset+i];
		}
		OmnScreen << "data_record : " << dataRecord << endl;

		if(record_len == -1)
		{
			OmnAlarm << "never come here" << enderr;
			break;
		}

		if(status == -2)
		{
			OmnScreen << "invalid record" << endl;

			//offset += record_len;
			//continue;
		}

		AosMetaDataPtr metaData = buffdata->getMetadata();
		rsPtr->appendRecord(rdata, 0, offset, record_len, metaData);

		//print by levi
		//char *curData = rsPtr->getData();
		//OmnScreen << "recordsetstream curData : " << curData << endl;
		offset += record_len;

		aos_assert_r(offset <= data_len, rdata);
	}
	
	//use stream data to distribute external data
	AosStreamDataPtr sData = AosStreamData::getStreamData(rdata, objName, svr_name);
	sData->feedStreamData(rdataPtr, rsPtr);

	return true;
}


AosDataFieldType::E                                                                           
AosRecordsetStream::convertToDataFieldType(
		AosRundata* rdata,
		const OmnString &str_type)
{
	AosDataFieldType::E type = AosDataFieldType::toEnum(str_type);
	aos_assert_r(type != AosDataFieldType::eInvalid, AosDataFieldType::eInvalid);
	switch(type)
	{
		case AosDataFieldType::eStr:
			break;
		case AosDataFieldType::eBinU64:
			break;
		case AosDataFieldType::eBinDouble:
			break;
		case AosDataFieldType::eU64:
			type = AosDataFieldType::eBinU64;
			break;
		case AosDataFieldType::eDouble:
		case AosDataFieldType::eNumber:
			type = AosDataFieldType::eBinDouble;
			break;
		case AosDataFieldType::eDateTime:
			type = AosDataFieldType::eBinDateTime;
			break;
		case AosDataFieldType::eInt64:
			type = AosDataFieldType::eBinInt64;
			break;
		default:
			OmnAlarm << "not handle this data field type: " << str_type << enderr;
			break;
	}

	return type;
}
