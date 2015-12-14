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
#ifndef Aos_JimoJob_JimoDataEngine_h
#define Aos_JimoJob_JimoDataEngine_h

#include "API/AosApiG.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JSON/JSON.h"

#include <vector>
using namespace std;

class AosJimoDataEngine
{
private:
	typedef map<string, string>::iterator 	mItr_t;

private:
	OmnString				mType;
	vector<OmnString> 		mDataProcs;
	vector<AosXmlTagPtr>	mDataSets;
	bool 					mIsIILBatchOpr;
	map<string, string>		mAttrs;

	bool 					mIsMultiRcd;
	JSONValue				mGroup;

public:
	AosJimoDataEngine();

	void reset();
	bool isMultiRcd();
	bool isJoinEngine();
	bool isIILBatchOpr();
	bool isValidType(const OmnString type);

	OmnString getType() {return mType;}
	OmnString getConfig(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	vector<OmnString>& getDataProcs() {return mDataProcs;}
	vector<AosXmlTagPtr>& getDatasets() {return mDataSets;}
	
	bool setType(const OmnString &type);
	bool setAttr(const string &name, const string &value);
	bool setGroup(const JSONValue &group);

	void isMultiRcd(bool flag){mIsMultiRcd = flag;};

	void appendDataProc(const OmnString dataproc) {mDataProcs.push_back(dataproc);}

private:

	OmnString generateGroupConf(
						const AosRundataPtr &rdata, 
						const AosJimoProgObjPtr &prog);
	OmnString getDataProcsConf(
						const AosRundataPtr &rdata,
						vector<OmnString> &data_procs, 
						const AosJimoProgObjPtr &prog);

};

#endif
