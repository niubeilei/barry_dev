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
#include "JimoDataset/DatasetIIL.h"

#include "AosConf/DataRecordFixbin.h"
#include "AosConf/DataSet.h"
#include "AosConf/DataFieldWithSubkeys.h"
#include "AosConf/DataConnectorIIL.h"
#include "DataProc/DataProc.h"
#include "SEUtil/JqlTypes.h"
#include "JSON/JSON.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDatasetIIL_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDatasetIIL(version);
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


AosDatasetIIL::AosDatasetIIL(const int version)
:
AosJimoDataset(version, AosJimoType::eDatasetIIL)
{
}


AosDatasetIIL::~AosDatasetIIL()
{
}


bool
AosDatasetIIL::config(
		const OmnString &jsonstr,
		const OmnString &dsname,
		const AosRundataPtr &rdata,
		const AosJimoProgObjPtr &prog)
{
	//create dataset myds {
	//	type:dir,
	//	dirs: [
	//		{dirname:/home1/young, serverid:0},
	//		{dirname:/home1/young, serverid:0}
	//	     ]	
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
	
	JSONValue jsonDataset;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, jsonDataset);
	aos_assert_r(rslt, false);

	AosConf::DataSet ds;
	ds.setAttribute("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
	ds.setAttribute("zky_otype", "dataset");
	ds.setAttribute("zky_name", dsname);


	// 1. datascanner
	boost::shared_ptr<AosConf::DataScanner> dscan = boost::make_shared<AosConf::DataScanner>();
	dscan->setAttribute("jimo_objid", "datascanner_cube_jimodoc_v0");
	dscan->setAttribute("zky_name", dsname);

	// dataconnector
	boost::shared_ptr<AosConf::DataConnectorIIL> dc = boost::make_shared<AosConf::DataConnectorIIL>();
	dc->setAttribute("jimo_objid", "dataconnector_iil_jimodoc_v0");
	dc->setAttribute("zky_iilname", jsonDataset["iilname"].asString());

	// split
	try 
	{
		int block_size = jsonDataset["split"]["block_size"].asInt();
		if (block_size > 0)
		{
			boost::shared_ptr<AosConf::DataSplit> dsp = boost::make_shared<AosConf::DataSplit>();
			dc->setAttribute("zky_blocksize", block_size);
			dsp->setAttribute("jimo_objid", "datasplit_iilcubegroup_jimodoc_v0");
			dc->setSplit(dsp);
		}
	}
	catch (std::exception &e)
	{
		OmnScreen << "JSONException: " << e.what() << jsonstr << endl;
	}

	// query cond
	bool conditions = jsonDataset["conditions"].asBool();
	if (conditions)
	{
		dc->setQueryCond("an", "*");	
	}

	dscan->setConnector(dc);
	ds.setScanner(dscan);

	// 2. dataschema 
	JSONValue schema = jsonDataset["schema"];
	boost::shared_ptr<AosConf::DataSchema> dschema = boost::make_shared<AosConf::DataSchema>();
	dschema->setAttribute("jimo_objid", "dataschema_record_jimodoc_v0");
	dschema->setAttribute("zky_name", dsname);

	//datarecord
	boost::shared_ptr<AosConf::DataRecord> dr = boost::make_shared<AosConf::DataRecordFixbin>();
	dr->setAttribute("zky_name", dsname);
	dr->setAttribute("type", schema["type"].asString());

	//datafields
	JSONValue fields = schema["fields"];
	for (u32 i = 0; i < fields.size(); i++)
	{
		boost::shared_ptr<AosConf::DataFieldWithSubkeys> df = boost::make_shared<AosConf::DataFieldWithSubkeys>();
		string type = fields[i]["type"].asString();
		aos_assert_r(type != "", false);
		df->setAttribute("type", type);

		string name = fields[i]["name"].asString();
		aos_assert_r(name != "", false);
		df->setAttribute("zky_name", name);
		
		u32 len = fields[i]["max_length"].asUInt();
		if (len > 0) df->setAttribute("zky_length", len);
		df->setAttribute("zky_otype", "datafield");

	 	if (fields[i].isMember("sub_fields"))
		{
			JSONValue sub_fields = fields[i]["sub_fields"];	
			for (size_t j=0; j<sub_fields.size(); j++)
			{
				string sub_field_name = sub_fields[j].asString();
				AosXmlTagPtr doc = AosJqlStatement::getDoc(
						rdata, JQLTypes::eDataFieldDoc, sub_field_name);
				aos_assert_r(doc, false);
				df->setSubkeyDef(doc->toString());
			}
		}
		dr->setField(df);
	} 

	dschema->setRecord(dr);
	ds.setSchema(dschema);

	string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDatasetDoc, dsname);
	ds.setAttribute(AOSTAG_CTNR_PUBLIC, "true");
	ds.setAttribute(AOSTAG_PUBLIC_DOC, "true");
	ds.setAttribute(AOSTAG_OBJID, objid);
	string ds_str = ds.getConfig();
	cout << "66666666666666666666666666666" << endl;
	cout << ds_str << endl;

	//return AosCreateDoc(ds_str, true, rdata);
	prog->saveLogicDoc(rdata.getPtr(), objid, ds_str);
	return true;
}


AosJimoPtr 
AosDatasetIIL::cloneJimo() const
{
	try 
	{
		return OmnNew AosDatasetIIL(*this);
	}
	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}

	return NULL;
}


bool
AosDatasetIIL::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	return config(jsonstr, obj_name, rdata, prog);
}
