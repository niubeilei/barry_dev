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
#ifndef Aos_StreamEngine_JqlService_h
#define Aos_StreamEngine_JqlService_h

#include "API/AosApiG.h"
#include "Jimo/Jimo.h"
#include "JimoJob/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericObj.h"
#include "XmlUtil/XmlTag.h"
#include "JSON/JSON.h"
#include "StreamEngine/RDD.h"
#include "StreamEngine/StreamSnapshot.h"
#include <vector>
using namespace std;

typedef vector<OmnString> OmnStringList;

class AosStreamData;
class AosJqlService: public AosGenericObj
{
	OmnDefineRCObject;

	OmnString							mName;
	OmnString							mId;
	u64									mDocid;
	map<OmnString, JSONValue>  			mDataProcMap;
	map<OmnString, AosDataProcObjPtr>	mDataProcObjMap;
    map<OmnString, JSONValue>           mParamMap;

public:
	AosJqlService(const int ver);
	~AosJqlService();

	AosJimoPtr cloneJimo() const;

	// AosGenericObj interfaces
	virtual bool createByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr,
			const AosJimoProgObjPtr &prog);

	virtual bool runByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool showByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);
	
	virtual bool stopByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool dropByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);
	
	OmnString getObjType(AosRundata *rdata) {return "";}

	bool parseJQL(                        
			AosRundata *rdata, 
			AosJimoParserObj *jimo_parser,
			AosJimoProgObj *prog,
			bool &parsed, 
			bool dft = false) {return false;}

	bool createDataProcDoc(
			const OmnString &objName,
			JSONValue json,
			OmnString &dataprocs_str,
			const AosJimoProgObjPtr &prog,
			AosRundata *rdata);

	bool createDataProcDoc(
			const OmnString &objName,
			AosRundata *rdata);
    
	bool replaceJsonParams(
			JSONValue &json, 
			JSONValue &jsonNew);

    bool updateDataProcInputOutput(
			map<OmnString, JSONValue> &DataProcMap,
            map<OmnString, OmnStringList> &DataflowMap);

	bool createServiceDoc(
			const OmnString &objName,
			AosRundata *rdata);
};

#endif
