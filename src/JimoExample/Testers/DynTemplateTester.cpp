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
// 2013/05/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ActionDyn/Testers/SampleTester.h"

#include "ActionDyn/Sample.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/DLLActionObj.h"
#include "Tester/TestMgr.h"


AosSampleTester::AosSampleTester()
{
	config();
}


bool 
AosSampleTester::start()
{
	AosDLLActionObjPtr action;
	AosRundataPtr rdata = OmnApp::getRundata();
	if (mIsDLL)
	{
		AosDLLActionObj dd("", "", "", "");
		rdata->setSiteid(100);
		AosDLLObjPtr obj = AosCreateDLLObj(rdata, 0, 
			"libaa.so.1", "AosCreateDLLActionSample", "");
		aos_assert_r(obj, false);
		OmnScreen << "DLL Object created" << endl;
		action = obj->convertToAction(rdata);
		aos_assert_r(action, false);
	}
	else
	{
		action = OmnNew AosDLLActionSample(rdata, 0, "no_lib", "no_method", "");
	}

	AosXmlTagPtr def, sdoc;
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


bool
AosSampleTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("actiondyn_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	mIsDLL = tag->getAttrBool("is_dll", false);
	return true;
}

#endif
