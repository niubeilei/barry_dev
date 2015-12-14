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
#include "SEInterfaces/DLLActionObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

bool DLLActionTester()
{
	// Run the DLL
	AosDLLActionObj dd("", "", "", "");
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	AosDLLObjPtr obj = AosCreateDLLObj(rdata, 0, 
			"libDLLTemplate.so", "AosCreateDLLActionDynTemplate", "");
	aos_assert_r(obj, false);
	OmnScreen << "DLL Object created" << endl;
	AosDLLActionObjPtr action = obj->convertToAction(rdata);
	aos_assert_r(action, false);

	OmnString ss = "<xml name=\"Chen Ding\"/>";
	AosXmlTagPtr def = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	AosXmlTagPtr sdoc;
	AosDataRecordObjPtr record;
	AosValueRslt value;
	AosBuffPtr buff;
	AosDataBlobObjPtr blob;
	char *data = 0;
	AosTaskObjPtr task;
	AosTaskDataObjPtr taskdata;

	action->serializeTo(buff, rdata);
	action->serializeFrom(buff, rdata);
	action->abortAction(rdata);
	action->run(def, rdata);
	action->run(value, rdata);
	action->run(record, rdata);
	action->run(blob, rdata);
	action->run(data, 0, rdata);
	action->run(buff, rdata);
	action->run(task, sdoc, taskdata, rdata);
	return true;
}

