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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataset/DatasetFile.h"

#include "AosConf/DataSet.h"
#include "AosConf/DataFieldExpr.h"
#include "AosConf/DataRecordFixbin.h"
#include "DataProc/DataProc.h"
#include "JSON/JSON.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDatasetFile_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDatasetFile(version);
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


AosDatasetFile::AosDatasetFile(const int version)
:
AosJimoDataset(version, AosJimoType::eDatasetFile)
{
}


AosDatasetFile::~AosDatasetFile()
{
}


bool
AosDatasetFile::config(
		AosRundata *rdata,
		const OmnString &jsonstr,
		const OmnString &dsname,
		const AosJimoProgObjPtr &prog)
{
	//create dataset myds {
	//	type:file,
	//	files: 
	//		(
	//			dirname:/home1/rain, 
	//			serverid:0
	//	    )	
	//	split:{type:file, block_size:100000}
	//	schema: {
	//		type:csv,
	//		charset:UTF8,
	//		record_delimiter:LF,
	//		field_delimiter:SEM,
	//		text_qualifier:DQM,
	//		fileds: [
	//			{
	//				type:str,
	//				name:f1,
	//				max_length:10
	//			},
	//			{
	//				type:int,
	//				name:f2,
	//				max_length:15
	//			},
	//			{
	//				type:docid_allocater,
	//				name:docid
	//			}			
	//		]
	//	    }
	//	}
	
	try
	{
		JSONValue jsonDataset;
		JSONReader reader;
		bool rslt = reader.parse(jsonstr, jsonDataset);
		aos_assert_r(rslt, false);

		AosConf::DataSet ds;
		ds.setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
		ds.setAttribute("zky_otype", "dataset");
		ds.setAttribute("zky_name", dsname);

		//datascanner
		boost::shared_ptr<AosConf::DataScanner> dscan = boost::make_shared<AosConf::DataScanner>();
		dscan->setAttribute("jimo_objid", "datascanner_parallel_jimodoc_v0");
		dscan->setAttribute("zky_name", dsname);

		//dataconector
		boost::shared_ptr<AosConf::DataConnector> dc = boost::make_shared<AosConf::DataConnector>();
		dc->setAttribute("jimo_objid", "dataconnector_file_jimodoc_v0");
		string datasetType = jsonDataset["type"].asString();
		dc->setAttribute("type", datasetType);
		// jimodb-906
		string skip_first_line = jsonDataset["skip_first_line"].asString();
		dc->setAttribute("skip_first_line", skip_first_line);

		//must be file
		JSONValue file = jsonDataset["files"];

		string value = file["filename"].asString();
		aos_assert_r(value != "", false);
		u32 phyid = file["serverid"].asUInt();
		
		JSONValue schema = jsonDataset["schema"];
		OmnString charSetValue = schema["charset"].asString();

		OmnString file_conf = "";
		file_conf << "<file zky_character=\"" << charSetValue << "\"" <<  " zky_physicalid=\"" << phyid << "\">";
		file_conf << "<![CDATA[" << value << "]]>";
		file_conf << "</file>"; 
		dc->setFile(file_conf);

		
		dc->setAttribute("zky_character", charSetValue);
		dc->setAttribute("zky_name", dsname);
		//dc->setAttribute("skip_first_line", schema["skip_first_line"].asString("false")); // Add by Young,2015/04/07

		//split
		boost::shared_ptr<AosConf::DataSplit> dsp = boost::make_shared<AosConf::DataSplit>();
		JSONValue split = jsonDataset["split"];
		u32 block_size = split["block_size"].asUInt();
		dsp->setAttribute("group_size", block_size);

		dsp->setAttribute("jimo_objid", "dataspliter_file_jimodoc_v0");
		dc->setSplit(dsp);

		dscan->setConnector(dc);


		//dataschema
		boost::shared_ptr<AosConf::DataSchema> dschema = boost::make_shared<AosConf::DataSchema>();
		dschema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
		dschema->setAttribute("zky_dataschema_type", "static");
		dschema->setAttribute("zky_name", dsname);

		//datarecord
		boost::shared_ptr<AosConf::DataRecord> dr;
		OmnString recordType = schema["type"].asString("csv");
		if ( recordType.toLower() == "csv")
		{
			dr = boost::make_shared<AosConf::DataRecord>();
			dr->setAttribute("zky_name", dsname);
			dr->setAttribute("type", schema["type"].asString());
			dr->setAttribute("zky_field_delimiter", schema["zky_field_delimiter"].asString());
			dr->setAttribute("zky_row_delimiter", schema["zky_row_delimiter"].asString());
			dr->setAttribute("zky_text_qualifier", schema["zky_text_qualifier"].asString());
			dr->setAttribute(AOSTAG_DOCID, schema["zky_docid"].asString());
		}
		else if (recordType.toLower() == "fixbin")
		{
			dr = boost::make_shared<AosConf::DataRecordFixbin>();
			dr->setAttribute("zky_name", dsname);
			dr->setAttribute("type", schema["type"].asString());
			dr->setAttribute("zky_length", schema["zky_length"].asInt());
			dr->setAttribute(AOSTAG_DOCID, schema["zky_docid"].asString());
		}
		else 
		{
			OmnAlarm << "record type is invalid" << enderr;
			return false;
		}
		dr->setAttribute("trim", schema["trim"].asString());

		//datafields
		JSONValue fields = schema["fields"];
		for (u32 i = 0; i < fields.size(); i++)
		{
			boost::shared_ptr<AosConf::DataFieldExpr> df = boost::make_shared<AosConf::DataFieldExpr>();
			value = fields[i]["type"].asString();
			aos_assert_r(value != "", false);
			df->setAttribute("type", value);

			// Add by Young, 2015/04/13
			// if field is expr 
			if(value == "number")
			{
				int zky_precision_len = fields[i]["zky_precision"].asInt();
				df->setAttribute("zky_precision", zky_precision_len);
				int zky_scale_len = fields[i]["zky_scale"].asInt();
				df->setAttribute("zky_scale", zky_scale_len);
			}

			value = fields[i]["value"].asString();
			if (value != "")
			{
				df->setExpr(value);
			}
			u32 fieldLen = fields[i]["max_length"].asUInt();
			if (fieldLen > 0) df->setAttribute("zky_length", fieldLen);
			df->setAttribute("zky_otype", "datafield");
			value = fields[i]["name"].asString();
			df->setAttribute("zky_name", value);
			value = fields[i]["zky_metadata_attrname"].asString();
			df->setAttribute("zky_metadata_attrname", value);
			df->setAttribute("format", fields[i]["format"].asString());

			dr->setField(df);
		} 
		dschema->setRecord(dr);

		ds.setScanner(dscan);
		ds.setSchema(dschema);

		ds.setAttribute(AOSTAG_CTNR_PUBLIC, "true");
		ds.setAttribute(AOSTAG_PUBLIC_DOC, "true");
		//2015/07/20  arvin
		//versionthirdone-420 : datasetfile objid isnot the same
		ds.setAttribute(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, dsname));

		string ds_str = ds.getConfig();
		cout << "66666666666666666666666666666" << endl;
		cout << ds_str << endl;

		//return AosCreateDoc(ds_str, true, rdata);
		OmnString objid =  AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, dsname);
		prog->saveLogicDoc(rdata, objid, ds_str);
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
	return false;
}


AosJimoPtr 
AosDatasetFile::cloneJimo() const
{
	try 
	{
		return OmnNew AosDatasetFile(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}

	return NULL;
}


bool
AosDatasetFile::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	return config(rdata, jsonstr, obj_name, prog);
}
