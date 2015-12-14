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
// 2014/12/11 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtSelectInto.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JQLExpr/Expr.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"
#include "SEInterfaces/NetFileMgrObj.h"

using namespace AosConf;

AosJqlStmtSelectInto::AosJqlStmtSelectInto()
{
	mFormat = 0;
	mMaxThreads = 1;
}


AosJqlStmtSelectInto::~AosJqlStmtSelectInto()
{
	if(mFormat)
	{
		delete mFormat;
	}
}

bool
AosJqlStmtSelectInto::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	bool rslt = false;
	rslt = mSelect->init(rdata);
	OmnString jobstr = "<job zky_job_version=\"1\" zky_otype=\"job\" zky_pctrs=\"jobctnr\" zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_ctnrobjid=\"task_ctnr\">";
		jobstr 	<< "<scheduler zky_jobschedulerid=\"norm\" zky_numslots=\"1\"/>"
				<< "<tasks><task zky_max_num_thread=\"" << mMaxThreads << "\" type=\"map\"></task></tasks></job>";
	AosXmlTagPtr jobxml = AosStr2Xml(rdata.getPtr(), jobstr AosMemoryCheckerArgs); 
	AosXmlTagPtr task = jobxml->xpathGetChild("tasks/task");
	u32 systemsec = OmnGetSecond();
	OmnString job_name = "selectinto_";
	job_name << systemsec;
	OmnString jname = "_zt4g_job_";
	jname << mJobName;
	jobxml->setAttr("zky_name", jname);
	jobxml->setAttr("zky_job_name", mJobName);
	task->setAttr("zky_name", job_name);
	OmnString str = "<input_datasets></input_datasets>";
	AosXmlTagPtr query_conf = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs); 
	rslt = mSelect->generateQueryConf(rdata, query_conf);
	if (!rslt)
	{
		return false;
	}
	AosXmlTagPtr query_conf_clone = query_conf->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr child = query_conf_clone->getFirstChild(true);
	aos_assert_r(child, false);
	child->setNodeName("dataset");
	OmnString psize = child->getAttrStr("psize", "");
	if(psize == "")
	{
		child->setAttr("psize", "NaN");
	}
	
	//felicia,2015/11/24, for JIMODB-1220, task.exe can't getCurDatabase
	OmnString tname = child->getAttrStr("zky_name", "");
	OmnString tmpname = getCurDatabase(rdata);
	tmpname << "." << tname;
	child->setAttr("zky_name", tmpname);
	
	task->addNode(query_conf_clone);

	OmnString output_datasets;
	output_datasets << "<output_datasets>"
					<< "<dataset zky_type=\"datacol\" trigger_type=\"datacol\" zky_name=\"dp_output\">"
					<< "<datacollector zky_name=\"dp_output\"/>"
					<< "</dataset>"
					<< "</output_datasets>";
	AosXmlTagPtr datasets_xml = AosStr2Xml(rdata.getPtr(), output_datasets AosMemoryCheckerArgs); 
	task->addNode(datasets_xml);


	AosXmlTagPtr outputrecord = task->xpathGetChild("input_datasets/dataset/outputrecord");
	OmnString output_name = outputrecord->getAttrStr("zky_name");
	AosXmlTagPtr datarecord = outputrecord->getFirstChild("datarecord");

	//set format
	OmnString zky_field_delimiter = ",";
	OmnString zky_text_qualifier = "DQM";
	OmnString zky_row_delimiter = "LF";
	if(mFormat)
	{
		zky_field_delimiter = (*mFormat)[0];
		zky_text_qualifier = (*mFormat)[1];     
		zky_row_delimiter = (*mFormat)[4];   
	}

	datarecord->setAttr("zky_name", output_name);
	datarecord->setAttr("type", "csv");
	datarecord->setAttr("zky_field_delimiter", zky_field_delimiter);
	datarecord->setAttr("zky_text_qualifier", zky_text_qualifier);
	datarecord->setAttr("zky_row_delimiter", zky_row_delimiter);

	AosXmlTagPtr datafields = datarecord->getFirstChild("datafields");
	AosXmlTagPtr datafield = datafields->getFirstChild(true);
	while(datafield)                                       
	{
	    datafield->setAttr("type", "str");
		datafield->removeAttr("zky_xpath");
		datafield = datafields->getNextChild();
	}

	if (mPath == "")
	{
		rdata->setJqlMsg("ERROR: no outfile ");
		return false;
	}
	int idx = mPath.indexOf(0, '/', true);
	if (idx != -1)
	{
		OmnString dir = mPath.substr(0, idx);
		bool exist = AosNetFileMgrObj::dirIsGoodLocalStatic(dir, false, rdata.getPtr());
		if (!exist)
		{
			OmnString tmp;
			tmp << "ERROR: dir " << dir << " not exist";
			rdata->setJqlMsg(tmp);
			return false;
		}
	}

	OmnString data_engine;
	data_engine << "<data_engine zky_type=\"dataengine_scan2\">"
                << "<dataproc zky_name=\"dp\" jimo_objid=\"dataprocselectinto_jimodoc_v0\"><![CDATA[{\"filename\": \"" << mPath << "\"}]]></dataproc>"
            	<< "</data_engine>";
	AosXmlTagPtr data_engine_xml = AosStr2Xml(rdata.getPtr(), data_engine AosMemoryCheckerArgs);
	aos_assert_r(data_engine_xml, false);
	task->addNode(data_engine_xml);

	AosXmlTagPtr jobdoc = createDoc1(rdata, jobxml->toString());
	OmnString jobobjid = jobdoc->getAttrStr(AOSTAG_OBJID);

	OmnString msg; 
	if ( !startJob(rdata, jobobjid) )
	{
		OmnString msg = "Failed to run Job .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg = "";
	msg << "Job ";
	if (mJobName != "")
	{
		msg << mJobName;
	}
	else
	{
		msg << jobobjid;
	}
	msg << " started successfully.";
	rdata->setJqlMsg(msg);
	return rslt;
}


AosJqlStatement *
AosJqlStmtSelectInto::clone()
{
	return 0;
}

void 
AosJqlStmtSelectInto::dump()
{
}

