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
#include "JimoLogic/JimoLogicDoc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"
#include "UtilTime/TimeFormat.h" 
#include "AosConf/DataSet.h" 
#include "AosConf/ReduceTaskDataProc.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/DataProcConvertdoc.h"
#include "AosConf/DataRecordCtnr.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using namespace AosConf;
using boost::make_shared;
using boost::shared_ptr;

AosJLDoc::AosJLDoc(
		const boost::shared_ptr<DataSet> &inputds, 
		list<string> &fields, 
		const string &tablename,
		map<OmnString, AosXmlTagPtr> &table_fileds,
		JQLTypes::OpType op)
:AosJimoLogic(1)
{
	//input virtual fields
	boost::shared_ptr<DataRecord> input_dr = inputds->getSchema()->getRecord();
	vector<boost::shared_ptr<DataField> > &rf = input_dr->getFields();		
	for (size_t i=0; i<rf.size(); i++)
	{
		OmnString fname = rf[i]->getAttribute("zky_name");	
		AosXmlTagPtr t_filed = table_fileds[fname];
		//aos_assert(t_filed);
		if(!t_filed) continue;
		OmnString type = t_filed->getAttrStr("datatype", "");
		if (type != "varchar")
		{
			OmnString nfname = AosDataType::getFieldName(fname, type);
			if (type == "date")
			{
				boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();
				docidFieldObj->setAttribute("zky_name", nfname);
				docidFieldObj->setAttribute("type", "time");
				docidFieldObj->setAttribute("zky_value_from_field", fname);
				OmnString ofmt = t_filed->getAttrStr("format", AOSTIMEFORMAT_YYYY_MM_DDHHMMSS);
				docidFieldObj->setAttribute("zky_origtmfmt", ofmt);
				OmnString tfmt = t_filed->getAttrStr("", AOSTIMEFORMAT_YYYY_MM_DDHHMMSS);
				docidFieldObj->setAttribute("zky_targettmfmt", tfmt);
				mInputFields.push_back(docidFieldObj);
			}

		}
	}
	
	const boost::shared_ptr<DataSet> outputs = inputds->clone();
	boost::shared_ptr<DataRecord> datarecord = outputs->getSchema()->getRecord();
	OmnString recordtype = datarecord->getAttribute("type");
	string recordname = datarecord->getAttribute("zky_name");

	//output dataset
	const boost::shared_ptr<DataSet> outputds = boost::make_shared<DataSet>();
	outputds->setAttribute("zky_name", tablename);
	mOutputds.push_back(outputds);


	//jackie
	if(op == JQLTypes::eDrop)
	{
		//maptask dataproc
		boost::shared_ptr<AosConf::DataProcCompose> dp = boost::make_shared<DataProcCompose>();
			
		string srcname = recordname + ".zky_docid";
		string dstname = recordname + "_new.zky_docid";
		dp->setInput(srcname);
		dp->setOutput(dstname);
		mMapDataProcs.push_back(dp);		

		//redcue task dataproc
		boost::shared_ptr<DataRecord> datarecord = boost::make_shared<DataRecord>();

		OmnString asmtype = "stradd";
		OmnString dataproctype = "delete_groupdoc";

		boost::shared_ptr<ReduceTaskDataProc> reduceproc = boost::make_shared<ReduceTaskDataProc>(dataproctype);
		boost::shared_ptr<DataRecordCtnr> recordCtnrObj = boost::make_shared<DataRecordCtnr>();

		boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();
		docidFieldObj->setAttribute("zky_name", "zky_docid");
		docidFieldObj->setAttribute("type", "bin_u64");
		docidFieldObj->setAttribute("zky_length", "8");
		docidFieldObj->setAttribute("zky_offset", "0");
		datarecord->setField(docidFieldObj);

		OmnString recordCtnrObjName = "";
		recordCtnrObjName << recordname<< "_new";
		datarecord->setAttribute("zky_name", recordCtnrObjName);
		datarecord->setAttribute("type", "fixbin");
		datarecord->setAttribute("zky_length", "8");
		recordCtnrObj->setRecord(datarecord);
		recordCtnrObj->setAttribute("zky_name", recordCtnrObjName);

		reduceproc->setRecord(recordCtnrObj);
		reduceproc->setAttribute("datasetname", tablename);
		reduceproc->setAttribute("type", dataproctype);

		string dcltype = "iil";
		reduceproc->setCollectorType(dcltype);

		reduceproc->setCollectorAttr("zky_name", tablename);
		reduceproc->setAssemblerType(asmtype);

		mReduceDataProcs.push_back(reduceproc);

	}
	else
	{
		//load

		//maptask dataproc
		boost::shared_ptr<AosConf::DataProcConvertdoc> dp = boost::make_shared<DataProcConvertdoc>();
			
		for (list<string>::iterator itr = fields.begin();
				itr != fields.end(); ++itr)
		{
			string srcname = recordname + "." +*itr;
			string dstname = recordname + "_new." + *itr;
			dp->setIOName(srcname, dstname);
			//dp->setField(srcname, dstname);
		}
		mMapDataProcs.push_back(dp);		


		//redcue task dataproc
		vector<boost::shared_ptr<DataField> > &v = datarecord->getFields();		

		list<string>::iterator f = fields.begin();
		for(size_t i=0; i<v.size(); i++) {
			if (f != fields.end()) {
				if (v[i]->getAttribute("zky_name") != *f)
				{
					v.erase(v.begin()+i);	
					i--;
				}
				f++;
			}
		}

		OmnString asmtype = "docnorm";
		OmnString dataproctype = "importdoc_fixed";
		if (recordtype == "csv")
		{
			asmtype = "doccsv";
			dataproctype = "importdoc_csv";
		}

		aos_assert(dataproctype != "");

		boost::shared_ptr<ReduceTaskDataProc> reduceproc = boost::make_shared<ReduceTaskDataProc>(dataproctype);
		boost::shared_ptr<DataRecordCtnr> recordCtnrObj = boost::make_shared<DataRecordCtnr>();
		/*
		boost::shared_ptr<DataRecord> recordObj = boost::make_shared<DataRecord>();

		int curFieldLen = 0;
		int curOffset = 0;
		for (size_t i=0; i<v.size(); i++)
		{
			OmnString field_type = v[i]->getAttribute("type");
			OmnString field_name = v[i]->getAttribute("zky_name");
			v[i]->setAttribute("zky_offset", curOffset);
			curFieldLen = atoi((v[i]->getAttribute("zky_length")).c_str());
			if (curFieldLen == 0)
			{
				curFieldLen = 20;
				v[i]->setAttribute("zky_length", curFieldLen);
			}
			curOffset += curFieldLen;
			recordObj->setField(v[i]);
		}
		*/

		boost::shared_ptr<DataField> docidFieldObj = boost::make_shared<DataField>();
		docidFieldObj->setAttribute("zky_name", "zky_docid");
		docidFieldObj->setAttribute("type", "docid");
		datarecord->setField(docidFieldObj);
		datarecord->setAttribute("zky_docid", "0");
		/*
		recordObj->setField(docidFieldObj);
		

		map<string, string> recordAttrs = datarecord->getAttributes();
		for (map<string, string>::iterator itr = recordAttrs.begin();
				itr != recordAttrs.end(); itr++)
		{
			recordObj->setAttribute(itr->first, itr->second);
		}

		if (recordtype == "fixbin")
		{
			aos_assert(curOffset > 0);
			recordObj->setAttribute("zky_length", curOffset);
		}
		*/

		OmnString recordCtnrObjName = "";
		recordCtnrObjName << recordname<< "_new";
		datarecord->setAttribute("zky_name", recordCtnrObjName);
		recordCtnrObj->setRecord(datarecord);
		recordCtnrObj->setAttribute("zky_name", recordCtnrObjName);
		recordCtnrObj->setAttribute("zky_docid", "0");

		reduceproc->setRecord(recordCtnrObj);
		reduceproc->setAttribute("datasetname", tablename);
		reduceproc->setAttribute("type", dataproctype);

		string dcltype = "doc";
		reduceproc->setCollectorType(dcltype);

		reduceproc->setCollectorAttr("zky_name", tablename);
		reduceproc->setAssemblerType(asmtype);

		mReduceDataProcs.push_back(reduceproc);
	}
}


AosJLDoc::AosJLDoc(int version)
:AosJimoLogic(1)
{
}


AosJLDoc::~AosJLDoc()
{
}
