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
#include "DataCalc/DataCalc.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/DataCalcObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

bool DataCalcDynTester()
{
	AosDataCalc dd("", "", "", "");
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	AosDLLObjPtr obj = AosCreateDLLObj(rdata, 0, 
			"libDataCalcWordCount.so", "AosCreateDataCalcWordCount", "");
	aos_assert_r(obj, false);
	OmnScreen << "DLL Object created" << endl;
	AosDataCalcObjPtr calc = obj->convertToDataCalc(rdata);
	aos_assert_r(calc, false);
	AosValueRslt output;
	bool rslt = calc->run(rdata, 0, 0, output);
	aos_assert_r(rslt, false);
	return true;
}


