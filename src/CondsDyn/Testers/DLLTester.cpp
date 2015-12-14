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
// 2013/05/06: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApi.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DLLCondObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

bool DLLCondTester()
{
	// Run the DLL
	AosDLLCondObj dd("", "", "", "");
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	AosDLLObjPtr obj = AosCreateDLLObj(rdata, 0, 
			"libDLLCondSample.so", "AosCreateDLLCondSample", "");
	aos_assert_r(obj, false);
	OmnScreen << "DLL Object created" << endl;
	AosDLLCondObjPtr cond = obj->convertToCondition(rdata);
	aos_assert_r(cond, false);

	AosXmlTagPtr tag;
	AosDataRecordObjPtr record;
	AosValueRslt value;
	cond->evalCond(tag, rdata);
	cond->evalCond(rdata);
	cond->evalCond(0, 0, rdata);
	cond->evalCond(record, rdata);
	cond->evalCond(value, rdata);

	return true;
}


