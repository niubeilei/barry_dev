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
#include "JQLStatement/JqlStmtDataProc.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"

#include "AosConf/DataProc.h"
#include "AosConf/DataProcCompose.h"
#include "AosConf/ReduceTaskDataProc.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using boost::shared_ptr;
using namespace AosConf;

AosJqlStmtDataProc::AosJqlStmtDataProc()
{
	mProcName = "";
	mProcType = "";
	mInputs = 0;
	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtDataProc::~AosJqlStmtDataProc()
{
	OmnDelete mInputs;
}

bool
AosJqlStmtDataProc::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createDataProc(rdata);
	if (mOp == JQLTypes::eShow) return showDataProcs(rdata);
	if (mOp == JQLTypes::eDrop) return dropDataProc(rdata);
	if (mOp == JQLTypes::eDescribe) return describeDataProc(rdata);

	AosSetEntityError(rdata, "JQL_schema_run_err", "JQL DataProc", "")
		<< "opr is undefined!" << enderr;                               
	return false;                                                           
}


bool
AosJqlStmtDataProc::createDataProc(const AosRundataPtr &rdata)
{
	//check if the dataset already exists
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataProcDoc, mProcName);
	AosXmlTagPtr doc = convertToXml(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	doc->setAttr(AOSTAG_OBJID, objid);
	doc->setAttr(AOSTAG_PUBLIC_DOC, "true");                     
	doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");                    
	
	if ( !createDoc(rdata, JQLTypes::eDataProcDoc, mProcName, doc) )
	{
		msg << "Failed to create DataProc doc." ;
		rdata->setJqlMsg(msg);
		return false;
	}

	msg << "Data Proc " << mProcName << " created successfully.";
	rdata->setJqlMsg(msg);
	return true;
}


AosXmlTagPtr
AosJqlStmtDataProc::convertToXml(const AosRundataPtr &rdata)
{
	boost::shared_ptr<DataProc> proc;
	OmnString input_name;
	
	if (mConfParms) 
	{
		boost::shared_ptr<DataProc> procObj = boost::make_shared<DataProc>();
		return getConf(rdata, procObj, mConfParms);
	}

	if (mProcType == "compose")
	{
		boost::shared_ptr<DataProcCompose> proc_comp = boost::make_shared<DataProcCompose>();
		proc_comp->setAttribute("zky_name", mProcName);
		proc_comp->setAttribute("zky_otype", "dataproc");
		proc_comp->setAttribute("type", "compose");

		for (u32 i = 0; i < mInputs->size(); i++)                    
		{                                                                     
			input_name = ((*mInputs)[i])->getValue(rdata.getPtrNoLock());
			proc_comp->setInput(input_name);
		}

		proc_comp->setOutput(mOutput);
		proc = proc_comp;
	}
	if (mProcType == "index")
	{
		boost::shared_ptr<ReduceTaskDataProc> reduce_proc = boost::make_shared<ReduceTaskDataProc>();
		reduce_proc->setAttribute("build_bitmap", "false");
		reduce_proc->setAttribute("zky_name", mProcName);
		reduce_proc->setAttribute("zky_otype", "dataproc");
		reduce_proc->setAttribute("type", "iilbatchopr");
		reduce_proc->setAttribute("datasetname", mDatasetName);
		reduce_proc->setAttribute("zky_iilname", mIILName);
		reduce_proc->setCollectorAttr("zky_name", mDatasetName);
		reduce_proc->setCollectorType("iil");
		reduce_proc->setAssemblerType(mOperator);
		AosXmlTagPtr recordDoc = getDoc(rdata, JQLTypes::eDataRecordDoc, mDataRecordName);
		if (!recordDoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		} 
		reduce_proc->setRecord(recordDoc->toString());
		proc = reduce_proc;
	}
	if (mProcType == "doc")
	{
		boost::shared_ptr<ReduceTaskDataProc> reduce_proc = boost::make_shared<ReduceTaskDataProc>();
		reduce_proc->setAttribute("zky_name", mProcName);
		reduce_proc->setAttribute("zky_otype", "dataproc");
		reduce_proc->setAttribute("type", "importdoc_fixed");
		reduce_proc->setAttribute("datasetname", mDatasetName);
		reduce_proc->setCollectorAttr("zky_name", mDatasetName);
		reduce_proc->setCollectorType("doc");
		reduce_proc->setAssemblerType("docnorm");

		AosXmlTagPtr recordDoc = getDoc(rdata, JQLTypes::eDataRecordDoc, mDataRecordName);
		if (!recordDoc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return NULL;
		} 
		reduce_proc->setRecord(recordDoc->toString());
		proc = reduce_proc;
	}
	proc->setAttribute(AOSTAG_PUBLIC_DOC, "true");
	proc->setAttribute(AOSTAG_CTNR_PUBLIC, "true");
	proc->setAttribute(AOSTAG_PUB_CONTAINER, AOSCTNR_PROC_CONTAINER);

	return AosXmlParser::parse(proc->getConfig() AosMemoryCheckerArgs); 
}

bool
AosJqlStmtDataProc::showDataProcs(const AosRundataPtr &rdata)
{
	OmnString msg;
	vector<AosXmlTagPtr> procs;
	if ( !getDocs(rdata, JQLTypes::eDataProcDoc, procs) )
	{
		msg << "No Data Proc Found. " ;
		rdata->setJqlMsg(msg);
		return true;
	}

	int total = procs.size() ; 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<procs.size(); i++)
	{
		OmnString vv = procs[i]->getAttrStr("zky_name", "");
		if (vv != "")
		{
			content << "<record procname=\"" << vv << "\"/>";
		}
	}

	content << "</content>";
	rdata->setResults(content);
	if (mContentFormat == "xml")
	{
		rdata->setResults(content);
		return true;
	}

	vector<OmnString> fields;
	fields.push_back("procname");
	map<OmnString, OmnString> alias_name;
	alias_name["procname"] = "DataPrcoName";

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);

	return true;
}

bool 
AosJqlStmtDataProc::dropDataProc(const AosRundataPtr &rdata)
{
	OmnString msg;
	OmnString objid = getObjid(rdata, JQLTypes::eDataProcDoc, mProcName);                  
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                        
		msg << "Failed to delete data proc doc.";  
		rdata->setJqlMsg(msg);
		return false;                                        
	}   
	msg << "Data Proc " << mProcName << " dropped successfully."; 
	rdata->setJqlMsg(msg);
                                                     
	return true;
}


bool
AosJqlStmtDataProc::describeDataProc(const AosRundataPtr &rdata)
{
	if (mProcName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg;
	string objid = getObjid(rdata, JQLTypes::eDataProcDoc, mProcName);                                   
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{                                                                      
		msg << "Failed to display data proc " << mProcName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;                                                      
	}                                                                      

	mProcType = doc->getAttrStr("type");
	vector<OmnString> fields;
	map<OmnString, OmnString> alias_name;
	if (mProcType == "compose")
	{
		fields.push_back("zky_name");
		fields.push_back("type");

		AosXmlTagPtr input_fields = doc->getFirstChild("input");
		if (!input_fields)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosXmlTagPtr input_field = input_fields->getFirstChild(true); 
		if (!input_field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		int idx = 1;
		OmnString field_name;
		OmnString tmp_name;
		OmnString tag_name = "zky_input_field_name";

		while (input_field)
		{ 
			field_name = input_field->getAttrStr(tag_name);
			tmp_name << tag_name << idx;
			fields.push_back(tmp_name);
			alias_name[tmp_name] << "FieldName_" << idx;
			doc->setAttr(tmp_name, field_name);
			input_field = input_fields->getNextChild();
			tmp_name = ""; 
			idx++;
		}                                                         
		alias_name["zky_name"] = "Name";
		alias_name["type"] = "Type";                             

		fields.push_back("zky_output_field_name"); 
		alias_name["zky_output_field_name"] = "OutputFieldName";
		
		AosXmlTagPtr output_fields = doc->getFirstChild("output");
		if (!output_fields)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosXmlTagPtr output_field = output_fields->getFirstChild(true); 
		if (!output_field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		OmnString v = output_field->getAttrStr("zky_output_field_name");
		doc->setAttr("zky_output_field_name", v);

		OmnString content = "<content>";
		content << doc->toString() << "</content>";
		if (mContentFormat == "xml")
		{
			rdata->setResults(doc->toString());
			return true;
		}

		AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
		msg = printTableInXml(fields, alias_name, def);
		setPrintData(rdata, msg);
		return true;
	}

	fields.push_back("zky_name");
	fields.push_back("type");

	if (mProcType == "iilbatchopr")
	{
		fields.push_back("zky_iilname");
	}

	fields.push_back("datasetname");
	fields.push_back("asm_type");
	fields.push_back("zky_name");

	alias_name["zky_name"] = "Name";
	alias_name["type"] = "Type";
	alias_name["datasetname"] = "DatasetName";
	alias_name["asm_type"] = "AsmType";
	//alias_name["zky_datarecord_name"] = "DataRecordName";
	alias_name["zky_iilname"] = "IndexName";

	AosXmlTagPtr node = doc->getFirstChild(true);
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	node = node->getFirstChild(true);
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString vv = node->getAttrStr("type");
	doc->setAttr("asm_type", vv);

	node = node->getFirstChild(true);
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	vv = node->getAttrStr("zky_name");
	doc->setAttr("zky_name", vv);

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
AosJqlStmtDataProc::clone()
{
	return OmnNew AosJqlStmtDataProc(*this);
}


void 
AosJqlStmtDataProc::dump()
{
	//OmnScreen << "JQL Check Doc Exist Statement: " << "" << endl;
}


/***************************
 * Getter/setter
 * **************************/
void
AosJqlStmtDataProc::setName(OmnString name)
{
	mProcName = name;
}


void
AosJqlStmtDataProc::setType(OmnString type)
{
	mProcType = type;
}


void 
AosJqlStmtDataProc::setInputs(AosExprList* inputs)
{
	mInputs = inputs;
}


void 
AosJqlStmtDataProc::setOutput(OmnString output)
{
	mOutput = output;
}


void 
AosJqlStmtDataProc::setOutputAsName(OmnString output)
{
	mOutputAsName = output;
}


void
AosJqlStmtDataProc::setDataRecordName(OmnString data_record_name)
{
	mDataRecordName = data_record_name;
}


void
AosJqlStmtDataProc::setDatasetName(OmnString dataset_name)
{
	mDatasetName = dataset_name;
}


void
AosJqlStmtDataProc::setOperator(OmnString opt)
{
	mOperator = opt;
}

void 
AosJqlStmtDataProc::setIILName(OmnString iil_name)
{
	mIILName = iil_name;
}

void
AosJqlStmtDataProc::setDocType(OmnString doc_type)
{
	mDocType = doc_type;
}

