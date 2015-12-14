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
// A Job is defined as:
// 	Create JimoJob job_name
// 	(
// 		parm_name,
// 		parm_name,
// 		....
//  );
//
// It will create the following:
//
// Modification History:
// 2015/03/22 Created by Chen Ding
// 2015/05/20 Worked on by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcNew.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"


AosJimoLogicDataProcNew::AosJimoLogicDataProcNew(const int version)
:
//AosJimoLogicDataProc(version)
AosJimoLogicDataProc(version),
AosJimoDataProc(version, AosJimoType::eDataProc)
{
}


AosJimoLogicDataProcNew::~AosJimoLogicDataProcNew()
{
}


bool 
AosJimoLogicDataProcNew::run(
			AosRundata *rdata, 
			AosJimoProgObj *prog,
			OmnString &statement_str,
			bool inparser)
{
	return generateCode(rdata, prog, statement_str);
}

bool
AosJimoLogicDataProcNew::generateCode(
			AosRundata *rdata, 
			AosJimoProgObj *prog,
			OmnString &statement_str)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosJimoLogicDataProcNew::generateDataProcCode(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString dpname,
		OmnString &statement_str)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dpconf = "";
	AosJqlStatementPtr stmt = dynamic_cast<AosJqlStatement*>(this);
	OmnString jql = stmt->getOrigStatement(); 
	dpconf
		<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
		<<  AOSTAG_OBJID << "=\"" << objid << "\">"
		<<      "<dataproc type=\"jimo_parser\" zky_name=\"" << dpname << "\">"
		<<          "<![CDATA[" << jql << ";" << "]]>"
		<<      "</dataproc>"
		<< "</jimodataproc>";
	//return AosCreateDoc(dpconf, true, rdata);

	prog->saveLogicDoc(rdata, objid, dpconf);
	return true;
}

bool
AosJimoLogicDataProcNew::generateTaskCode(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					const OmnString objname,
					OmnString &dpName,
					const OmnString dataEnginType,
					vector<OmnString> inputs)
{
	OmnString task_str;
	OmnString taskName = "task_";
	taskName << objname << "_" << dpName;
	task_str << " {"
			<< "\"name\":" << "\"" << taskName << "\","
			<< "\"dataengine_type\":" << "\"" << dataEnginType  << "\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << dpName << "\","
			<< "\"inputs\":[";
	for(size_t i = 0; i < inputs.size(); i++)
	{
		if(i > 0)
			task_str << ",";
		task_str << "\""<< inputs[i] << "\"";
	}
	
	task_str << "]}]}";
	//add_wordcount_str << "],\"outputs\":[\"" << mDpName << "_output\""
	//				 << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks", task_str);
	OmnScreen << task_str << endl;
	return true;
}
