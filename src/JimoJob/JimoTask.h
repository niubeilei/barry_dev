////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoJob_JimoTask_h
#define Aos_JimoJob_JimoTask_h

#include "API/AosApiG.h"
#include "Jimo/Jimo.h"
#include "JimoJob/JimoJob.h"
#include "JimoJob/JimoDataEngine.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JSON/JSON.h"

#include <vector>
using namespace std;

class AosJimoTask
{
private:
	vector<OmnString>  			mInputs;
	//vector<OmnString>			mOutputs;
	//map<string, string>  		mInputs;
	map<string, string>			mOutputs;
	AosJimoDataEngine			mDataEngine;
	AosJimoJob *				mJob;
	OmnString					mTaskName;
	JSONValue					mDatasetSplit;
	OmnString							mPhyId;

public:
	AosJimoTask(const JSONValue	&jsonObj, AosJimoJob* job);
	AosJimoTask(const OmnString &jsonstr, AosJimoJob* job);

	void reset();
	bool init(const OmnString &jsonstr);
	bool config(const JSONValue &jsonObj);
	bool configDataprocs(const JSONValue &dps);
	OmnString getConfig(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);

	void setDataEngineType(const OmnString &type) {mDataEngine.setType(type);}
	void appendDataProc(const OmnString &dataproc) {mDataEngine.appendDataProc(dataproc);}

private:
	bool configMultiRcdDataproc(const JSONValue &dp);

};

#endif
