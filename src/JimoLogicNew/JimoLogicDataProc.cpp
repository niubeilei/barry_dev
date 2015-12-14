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
#include "JimoLogicNew/JimoLogicDataProc.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"



AosJimoLogicDataProc::AosJimoLogicDataProc(const int version)
:AosJimoLogicNew(version)
{

}


AosJimoLogicDataProc::~AosJimoLogicDataProc()
{

}

/*
bool 
AosJimoLogicDataProc::getOutputsFromDataProc(
		AosRundata* rdata,
		AosJimoProgObj *jimo_prog,
		const vector<OmnString> &procNames,
		vector<OmnString> &outputs)
{
	aos_assert_rr(procNames.size()>0, rdata, false);
	for(size_t i = 0; i < procNames.size();i++)
	{
		AosJimoLogicObjNew* logic = jimo_prog->getJimoLogic( rdata, procNames[i]);
		if(!logic)
		{
		//	AosLogError(rdata, true, "please_checkout_data_proc_name is Exist");
			return false;
		}
		vector<OmnString> output_names = logic->getOutputNames();
		outputs.insert(outputs.end(),output_names.begin(),output_names.end());
	}
	return true;
}
*/
