//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is automatically generated by the TorturerGen facility. 
//
// Modification History:
// 7/18/2007 : Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#include "AutoTorturing/src/QoS/qos_aos_qos_enable_Tester.h"

#include "CliTorturer/CliTorturer.h"
#include "CliTorturer/ApiTorturerDef.h"
#include "Torturer/Parm.h"
#include "Torturer/ParmIncls.h"
#include "TorturerConds/CondCheckTable.h"
#include "Util/VarList.h"
#include "Util/GenTable.h"
#include "Util/GenRecord.h"
#include "AutoTorturing/src/QoS/Ptrs.h"


AosDefineApiTorturer(AosClass_aos_qos_enable, AosClass_aos_qos_enablePtr, "SetQoSStatus")



bool
AosClass_aos_qos_enable::runFunc(bool &correctOnly, 
					   const AosCliTorturerPtr &torturer, 
					   const OmnTestMgrPtr &testMgr, 
					   bool &cmdCorrect)
{
	// 
	// Below are the lines to generate individual arguments
	//
	mTestMgr = testMgr;
	mExecutionFailed = false;
	mExecutionErrmsg = "";
	;

    OmnTCTrue_1(mArgs[0]->getCrtValue(mArg0), false) << endtc;
    
	 
	
		OmnString status;
		if (!torturer->getVar("QoSStatus", status))
		{
			mExecutionFailed = true;
			mExecutionErrmsg = "QoSStatus variable not defined";
			return false;
		}
	
	
	int __ret;
    if (!(AosCliTorturer::getTestOnlyFlag() || correctOnly && !cmdCorrect))
    {
        correctOnly = true;
         __ret = aos_qos_enable(mArg0);
    }
    else
    {
        correctOnly = false;
    }
    mFuncRet = "";
    mFuncRet << __ret;
	
		if (mArg0 == 0)
		{
			torturer->setVar("QoSStatus", "off");
		}
		else 
		{
			torturer->setVar("QoSStatus", "on");
		}
	
	
	return true;
}


bool    
AosClass_aos_qos_enable::preAddToTableProc(const AosGenTablePtr &table,
                    const AosGenRecordPtr &record,
					bool &cmdCorrect,       
					OmnString &cmdErrmsg)
{

	return true;
}

